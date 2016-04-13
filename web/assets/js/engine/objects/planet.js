define(
    ['pixi'],
    function(PIXI) {
        var i, j;

        var planetTexture = new PIXI.Texture.fromCanvas(function() {
            var planetRadius = 256;
            var c = document.createElement('canvas');
            c.width = planetRadius * 2;
            c.height = planetRadius * 2;
            var ctx = c.getContext('2d');
            ctx.fillStyle = 'white';
            ctx.arc(planetRadius, planetRadius, planetRadius, 0, Math.PI * 2);
            ctx.fill();
            return c;
        }());

        var glowTexture = new PIXI.Texture.fromCanvas(function() {
            var glowRadius = 40;
            var c = document.createElement('canvas');
            c.width = glowRadius * 2;
            c.height = glowRadius * 2;
            var ctx = c.getContext('2d');
            var grd = ctx.createRadialGradient(glowRadius, glowRadius, glowRadius, glowRadius, glowRadius, glowRadius - 10);
            grd.addColorStop(0, 'transparent');
            grd.addColorStop(0.8, 'rgba(255, 255, 255, 0.5');
            grd.addColorStop(1, 'transparent');
            ctx.fillStyle = grd;
            ctx.rect(0, 0, glowRadius * 2, glowRadius * 2);
            ctx.fill();
            return c;
        }());

        var lavaTexture = new PIXI.Texture.fromCanvas(function() {
            var c = document.createElement('canvas');
            c.width = c.height = 500;
            var ctx = c.getContext('2d');
            ctx.fillStyle = '#F49053';
            ctx.shadowColor = '#F49053';
            ctx.shadowOffsetX = 0;
            ctx.shadowOffsetY = 0;
            ctx.shadowBlur = 2;
            for (i = 0; i < 200; i++) {
                ctx.beginPath();
                var beginX = Math.random() * c.width;
                var beginY = Math.random() * c.height;
                var stepMax = 8;
                ctx.moveTo(beginX, beginY);
                for (j = 0; j < 4; j++) {
                    beginX += (Math.random() - 0.5) * 2 * stepMax;
                    beginY += (Math.random() - 0.5) * 2 * stepMax;
                    ctx.lineTo(beginX, beginY);
                }
                ctx.fill();
            }
            return c;
        }());

        var linesTexture = new PIXI.Texture.fromCanvas(function() {
            var t = 30, c = document.createElement('canvas');
            c.height = c.width = 512;
            var ctx = c.getContext('2d');
            ctx.beginPath();
            ctx.strokeStyle = 'green';
            ctx.lineWidth = 1;
            for (var i = 0; i < t; i++) {
                ctx.moveTo(0, c.height / t * i);
                ctx.lineTo(c.width, c.height / t * i);
                ctx.moveTo(c.width / t * i, 0);
                ctx.lineTo(c.width / t * i, c.height);
                /*if (i >= (t / 2)) {
                    ctx.stroke();
                    ctx.beginPath();
                    ctx.strokeStyle = 'red';
                }*/
            }
            ctx.stroke();

            return c;
        }());

        var displaceTexture = new PIXI.Texture.fromCanvas(function() {
            var s = 256, o = 0;
            var c = document.createElement('canvas');
            c.width = s * 2;
            c.height = s * 2;
            var ctx = c.getContext('2d'), x, y;

            ctx.beginPath();
            ctx.fillStyle = '#808080';
            ctx.rect(0, 0, s * 2, s * 2);
            ctx.fill();

            /*ctx.beginPath();
            var radGrd = ctx.createRadialGradient(s, s, s - o, s, s, 0);
            radGrd.addColorStop(0, '#808080');
            radGrd.addColorStop(1, '#ffff00');
            //radGrd.addColorStop(1, '#898900');
            ctx.fillStyle = radGrd;
            ctx.arc(s, s, s - o, 0, Math.PI * 2);
            ctx.fill();*/

            ctx.beginPath();
            var grdRed = ctx.createLinearGradient(o, 0, s * 2 - o, 0);
            for (x = 0; x < 1; x += 0.001) {
                y = Math.round((1 - Math.acos((x - 0.5) * 2) / Math.PI) * 0xFF).toString(16);
                y = (y.length > 1) ? y : '0' + y;
                grdRed.addColorStop(x, '#' + y + '0000');
            }
            grdRed.addColorStop(1, '#ff0000');
            ctx.fillStyle = grdRed;
            ctx.arc(s, s, s - o, 0, Math.PI * 2);
            ctx.fill();

            ctx.globalCompositeOperation = 'lighten';//'screen';//'lighter';

            ctx.beginPath();
            var grdGreen = ctx.createLinearGradient(0, o, 0, s * 2 - o);
            for (x = 0; x < 1; x += 0.001) {
                y = Math.round((1 - Math.acos((x - 0.5) * 2) / Math.PI) * 0xFF).toString(16);
                y = (y.length > 1) ? y : '0' + y;
                grdGreen.addColorStop(x, '#00' + y + '00');
            }
            grdGreen.addColorStop(1, '#00ff00');
            ctx.fillStyle = grdGreen;
            ctx.arc(s, s, s - o, 0, Math.PI * 2);
            ctx.fill();

            //ctx.globalCompositeOperation = 'source-over';
            return c;
        }());

        function Planet(id, x, y, radius, color, count, handlers) {
            PIXI.Container.call(this);
            Object.defineProperties(this, {
                id: { value: id, writable: false, configurable: false },
                radius: { value: radius, writable: false, configurable: false }
            });
            this.x = x;
            this.y = y;

            var sp;

            var content = new PIXI.Container();

            this._gr = new PIXI.Graphics();
            //content.addChild(this._gr = new PIXI.Graphics());

            sp = new PIXI.Sprite(planetTexture);
            sp.anchor.set(0.5);
            sp.width = sp.height = 256;
            content.addChild(sp);
            this.mask = sp;

            sp = new PIXI.Sprite(linesTexture);
            sp.anchor.set(0.5);
            //sp.width = sp.height = radius * 2;
            content.addChild(sp);

            //sp = new PIXI.Sprite(displaceTexture);
            sp = new PIXI.Sprite.fromImage('displace_article.png');
            sp.anchor.set(0.5);
            sp.width = sp.height = 256;
            content.addChild(sp);
            var displaceFilter = new PIXI.filters.DisplacementFilter(sp, 150);
            content.filters = [displaceFilter];

            //setInterval(function() {sp.x += 0.1}, 50);

            //if (Math.random() < 0.4) {
                var lavaSp = new PIXI.Sprite(lavaTexture);
                lavaSp.anchor.set(0.5);
                //lavaSp.width = lavaSp.height = radius * 2;
                //content.addChild(lavaSp);
            //}

            this.addChild(content);

            /*sp = new PIXI.Sprite(glowTexture);
            sp.anchor.set(0.5);
            sp.width = sp.height = radius * 2;
            this.addChild(sp);*/

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
