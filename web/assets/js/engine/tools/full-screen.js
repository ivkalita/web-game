define(
    ['pixi'],
    function(PIXI) {
        function FullScreen(element) {
            PIXI.Container.call(this);
            if (!element.requestFullscreen && !element.webkitRequestFullscreen && !element.mozRequestFullScreen)
                return;
            this._element = element;
            this.buttonMode = this.interactive = true;
            this._gr = new PIXI.Graphics();
            this.addChild(this._gr);
            this._text = new PIXI.Text('Full screen', {font: 'bold 15px Arial'});
            this.addChild(this._text);
            this
                .on('click', this._onClick)
                .on('tap', this._onClick)
                .on('mouseover', this._onMouseOver)
                .on('touchover', this._onMouseOver)
                .on('touchstart', this._onMouseOver)
                .on('mouseout', this._onMouseOut)
                .on('touchout', this._onMouseOut)
                .on('touchend', this._onMouseOut);
            this._draw();
        }

        FullScreen.prototype = Object.create(PIXI.Container.prototype);
        FullScreen.prototype.constructor = FullScreen;

        FullScreen.prototype._draw = function(opacity) {
            opacity = opacity || 0.5;
            this._gr.clear();
            this._gr.beginFill(0xFFFFFF, opacity);
            this._gr.drawRoundedRect(-5, -5, this._text.width + 10, this._text.height + 10, 10);
            this._gr.endFill();
        };

        FullScreen.prototype._onClick = function() {
            var el = this._element;
            if (!document.fullscreenElement && !document.mozFullScreenElement && !document.webkitFullscreenElement)
                (el.requestFullscreen || el.webkitRequestFullscreen || el.mozRequestFullScreen || function(){}).call(el);
            else
                (document.cancelFullScreen || document.webkitCancelFullScreen || document.mozCancelFullScreen ||
                function(){}).call(document);
        };

        FullScreen.prototype._onMouseOver = function() {
            this._draw(1);
        };

        FullScreen.prototype._onMouseOut = function() {
            this._draw();
        };

        return FullScreen;
    }
);
