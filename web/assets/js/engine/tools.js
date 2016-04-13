define(
    ['pixi', './tools/full-screen'],
    function(PIXI, FullScreen) {
        function Tools(engine) {
            PIXI.Container.call(this);
            this._engine = engine;
            this._fullScreen = new FullScreen(engine.view);
            this._resize();
            this.addChild(this._fullScreen);
        }

        Tools.prototype = Object.create(PIXI.Container.prototype);
        Tools.prototype.constructor = Tools;

        Tools.prototype._resize = function()
        {
            this._fullScreen.x =  this._engine._renderer.width - this._fullScreen.width - 20;
            this._fullScreen.y =  this._engine._renderer.height - this._fullScreen.height - 20;
            this._fullScreen._draw(1);
        }
        return Tools;
    }
);
