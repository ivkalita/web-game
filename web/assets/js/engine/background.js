define(
    ['pixi', './objects/clouds', './objects/star'],
    function(PIXI, Clouds, Star) {
        function Background(width, height) {
            PIXI.Container.call(this);
            this.width = width;
            this.height = height;
            var i, areaOfScreen = width * height;

            this._update = [];
            this._dinamic = new PIXI.ParticleContainer(null, {alpha: true});
            for (i = 0; i < areaOfScreen / 200000; i++) {
                this._update.push(new Star(width, height, true, true));
                this._dinamic.addChild(this._update[this._update.length - 1]);
            }
            this.addChild(this._dinamic);

            this._static = new PIXI.Container();
            var starCount = areaOfScreen / 5000;
            for (i = 0; i < starCount; i++) {
                if (i == Math.floor(starCount / 2)) this._static.addChild(new Clouds(width, height));
                this._static.addChild(new Star(width, height));
            }

            this._static.cacheAsBitmap = true;
            this.addChild(this._static);
        }

        Background.prototype = Object.create(PIXI.Container.prototype);
        Background.prototype.constructor = Background;

        Background.prototype.update = function() {
            this._update.forEach(function(el){ el.update(); });
        };

        return Background;
    }
);
