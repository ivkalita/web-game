define(
    ['pixi'],
    function(PIXI) {
        function Ship(x, y, color){
            PIXI.Graphics.call(this);
            this.update(x, y, color);
        }

        Ship.prototype = Object.create(PIXI.Graphics.prototype);
        Ship.prototype.constructor = Ship;

        Ship.prototype.update = function(x, y, color){
            this.x = x;
            this.y = y;
            this.visible = true;
            if (this.color != color) {
                this.clear();
                this.beginFill(color);
                this.drawRect(-3, -3, 3, 3);
                this.endFill();
                this.color = color;
            }
        };

        return Ship;
    }
);
