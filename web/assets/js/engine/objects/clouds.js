define(
    ['pixi'],
    function(PIXI) {
        function Clouds(width, height) {
            this._c = document.createElement('canvas');
            this._c.width =  width;
            this._c.height = height;
            var ctx = this._c.getContext('2d');

            this._cloudsColors = function (a){
                return a[Math.floor(Math.random() * a.length)];
            }([
                [ '#4A1730', '#582143', '#550101', '#11091E' ],
                [ '#221587', '#372589', '#0C3C93', '#4646B4', '#11091E' ],
                [ '#4F8585', '#1E4C4C', '#006183', '#1D7D6D', '#11091E' ],
                [ '#742779', '#7C5578', '#18022B', '#11091E' ]
            ]);
            while (!this._isEnoughClouds()) {
                var r = Math.random() * 400 + 200;
                ctx.drawImage(
                    this._genCloudCanvas(r, this._cloudsColors[Math.floor(this._cloudsColors.length * Math.random())]),
                    width * Math.random() - r, height * Math.random() - r
                );
            }

            PIXI.Sprite.call(this, PIXI.Texture.fromCanvas(this._c));

            var twistFilter = new PIXI.filters.TwistFilter();
            twistFilter.angle = (Math.random() - .5) * Math.PI;
            twistFilter.radius = 0.5;
            this.filters = [twistFilter];

            this._c = null;
        }

        Clouds.prototype = Object.create(PIXI.Sprite.prototype);
        Clouds.prototype.constructor = Clouds;

        Clouds.prototype._isEnoughClouds = function() {
            var image = this._c.getContext('2d').getImageData(0, 0, this._c.width, this._c.height);
            var sum = 0, count = 10, dx = Math.floor(this._c.width / count), dy = Math.floor(this._c.height / count);
            for (var i = 0; i < count; i++)
                for (var j = 0; j < count; j++)
                    sum += image.data[(this._c.width * dy * j + dx * i) * 4 + 3];
            return sum / (count * count) > 127;
        };

        var circleCount = 15;
        Clouds.prototype._genCloudCanvas = function(radius, color) {
            var c = document.createElement('canvas');
            c.width =  radius * 2;
            c.height = radius * 2;
            var ctx = c.getContext('2d');
            var circleRadius = radius * 0.4;
            var circleC = this._genCircleCanvas(circleRadius, color);
            for (var i = 0; i < circleCount; i++) {
                var angle = Math.random() * Math.PI * 2;
                var x = Math.random() * Math.cos(angle) * (radius - circleRadius) + radius;
                var y = Math.random() * Math.sin(angle) * (radius - circleRadius) + radius;
                ctx.drawImage(circleC, x - circleRadius, y - circleRadius);
            }

            ctx.globalCompositeOperation = 'destination-out';
            ctx.beginPath();
            ctx.translate(radius, radius);
            ctx.rotate(Math.random() * Math.PI);
            var t = 3/4;
            ctx.moveTo(radius * (t - 1), -radius);
            function r() { return radius * (2 - t) * Math.random(); }
            var x1 = radius * (t - 1) + r(), y1 = -radius + r();
            var x2 = radius - r(), y2 = radius * (1 - t) - r();
            ctx.bezierCurveTo(x1, y1, x2, y2, radius, radius * (1 - t));
            ctx.lineTo(radius, -radius);
            ctx.lineTo(radius * (t - 1), -radius);
            ctx.fill();

            return c;
        };

        function hexToRgb(hex) {
            var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
            return result ? {
                r: parseInt(result[1], 16),
                g: parseInt(result[2], 16),
                b: parseInt(result[3], 16)
            } : null;
        }

        Clouds.prototype._genCircleCanvas = function(radius, color) {
            var c = document.createElement('canvas');
            var ctx = c.getContext('2d');
            c.width =  radius * 2;
            c.height = radius * 2;
            var grd = ctx.createRadialGradient(radius, radius, radius, radius, radius, 0);
            grd.addColorStop(0, 'transparent');
            var r = hexToRgb(color);
            grd.addColorStop(1, 'rgba(' + r.r + ', ' + r.g + ', ' + r.b + ', 0.15)');
            ctx.fillStyle = grd;
            ctx.arc(radius, radius, radius, 0, Math.PI * 2);
            ctx.fill();
            return c;
        };

        return Clouds;
    }
);
