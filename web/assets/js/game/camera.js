define(['./scene', './component', './velocity-meter', './webgl'], function(Scene, Component, VelocityMeter, Webgl) {

	var Camera = function(scene) {
		Component.call(this, scene);

		this._y = 0;
		this._y = 0;
		this.viewportWidth = 0;
		this.viewportHeight = 0;

		this._viewDirty = this._projectionDirty = true;
		this._view = Webgl.Matrix4x4.create();
		this._projection = Webgl.Matrix4x4.create();
		this._viewProjection = Webgl.Matrix4x4.create();

		this._motion = false;
		this._velocityMeter = new VelocityMeter();
		this._velocity = {
			x: 0,
			y: 0
		};
	};

	Camera.prototype = Object.create(Component.prototype);
	Camera.prototype.constructor = Camera;

	Camera.prototype.update = function(delta) {
		var DAMPING = 3;

		if (this._motion) {
			return;
		}

		var scene = this.scene;
		var velocity = this._velocity;
		var velocityLength = Math.sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
		if (velocityLength > 1) {
			velocity.x -= velocity.x * DAMPING * delta;
			velocity.y -= velocity.y * DAMPING * delta;
			this.x = this.x + velocity.x * delta;
			this.y = this.y + velocity.y * delta;
			this.clampPosition();
		}
	};

	Camera.prototype.beginMotion = function() {
		this._startX = this.x;
		this._startY = this.y;
		this._motion = true;
	};

	Camera.prototype.endMotion = function() {
		if (this._motion) {
			this._motion = false;
			this._velocity = this._velocityMeter.calcVelocity();
		}
	};

	Camera.prototype.move = function(deltaX, deltaY) {
		if (this._motion) {
			this.x = this._startX + deltaX;
			this.y = this._startY + deltaY;
			this.clampPosition();
			this._velocityMeter.addSample(deltaX, deltaY);
		}
	};

	Camera.prototype.clampPosition = function() {
		var scene = this.scene;
		var borderSize = scene.borderSize;

		this.x = Math.max(-borderSize, Math.min(scene.width - this.viewportWidth + borderSize, this.x));
		this.y = Math.max(-borderSize, Math.min(scene.height - this.viewportHeight + borderSize, this.y));
	};

	Camera.prototype.setViewport = function(width, height) {
		this.gl.viewport(0, 0, width, height);
		this.viewportWidth = width;
		this.viewportHeight = height;
		this._projectionDirty = true;
	};

	Camera.prototype.setPosition = function(x, y) {
		this._x = x;
		this._y = y;
		this._viewDirty = true;
	};

	Object.defineProperties(Camera.prototype, {
		x: {
			get: function() { return this._x; },
			set: function(value) { this.setPosition(value, this.y); }
		},

		y: {
			get: function() { return this._y; },
			set: function(value) { this.setPosition(this.x, value); }
		},

		view: {
			get: function() {
				if (this._viewDirty) {
					this._viewDirty = false;
					Webgl.Matrix4x4.fromTranslation(this._view, [-this.x, -this.y, 0]);
				}
				return this._view;
			}
		},

		projection: {
			get: function() {
				if (this._projectionDirty) {
					this._projectionDirty = false;
					Webgl.Matrix4x4.ortho(this._projection, 0, this.viewportWidth, this.viewportHeight, 0, -50, 50);
				}
				return this._projection;
			}
		},

		viewProjection: {
			get: function() {
				if (this._viewDirty | this._projectionDirty) {
					Webgl.Matrix4x4.mul(this._viewProjection, this.projection, this.view);
				}
				return this._viewProjection;
			}
		},
	});

	return Camera;
});