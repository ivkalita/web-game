define(
    ['pixi', './objects/planet', './objects/ship', 'pixi/pixiTouchOverOutPatch'],
    function(PIXI, Planet, Ship) {
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

            this._selectDraw = new PIXI.Graphics();
            this._stage.addChild(this._selectDraw);

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
            var borderWidth = 4;
            var offset = 6;
            this._selectDraw.clear();
            this._selectDraw.lineStyle(borderWidth, 0xFFFFFF, 0.5);
            this._planetsSelected.forEach(function(pl){
                this._selectDraw.drawCircle(pl.x, pl.y, pl.radius + offset);
            }, this);
            if (this._target) {
                var lOffset = offset + borderWidth / 2;
                var pt = this._target;

                this._planetsSelected.forEach(function(pl){
                    if (pl == pt) return;
                    var dest = new PIXI.Point(pt.x - pl.x, pt.y - pl.y);
                    var len = Math.sqrt(dest.x * dest.x + dest.y * dest.y);
                    var n = new PIXI.Point(dest.x / len, dest.y / len);

                    this._selectDraw.moveTo(pl.x + n.x * (pl.radius + lOffset), pl.y + n.y * (pl.radius + lOffset));
                    if (pt.radius)
                        this._selectDraw.lineTo(pt.x - n.x * (pt.radius + lOffset), pt.y - n.y * (pt.radius + lOffset));
                    else
                        this._selectDraw.lineTo(pt.x, pt.y);
                }, this);
                if (pt.radius && !this._planetsSelected.has(pt))
                    this._selectDraw.drawCircle(pt.x, pt.y, pt.radius + offset);
            }
            if (p1 && p2) {
                this._selectDraw.lineStyle(2, 0xFFFFFF, 0.5);
                this._selectDraw.drawRect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
            }
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
            var min = new PIXI.Point(Math.min(p1.x, p2.x), Math.min(p1.y, p2.y));
            var max = new PIXI.Point(Math.max(p1.x, p2.x), Math.max(p1.y, p2.y));
            Object.keys(this._planets).forEach(function (id) {
                var p = this._planets[id].position;
                var f = min.x <= p.x && min.y <= p.y && p.x <= max.x && p.y <= max.y;
                if (f && this._planetsOwned.has(this._planets[id]))
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
