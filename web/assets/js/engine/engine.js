//class Engine
function Engine(scene) {
    this.prepareGL = function(scene) {
        var gl;

        var names = ["webgl", "experimental-webgl", "webkit-3d", "moz-webgl"];
        var width = scene.canvas.clientWidth;
        var height = scene.canvas.clientHeight;
        if (scene.canvas.width != width || scene.canvas.height != height) {
            scene.canvas.width = width;
            scene.canvas.height = height;
        }

        gl = null;
        for (var i = 0; i < names.length; i++) {
            try {
                gl = scene.canvas.getContext(names[i]);
            } catch(e) {}

            if (gl) {
                gl.viewportWidth = scene.canvas.width;
                gl.viewportHeight = scene.canvas.height;

                break;
            } else {
                alert("Could not initialize WebGL");
            }
        }

        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.enable(gl.DEPTH_TEST);

        return gl;
    };

    this.prepareShaderProgram = function(shaderIds) {
        var shaderProgram = _gl.createProgram();

        for (var i in shaderIds) {
            if (shaderIds.hasOwnProperty(i)) {
                s = new Shader(_gl, shaderIds[i]);
                _gl.attachShader(shaderProgram, s.shader);
            }
        }

        _gl.linkProgram(shaderProgram);
        if (!_gl.getProgramParameter(shaderProgram, _gl.LINK_STATUS)) {
            alert("Could not initialize shaders");
        }

        return shaderProgram;
    };

    var _scene = scene;
    var _gl = this.prepareGL(_scene);

    var _shaderIds = ["vertex-shader", "fragment-shader"];
    var _shaderProgram = this.prepareShaderProgram(_shaderIds);
    var _vertexPositionBuffer;

    this.setAttribs = function(object) {
        object.attribs["aVertexPosition"] = object.vertSize;

        for (a in object.attribs) {
            if (object.attribs.hasOwnProperty(a)) {
                var attribLocation = _gl.getAttribLocation(_shaderProgram, a);
                _gl.enableVertexAttribArray(attribLocation);
                _gl.bindBuffer(_gl.ARRAY_BUFFER, _vertexPositionBuffer);
                _gl.vertexAttribPointer(attribLocation, object.attribs[a], _gl.FLOAT, false, 0, 0);
            }
        }
    };

    this.setUniforms = function(object) {
        mat4.perspective(object.uniforms["uPMatrix"], 90, _gl.viewportWidth / _gl.viewportHeight, 0.1, 100.0);
        mat4.identity(object.uniforms["uMVMatrix"]);
        mat4.translate(object.uniforms["uMVMatrix"], object.uniforms["uMVMatrix"], [0.0, 0.0, -1.0]);
        if (object instanceof Ship) {
            mat4.rotate(object.uniforms["uMVMatrix"], object.uniforms["uMVMatrix"], object.direction * Math.PI / 180, [0, 0, 1]);
        }

        for (u in object.uniforms) {
            if (object.uniforms.hasOwnProperty(u)) {
                _gl.uniformMatrix4fv(_gl.getUniformLocation(_shaderProgram, u), false, object.uniforms[u]);
            }
        }
    };

    this.initBuffers = function(object) {
        _vertexPositionBuffer = _gl.createBuffer();
        _gl.bindBuffer(_gl.ARRAY_BUFFER, _vertexPositionBuffer);
        _gl.bufferData(_gl.ARRAY_BUFFER, new Float32Array(object.vertices), _gl.STATIC_DRAW);
    };

    this.render = function() {
        requestAnimationFrame(this.render.bind(this));
        this.draw();
    };

    this.draw = function() {
        _gl.clear(_gl.COLOR_BUFFER_BIT | _gl.DEPTH_BUFFER_BIT);
        //_gl.viewport(0, 0, _gl.viewportHeight, _gl.viewportWidth);

        _scene.updateObjects();
        for (i in _scene.gameObjects) {
            if (_scene.gameObjects.hasOwnProperty(i)) {
                o = _scene.gameObjects[i];

                _gl.useProgram(_shaderProgram);

                this.initBuffers(o);
                this.setAttribs(o);
                this.setUniforms(o);


                _gl.drawArrays(_gl.TRIANGLE_FAN, 0, o.vertCount);
            }
        }
    }
}

//class Shader
function Shader(gl, id) {
    var _shaderType = $("#" + id).attr("type");
    var _shaderText = $("#" + id).text();

    this.prepareShader = function() {
        var shader;
        if (_shaderType == "x-shader/x-vertex") {
            shader = gl.createShader(gl.VERTEX_SHADER);
        } else if (_shaderType == "x-shader/x-fragment") {
            shader = gl.createShader(gl.FRAGMENT_SHADER);
        } else {
            return null;
        }

        gl.shaderSource(shader, _shaderText);
        gl.compileShader(shader);

        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            return null;
        }

        return shader;
    };

    var _shader = this.prepareShader();

    Object.defineProperties(this, {
        "shaderType": { value: _shaderType, writable: true },
        "shaderText": { value: _shaderText, writable: true },
        "shader": { value: _shader, writable: true }
    });
}

Shader.prototype.constructor = Shader;