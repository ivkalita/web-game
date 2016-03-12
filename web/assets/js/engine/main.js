define(
    ['pixi', './objects/planet', './objects/ship', './selection', './tools', './background', 'pixi/pixiTouchOverOutPatch'],
    function(PIXI, Planet, Ship, Selection, Tools, Background) {
        function Engine(width, height, sendShipsHandler, noWebGL) {
            this._renderer = PIXI.autoDetectRenderer(width, height, { antialias: true }, noWebGL);
            this.view = this._renderer.view;
            this._sendShipsHandler = sendShipsHandler;
            this._sendShipsPercent = 0.5;
            this._planets = {};
            this._planetsSelected = new Set();
            this._planetsOwned = new Set();
            this._ships = [];

            this._stage = new PIXI.Container();
            this._background = new Background(width, height);
            this._stage.addChild(this._background);

            this._selectEventLayer = new PIXI.Graphics();
            this._selectEventLayer.beginFill(0x000000, 0);
            this._selectEventLayer.drawRect(0, 0, width, height);
            this._selectEventLayer.interactive = true;

            this._selectEventLayer
                .on('mousedown', this._onDragStart, this)
                .on('touchstart', this._onDragStart, this)
                .on('mouseup', this._onDragEnd, this)
                .on('mouseupoutside', this._onDragEnd, this)
                .on('touchend', this._onDragEnd, this)
                .on('touchendoutside', this._onDragEnd, this)
                .on('mousemove', this._onDragMove, this)
                .on('touchmove', this._onDragMove, this);

            this._stage.addChild(this._selectEventLayer);

            this._objectsLayer = new PIXI.Container();
            this._objectsLayer.interactive = true;
            this._stage.addChild(this._objectsLayer);

            this._selectDraw = new Selection();
            this._stage.addChild(this._selectDraw);

            this._stage.addChild(new Tools(this));

            this._animate();

            var allSelectedByPlanet = null;
            var engine = this;
            this._dragging = false;
            this._planetHandlers = {
                down: function(){
                    engine._dragging = true;
                    var f = false;
                    if (engine._planetsSelected.size == 1 && engine._planetsSelected.has(this)) {
                        engine._planetsSelected = new Set(engine._planetsOwned);
                        allSelectedByPlanet = this;
                        engine._target = this;
                        f = true;
                    } else if (engine._planetsOwned.has(this)) {
                        engine._planetsSelected.add(this);
                        f = true;
                    }
                    if (f) {
                        engine._updatePlanetsInteractive();
                        engine._drawSelection();
                    }
                },
                up: function(){
                    engine._dragging = false;
                    if (engine._planetsSelected.size == 0) return;
                    if (engine._planetsSelected.size == 1 && engine._planetsSelected.has(this)) return;
                    if (allSelectedByPlanet) {
                        var f = allSelectedByPlanet == this;
                        allSelectedByPlanet = null;
                        if (f) return;
                    }
                    engine._sendShips(this);
                    engine._planetsSelected.clear();
                    engine._target = null;
                    if (!engine._planetsOwned.has(this)) this.emit('mouseout');
                    engine._updatePlanetsInteractive();
                    engine._drawSelection();
                },
                over: function(event){
                    if (engine._planetsSelected.size == 0) return;
                    engine._target = this;
                    if (engine._dragging && engine._planetsOwned.has(this))
                        engine._planetsSelected.add(this);
                    engine._drawSelection();
                },
                out: function(event){
                    if (!engine._target) return;
                    if (engine._target.radius == 0) return;
                    engine._target = null;
                    engine._drawSelection();
                }
            }
        }

        Engine.prototype._animate = function(){
            this._background.update();
            this._renderer.render(this._stage);
            requestAnimationFrame(this._animate.bind(this));
        };

        Engine.prototype._sendShips = function(target){
            this._planetsSelected.forEach(function(pl){
                if (pl.id == target.id) return;
                this._sendShipsHandler(pl.id, target.id, Math.ceil(pl.count * this._sendShipsPercent));
            }, this);
        };

        Engine.prototype._drawSelection = function(p1, p2) {
            this._selectDraw.clear();
            this._planetsSelected.forEach(this._selectDraw.drawPlanet.bind(this._selectDraw));
            if (this._target) {
                this._planetsSelected.forEach(function(pl){
                    if (pl == this._target) return;
                    this._selectDraw.drawLine(pl, this._target);
                }, this);
                if (this._target.radius && !this._planetsSelected.has(this._target))
                    this._selectDraw.drawPlanet(this._target);
            }
            if (p1 && p2)
                this._selectDraw.drawSelection(p1, p2);
        };

        Engine.prototype._updatePlanetsInteractive = function() {
            Object.keys(this._planets).forEach(function(key) {
                this._planets[key].interactive = this._planetsSelected.size > 0 ||
                    this._planetsOwned.has(this._planets[key]);
            }, this);
        };

        Engine.prototype.update = function(msg, playerId) {
            function ownerColor(owner) { return owner & 0xFFFFFF; }
            this._planetsOwned.clear();
            msg.planets.forEach(function(pl){
                if (!this._planets[pl.id]) {
                    this._planets[pl.id] = new Planet(pl.id, pl.x, pl.y, pl.radius, ownerColor(pl.owner),
                        pl.ships_sum, this._planetHandlers);
                    this._objectsLayer.addChild(this._planets[pl.id]);
                } else
                    this._planets[pl.id].update(ownerColor(pl.owner), pl.ships_sum);
                if (pl.owner == playerId)
                    this._planetsOwned.add(this._planets[pl.id]);
            }, this);
            if (Array.from) {
                this._planetsSelected = new Set(Array.from(this._planetsOwned).filter(function (pl) {
                    return this._planetsSelected.has(pl);
                }, this));
            }
            this._drawSelection();
            this._updatePlanetsInteractive();

            msg.ships.forEach(function(sh, i){
                if (i < this._ships.length)
                    this._ships[i].update(sh.x, sh.y, ownerColor(sh.owner));
                else {
                    this._ships.push(new Ship(sh.x, sh.y, ownerColor(sh.owner)));
                    this._objectsLayer.addChild(this._ships[this._ships.length - 1]);
                }
            }, this);
            for (var i = msg.ships.length; i < this._ships.length; i++)
                this._ships[i].visible = false;
        };

        Engine.prototype._onDragStart = function(event) {
            this._selectBeginPoint = event.data.global.clone();
        };

        Engine.prototype._selectionUpdate = function(p1, p2) {
            this._planetsSelected.clear();
            Object.keys(this._planets).forEach(function (id) {
                if (this._planetsOwned.has(this._planets[id]) && this._planets[id].intersects(p1, p2))
                    this._planetsSelected.add(this._planets[id]);
            }, this);
            this._target = null;
        };

        Engine.prototype._onDragMove = function(event){
            if (this._selectBeginPoint) {
                this._selectionUpdate(this._selectBeginPoint, event.data.global);
                this._drawSelection(this._selectBeginPoint, event.data.global);
            }
            if (this._dragging && this._planetsSelected.size) {
                if (!this._target || !this._target.radius) this._target = event.data.global.clone();
                this._drawSelection();
            }
        };

        Engine.prototype._onDragEnd = function(event) {
            if (this._selectBeginPoint) {
                this._selectionUpdate(this._selectBeginPoint, event.data.global);
                this._drawSelection();
                this._updatePlanetsInteractive();
                this._selectBeginPoint = null;
            }
            if (this._dragging) {
                this._target = null;
                this._drawSelection();
                this._dragging = false;
            }
        };

        return Engine;
    }
);
