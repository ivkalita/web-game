define(
    ['pixi'],
    function(PIXI) {
        function Tools(engine) {
            PIXI.Container.call(this);
        }

        Tools.prototype = Object.create(PIXI.Container.prototype);
        Tools.prototype.constructor = Tools;

        return Tools;
    }
);
