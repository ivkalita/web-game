define(['./scene', './component', './webgl'], function(Scene, Component, Webgl) {

	var Sky = function(scene) {
		Component.call(this, scene);

		var gl = this.gl;
		var layerProgram = this._layerProgram = scene.assets.programs.skyLayer;
		var layerTextures = scene.assets.textures.sky;
		var layers = this._layers = [
			{
				speed: 0,
				texture: layerTextures.layer0
			},
			{
				speed: 8,
				texture: layerTextures.layer1
			},
			{
				speed: 15,
				texture: layerTextures.layer2
			},
			{
				speed: 30,
				texture: layerTextures.layer3
			}
		];

		for (var i = 0; i < layers.length; i++) {
			var texture = layers[i].texture;
			layers[i].textureSize = Webgl.Vector2.fromValues(texture.width, texture.height);
			layers[i].offset = 0;
		}

		this._attributeLocations = {
			position: layerProgram.getAttributeLocation('in_Position')
		};

		this._uniformLocations = {
			viewProjection: layerProgram.getUniformLocation('viewProjection'),
			textureSize: layerProgram.getUniformLocation('textureSize'),
			texture: layerProgram.getUniformLocation('texture'),
			offset: layerProgram.getUniformLocation('offset'),
			mapSize: layerProgram.getUniformLocation('mapSize'),
			borderSize: layerProgram.getUniformLocation('borderSize')
		};

		this._positionBuffer = Webgl.createVertexBuffer(gl, {
			type: gl.FLOAT,
			count: 2
		});

		this._positionBuffer.setData(new Float32Array(Webgl.createQuad(-scene.borderSize, -scene.borderSize, scene.width + scene.borderSize, scene.height + scene.borderSize)));
	};

	Sky.prototype = Object.create(Component.prototype);
	Sky.prototype.constructor = Sky;
	
	Sky.prototype.update = function(delta) {
		for (var i = 0; i < this._layers.length; i++) {
			var layer = this._layers[i];
			layer.offset += delta * layer.speed;
			//layer.offset %= layer.texture.width;
		}
	};

	Sky.prototype.render = function() {
		var gl = this.gl;
		gl.enable(gl.BLEND);
		gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
		for (var i = 0; i < this._layers.length; i++) {
			var layer = this._layers[i];
			this._layerProgram.use();
			this._layerProgram.setFloat(this._uniformLocations.offset, layer.offset);
			this._layerProgram.setFloat(this._uniformLocations.borderSize, this.scene.borderSize);
			this._layerProgram.setMatrix4x4(this._uniformLocations.viewProjection, this.scene.camera.viewProjection);
			this._layerProgram.setVector2(this._uniformLocations.mapSize, Webgl.Vector2.fromValues(this.scene.width, this.scene.height));
			this._layerProgram.setVector2(this._uniformLocations.textureSize, layer.textureSize);
			this._layerProgram.setTexture(this._uniformLocations.texture, layer.texture);
			this._positionBuffer.bind(this._attributeLocations.position);
			gl.drawArrays(gl.TRIANGLES, 0, this._positionBuffer.length());
		}
		gl.disable(gl.BLEND);
	};

	return Sky;
})