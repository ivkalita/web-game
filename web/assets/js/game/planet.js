define(['./scene', './component', './webgl'], function(Scene, Component, Webgl) {

	var Planet = function(scene) {
		Component.call(this, scene);

		var gl = this.gl;

		this.position = Webgl.Vector2.fromValues(0, 0);
		this.color = Webgl.Vector4.fromValues(1, 1, 0, 1);
		this.radius = 150;

		var texture = this._templateTexture = scene.assets.textures.planetTemplate;
		var program = this._program = scene.assets.programs.planet;

		this._attributeLocations = {
			position: program.getAttributeLocation('in_Position')
		};

		this._uniformLocations = {
			viewProjection: program.getUniformLocation('viewProjection'),
			color: program.getUniformLocation('color'),
			radius: program.getUniformLocation('radius'),
			offset: program.getUniformLocation('offset'),
			texture: program.getUniformLocation('texture')
		};

		this._positionBuffer = Webgl.createVertexBuffer(gl, {
			type: gl.FLOAT,
			count: 2
		});

		this._positionBuffer.setData(new Float32Array(Webgl.createQuad(0, 0, 1, 1)));
	};

	Planet.prototype = Object.create(Component.prototype);
	Planet.prototype.constructor = Planet;

	Planet.prototype.render = function() {
		var gl = this.gl;

		this._program.use();
		this._program.setMatrix4x4(this._uniformLocations.viewProjection, this.scene.camera.viewProjection);
		this._program.setVector4(this._uniformLocations.color, this.color);
		this._program.setFloat(this._uniformLocations.radius, this.radius);
		this._program.setVector2(this._uniformLocations.offset, this.position);
		this._program.setTexture(this._uniformLocations.texture, this._templateTexture);
		this._positionBuffer.bind(this._attributeLocations.position);

		gl.enable(gl.BLEND);
		gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
		gl.drawArrays(gl.TRIANGLES, 0, this._positionBuffer.length());
		gl.disable(gl.BLEND);
	};

	return Planet;
});