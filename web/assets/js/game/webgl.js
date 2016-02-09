define(['jquery', './utils/gl-matrix-min', './utils/webgl-utils'], function($, GLM) {

	var compileShader = function(gl, type, code) {
		var shader = gl.createShader(type);
		gl.shaderSource(shader, code);
		gl.compileShader(shader);

		if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
			var log = gl.getShaderInfoLog(shader);
			gl.deleteShader(shader);
			throw new Error(log);
		}

		return shader;
	};

	var linkProgram = function(gl, vs, fs) {
		var program = gl.createProgram();
		gl.attachShader(program, vs.glShader);
		gl.attachShader(program, fs.glShader);
		gl.linkProgram(program);

		if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
			var log = gl.getProgramInfoLog(program);
			gl.deleteProgram(program);
			throw new Error(log);
		}
		
		return program;
	};

	return {
		Vector2: GLM.vec2,
		Vector3: GLM.vec3,
		Vector4: GLM.vec4,
		Matrix2x2: GLM.mat2,
		Matrix2x3: GLM.mat2d,
		Matrix3x3: GLM.mat3,
		Matrix4x4: GLM.mat4,

		createTexture2D: function(gl, image) {
			var glTexture = gl.createTexture();

			gl.bindTexture(gl.TEXTURE_2D, glTexture);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
			gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

			return {
				glTexture: glTexture,
				width: image.width,
				height: image.height,
				image: image,

				bind: function(slot) {
					gl.activeTexture(gl.TEXTURE0 + slot);
					gl.bindTexture(gl.TEXTURE_2D, this.glTexture);
				},

				dispose: function() {
					gl.deleteTexture(this.glTexture);
				}
			}
		},

		createVertexBuffer: function(gl, vertexDecl) {
			var dirty = false;
			var capacity = 0;

			var type = vertexDecl.type;
			var count = vertexDecl.count;
			var normalized = vertexDecl.normalized === true;

			return {
				glBuffer: gl.createBuffer(),
				vertexDecl: vertexDecl,

				bind: function(attribute) {
					gl.bindBuffer(gl.ARRAY_BUFFER, this.glBuffer);
					if (dirty) {
						dirty = false;
						if (capacity < this.data.byteLength) {
							capacity = this.data.byteLength;
							gl.bufferData(gl.ARRAY_BUFFER, this.data, gl.DYNAMIC_DRAW);
						} else {
							gl.bufferSubData(gl.ARRAY_BUFFER, 0, this.data);
						}
					}
					gl.enableVertexAttribArray(attribute);
					gl.vertexAttribPointer(attribute, count, type, normalized, 0, 0);
				},

				invalidate: function() {
					dirty = true;
				},

				setData: function(data) {
					this.data = data;
					this.invalidate();
				},

				dispose: function() {
					gl.deleteBuffer(this.glBuffer);
				},

				length: function() {
					return this.data.length / count;
				}
			}
		},

		createShader: function(gl, type, code) {
			return {
				glShader: compileShader(gl, type, code),

				dispose: function() {
					gl.deleteShader(this.glShader)
				}
			}
		},

		createProgram: function(gl, vs, fs) {
			var uniformLocations = {};
			var attributeLocations = {};
			var samplerCount = 0;

			return {
				glProgram: linkProgram(gl, vs, fs),

				use: function() {
					gl.useProgram(this.glProgram);
				},

				getUniformLocation: function(name) {
					var location = uniformLocations[name];
					if (location === undefined) {
						location = gl.getUniformLocation(this.glProgram, name);
						uniformLocations[name] = location;
					}
					return location;
				},

				getAttributeLocation: function(name) {
					var location = attributeLocations[name];
					if (location === undefined) {
						location = gl.getAttribLocation(this.glProgram, name);
						attributeLocations[name] = location;
					}
					return location;
				},

				setInt: function(location, value){
					gl.uniform1i(location, value);
				},

				setFloat: function(location, value) {
					gl.uniform1f(location, value);
				},

				setVector2: function(location, value) {
					gl.uniform2fv(location, value);
				},

				setVector3: function(location, value) {
					gl.uniform3fv(location, value);
				},

				setVector4: function(location, value) {
					gl.uniform4fv(location, value);
				},

				setMatrix4x4: function(location, value) {
					gl.uniformMatrix4fv(location, false, value);
				},

				setTexture: function(location, value) {
					if (location.textureSlot === undefined) {
						location.textureSlot = samplerCount++;
						gl.uniform1i(location, location.textureSlot);
					}
					value.bind(location.textureSlot);
				},

				dispose: function() {
					gl.deleteProgram(this.glProgram);
				}
			}
		},

		getContext: function(canvas) {
			var gl = WebGLUtils.create3DContext(canvas);
			if (gl) {
				return gl;
			}
			throw new Error('Webgl is not supported');
		},

		createQuad: function(x1, y1, x2, y2) {
			return [
				x1, y1,
				x2, y1,
				x2, y2,

				x2, y2,
				x1, y2,
				x1, y1,
			];
		}
	};

});