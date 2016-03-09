define(
    ['pixi'],
    function(PIXI) {
        function Selection() {
            PIXI.Container.call(this);
            this._gr = new PIXI.Graphics();
            this.addChild(this._gr);
        }

        Selection.prototype = Object.create(PIXI.Container.prototype);
        Selection.prototype.constructor = Selection;

        var borderWidth = 3;
        var offset = 4;
        var lOffset = offset + borderWidth / 2;
        var selectionColor = 0x00FFFF;

        Selection.prototype.drawPlanet = function(pl) {
            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.drawCircle(pl.x, pl.y, pl.radius + offset);
            this._gr.lineStyle(1, selectionColor, 1);
            this._gr.drawCircle(pl.x, pl.y, pl.radius + offset + 1);
            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.drawCircle(pl.x, pl.y, pl.radius + offset + 2);
        };

        Selection.prototype.drawLine = function(pl, pt) {
            var dest = new PIXI.Point(pt.x - pl.x, pt.y - pl.y);
            var len = Math.sqrt(dest.x * dest.x + dest.y * dest.y);
            var n = new PIXI.Point(dest.x / len, dest.y / len);

            var from = new PIXI.Point(pl.x + n.x * (pl.radius + lOffset), pl.y + n.y * (pl.radius + lOffset));
            var to = new PIXI.Point(pt.x - (pt.radius ? n.x * (pt.radius + lOffset) : 0),
                pt.y - (pt.radius ? n.y * (pt.radius + lOffset) : 0));

            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.moveTo(from.x - n.y, from.y + n.x);
            this._gr.lineTo(to.x - n.y, to.y + n.x);
            this._gr.lineStyle(1, selectionColor, 1);
            this._gr.moveTo(from.x, from.y);
            this._gr.lineTo(to.x, to.y);
            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.moveTo(from.x + n.y, from.y - n.x);
            this._gr.lineTo(to.x + n.y, to.y - n.x);
        };

        Selection.prototype.drawSelection = function(p1, p2) {
            var t1 = new PIXI.Point(Math.min(p1.x, p2.x), Math.min(p1.y, p2.y));
            var t3 = new PIXI.Point(Math.max(p1.x, p2.x), Math.max(p1.y, p2.y));
            var t2 = new PIXI.Point(t3.x - t1.x, t3.y - t1.y);

            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.drawRect(t1.x + 1, t1.y + 1, t2.x - 2, t2.y - 2);
            this._gr.lineStyle(1, selectionColor, 1);
            this._gr.drawRect(t1.x, t1.y, t2.x, t2.y);
            this._gr.lineStyle(1, selectionColor, 0.5);
            this._gr.drawRect(t1.x - 1, t1.y - 1, t2.x + 2, t2.y + 2);
        };

        Selection.prototype.clear = function() {
            this._gr.clear();
        };

        return Selection;
    }
);
