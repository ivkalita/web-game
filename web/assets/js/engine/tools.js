define(
    ['pixi', './tools/full-screen'],
    function(PIXI, FullScreen) {
        function Tools(engine) {
            PIXI.Container.call(this);

            var fullScreen = new FullScreen(engine.view);
            fullScreen.x = engine._renderer.width - fullScreen.width - 10;
            fullScreen.y = engine._renderer.height - fullScreen.height - 10;
            this.addChild(fullScreen);
        }

        Tools.prototype = Object.create(PIXI.Container.prototype);
        Tools.prototype.constructor = Tools;

        return Tools;
    }
);
