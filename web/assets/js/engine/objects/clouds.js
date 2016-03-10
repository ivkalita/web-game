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
                [[74, 23, 48], [88, 33, 67], [85, 1, 1], [17, 9, 30]],
                [[34, 21, 135], [55, 37, 137], [12, 60, 147], [70, 70, 180], [17, 9, 30]],
                [[79, 133, 133], [30, 76, 76], [0, 97, 131], [29, 125, 109], [17, 9, 30]],
                [[116, 39, 121], [124, 85, 120], [24, 2, 43], [17, 9, 30]]
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
            return c;
        };

        Clouds.prototype._genCircleCanvas = function(radius, color) {
            var c = document.createElement('canvas');
            var ctx = c.getContext('2d');
            c.width =  radius * 2;
            c.height = radius * 2;
            var grd = ctx.createRadialGradient(radius, radius, radius, radius, radius, 0);
            grd.addColorStop(0, 'transparent');
            grd.addColorStop(1, 'rgba(' + color[0] + ', ' + color[1] + ', ' + color[2] + ', 0.15)');
            ctx.fillStyle = grd;
            ctx.arc(radius, radius, radius, 0, Math.PI * 2);
            ctx.fill();
            return c;
        };

        return Clouds;
    }
);
