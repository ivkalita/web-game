define(
    ['pixi'],
    function(PIXI) {
        function Planet(id, x, y, radius, color, count, handlers) {
            PIXI.Container.call(this);
            Object.defineProperties(this, {
                id: { value: id, writable: false, configurable: false },
                radius: { value: radius, writable: false, configurable: false }
            });
            this.x = x;
            this.y = y;
            this.addChild(this._gr = new PIXI.Graphics());
            this._caption = new PIXI.Text('', {
                font: 'normal 18px Arial', fill: '#fff', stroke: '#000', strokeThickness: 2
            });
            this._caption.anchor.set(0.5);
            this.addChild(this._caption);
            this.update(color, count);

            this._on(['mouseover', 'touchover', 'touchstart'], [this._onMouseOver, handlers.over])
                ._on(['mouseout', 'touchout', 'touchend'], [this._onMouseOut, handlers.out])
                ._on(['mousedown', 'touchstart'], [handlers.down])
                ._on(['mouseup', 'touchend'], [handlers.up]);
        }

        Planet.prototype = Object.create(PIXI.Container.prototype);
        Planet.prototype.constructor = Planet;

        Object.defineProperty(Planet.prototype, 'count', {
            get: function() { return this._caption.text * 1; }
        });

        Planet.prototype._on = function(events, fns){
            fns.forEach(function(fn) {
                if (!fn) return;
                events.forEach(function(event){
                    this.on(event, fn);
                }, this)
            }, this);
            return this;
        };

        Planet.prototype.update = function(color, count){
            this._caption.text = count;
            if (this._color != color) {
                this._color = color;
                this._draw();
            }
        };

        function brighter(color, c) {
            return [0, 1, 2].reduce(function(t, s) {
                return t + (Math.min(((color >> s * 8) & 0xFF) + 0xFF * c, 0xFF) << s * 8)
            }, 0);
        }

        Planet.prototype._draw = function() {
            this._gr.clear();
            this._gr.beginFill(brighter(this._color, this._mouseOver ? 0.3 : 0), 1);
            this._gr.drawCircle(0, 0, this.radius);
            this._gr.endFill();
        };

        Planet.prototype._onMouseOver = function(){
            this._mouseOver = true;
            this._draw();
        };

        Planet.prototype._onMouseOut = function() {
            this._mouseOver = false;
            this._draw();
        };

        Planet.prototype.intersects = function(p1, p2) {
            var recSizeH = new PIXI.Point(Math.abs(p1.x - p2.x) / 2, Math.abs(p1.y - p2.y) / 2);
            var recCenter = new PIXI.Point(Math.min(p1.x, p2.x) + recSizeH.x, Math.min(p1.y, p2.y) + recSizeH.y);
            var dis = new PIXI.Point(Math.abs(this.x - recCenter.x), Math.abs(this.y - recCenter.y));

            if (dis.x > recSizeH.x + this.radius || dis.y > recSizeH.y + this.radius) return false;
            if (dis.x <= recSizeH.x || dis.y <= recSizeH.y) return true;
            return Math.pow(dis.x - recSizeH.x, 2) + Math.pow(dis.y - recSizeH.y, 2) <= Math.pow(this.radius, 2);
        };

        return Planet;
    }
);
