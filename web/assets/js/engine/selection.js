define(
    ['pixi'],
    function(PIXI) {
        function Selection() {
            PIXI.Graphics.call(this);
        }

        Selection.prototype = Object.create(PIXI.Graphics.prototype);
        Selection.prototype.constructor = Selection;

        var borderWidth = 4;
        var offset = 6;
        var lOffset = offset + borderWidth / 2;

        Selection.prototype.drawPlanet = function(pl) {
            this.lineStyle(borderWidth, 0xFFFFFF, 0.5);
            this.drawCircle(pl.x, pl.y, pl.radius + offset);
        };

        Selection.prototype.drawLine = function(pl, pt) {
            this.lineStyle(borderWidth, 0xFFFFFF, 0.5);
            var dest = new PIXI.Point(pt.x - pl.x, pt.y - pl.y);
            var len = Math.sqrt(dest.x * dest.x + dest.y * dest.y);
            var n = new PIXI.Point(dest.x / len, dest.y / len);

            this.moveTo(pl.x + n.x * (pl.radius + lOffset), pl.y + n.y * (pl.radius + lOffset));
            if (pt.radius)
                this.lineTo(pt.x - n.x * (pt.radius + lOffset), pt.y - n.y * (pt.radius + lOffset));
            else
                this.lineTo(pt.x, pt.y);
        };

        Selection.prototype.drawSelection = function(p1, p2) {
            this.lineStyle(borderWidth, 0xFFFFFF, 0.5);
            this.drawRect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
        };

        return Selection;
    }
);
