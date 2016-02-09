define([
	'jquery',
	'./webgl',
	'./asset-manager',
	'./assets',
	'./sky',
	'./camera',
	'./planet',
	'./fps-meter'
], function($, Webgl, AssetManager, Assets, Sky, Camera, Planet, FpsMeter) {

	// Этот модуть желательно разделить на Game(база) и Map
	var Scene = function(canvas, settings) {
		canvas.width = canvas.clientWidth;
		canvas.height = canvas.clientHeight;

		this.canvas = canvas;
		this.width = settings.width;
		this.height = settings.height;
		this.borderSize = settings.borderSize || 0;
		this.gl = Webgl.getContext(this.canvas);
		this.assetManager = AssetManager.create(this.gl);
		this._fpsMeter = new FpsMeter();

		this.components = [];

		this.camera = new Camera(this);
		this.camera.setViewport(canvas.width, canvas.height);
		this.camera.x = (this.width - this.camera.viewportWidth) / 2;
		this.camera.y = (this.height - this.camera.viewportHeight) / 2;
		this.components.push(this.camera);
	};

	//Это должно буть где-то в другом месте, но точно не здесь!!!
	Scene.prototype.init = function() {
		var sky = this.sky = new Sky(this);
		var planet = new Planet(this);
		planet.position = Webgl.Vector2.fromValues(this.width / 2, this.height / 2);

		this.components.push(sky);
		this.components.push(planet);

		(function(self) {
			var startPageX = 0;
			var startPageY = 0;
			var movement = false;

			$(self.canvas).mousedown(function(e) {
				startPageX = e.pageX;
				startPageY = e.pageY;
				self.camera.beginMotion();
			});

			$(self.canvas).mousemove(function(e) {
				self.camera.move(startPageX - e.pageX, startPageY - e.pageY);
			});

			$(self.canvas).mouseup(function(e) {
				self.camera.endMotion();
			});

		})(this);
	};

	Scene.prototype.update = function(delta) {
		for (var i = 0; i < this.components.length; i++) {
			this.components[i].update(delta);
		}
	};

	Scene.prototype.render = function() {
		var gl = this.gl;
		gl.clearColor(1, 1, 1, 1);
		gl.clear(gl.COLOR_BUFFER_BIT);
		for (var i = 0; i < this.components.length; i++) {
			this.components[i].render();
		}
	};

	Scene.prototype.frame = function(delta) {
		this.update(delta);
		this.render();
	};

	Scene.prototype.loop = function() {
		var time = Date.now();
		var delta = (time - this._time) / 1000;
		this.frame(delta);
		this._time = time;
		this._fpsMeter.update();
		requestAnimFrame(this.loop.bind(this));
	};

	Scene.prototype.run = function() {
		this.loadAssets(function(assets) {
			this._time = Date.now();
			this._fpsMeter.start();
			this.assets = assets;
			this.init();
			this.loop();
		});
	};

	Scene.prototype.loadAssets = function(done) {
		Assets.load(this.assetManager).done(done.bind(this)).fail(function(err) {
			throw new Error(err.url + ': ' + err.message);
		});
	};

	Object.defineProperties(Scene.prototype, {
		fps: {
			get: function() {
				return this._fpsMeter.fps;
			}
		}
	});

	return Scene;
});