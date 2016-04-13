define(
    ['pixi'],
    function(PIXI) {
        var side;
        var brd;

        function Ship(x, y, angle, color, scale){
            PIXI.Sprite.call(this);
            this.anchor.set(0.5, 0.5);
            this.update(x, y, angle, color, scale);

        }

        Ship.prototype = Object.create(PIXI.Sprite.prototype);
        Ship.prototype.constructor = Ship;

        Ship.prototype.update = function(x, y, angle, color, scale){
            this.x = x;
            this.y = y;
            side = scale;
            brd = scale / 4;
            this.rotation = angle;
            this.visible = true;
            if (this.texture.color != color)
                this.texture = this._getTexture(color);
        };

        var textures = {};
        Ship.prototype._getTexture = function(color){
            if (textures[color]) return textures[color];

            var c = document.createElement('canvas');
            c.width = c.height = side;
            var ctx = c.getContext('2d');
            ctx.strokeStyle = '#' + (color.toString(16)).substr(-6);
            ctx.lineWidth = brd;
            ctx.moveTo(brd, brd);
            ctx.lineTo(side - brd, side / 2);
            ctx.lineTo(brd, side - brd);
            ctx.lineTo(side * 0.3, side / 2);
            ctx.lineTo(brd, brd);
            ctx.lineTo(side - brd, side / 2);
            ctx.stroke();

            textures[color] = new PIXI.Texture.fromCanvas(c);
            textures[color].color = color;
            textures[color].width = textures[color].height = side;
            return textures[color];
        };

        return Ship;
    }
);
