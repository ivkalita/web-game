define(
    ['pixi'],
    function(PIXI) {
        var radius = 2;
        var c = document.createElement('canvas');
        c.width = radius * 2;
        c.height = radius * 2;
        var ctx = c.getContext('2d');

        var grd = ctx.createRadialGradient(radius, radius, radius, radius, radius, 0);
        grd.addColorStop(0, "transparent");
        grd.addColorStop(1, "#ffffff");

        ctx.fillStyle = grd;
        ctx.fillRect(0, 0, radius * 2, radius * 2);

        var texture = PIXI.Texture.fromCanvas(c);

        function Star(maxX, maxY, moving, shining) {
            PIXI.Sprite.call(this, texture);
            this._maxX = maxX;
            this._maxY = maxY;
            this._moving = moving;
            this._shining = shining;

            this._gen();
            this.alpha = Math.random();
        }

        Star.prototype = Object.create(PIXI.Sprite.prototype);
        Star.prototype.constructor = Star;

        Star.prototype._gen = function() {
            this.x = Math.random() * this._maxX;
            this.y = Math.random() * this._maxY;
            if (this._moving) {
                this._dx = (Math.random() - 0.5) * 0.75;
                this._dy = (Math.random() - 0.5) * 0.75;
            }
            if (this._shining) {
                this._dAlpha = Math.random() * 0.01;
                this._maxAlpha = Math.random();
            }
        };

        Star.prototype.update = function() {
            if (this._moving) {
                this.x += this._dx;
                this.y += this._dy;
                if (this.x < 0) this.x = this._maxX;
                if (this.y < 0) this.y = this._maxY;
                if (this.x > this._maxX) this.x = 0;
                if (this.y > this._maxY) this.y = 0;
            }
            if (this._shining) {
                if (this.alpha + this._dAlpha > this._maxAlpha) this._dAlpha *= -1;
                if (this.alpha + this._dAlpha < 0) this._gen();
                this.alpha += this._dAlpha;
            }
        };

        return Star;
    }
);
