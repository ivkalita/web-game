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
        //gl.enable(gl.DEPTH_TEST);

        return gl;
    };

    this.prepareShaderProgram = function(shaderIds) {
        var shaderProgram = _gl.createProgram();

        for (var i in shaderIds) {
            if (shaderIds.hasOwnProperty(i)) {
                var s = new Shader(_gl, shaderIds[i]);
                _gl.attachShader(shaderProgram, s.shader);
            }
        }


        _gl.linkProgram(shaderProgram);
        if (!_gl.getProgramParameter(shaderProgram, _gl.LINK_STATUS)) {
            alert("Could not initialize shaders");
        }

        return shaderProgram;
    };

    this.setPerspective = function(gl, fow) {
        p = [];
        mat4.perspective(p, 90, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);

        return p;
    };

    var _scene = scene;
    var _gl = this.prepareGL(_scene);

    var _shaderIds = ["vertex-shader", "fragment-shader"];
    var _shaderProgram = this.prepareShaderProgram(_shaderIds);
    _gl.useProgram(_shaderProgram);
    var _vertexPositionBuffers = {};
    var _vertexIndicesBuffers = {};
    var _perspective = this.setPerspective(_gl, 90);
    var _uPMatrixLoc = _gl.getUniformLocation(_shaderProgram, "uPMatrix");
    var _uMVMatrixLoc = _gl.getUniformLocation(_shaderProgram, "uMVMatrix");

    this.setAttribs = function(object) {
        for (var a in object.attribs) {
            if (object.attribs.hasOwnProperty(a)) {
                var attribLocation = _gl.getAttribLocation(_shaderProgram, a);
                _gl.enableVertexAttribArray(attribLocation);
                _gl.bindBuffer(_gl.ARRAY_BUFFER, _vertexPositionBuffers);
                _gl.vertexAttribPointer(attribLocation, object.attribs[a], _gl.FLOAT, false, 0, 0);
            }
        }
    };

    this.setUniforms = function(object) {
        if (object.uniforms["uPMatrix"].length == 0) {
            object.uniforms["uPMatrix"] = _perspective;
        }

        _gl.uniformMatrix4fv(_uPMatrixLoc, false, object.uniforms["uPMatrix"]);
        _gl.uniformMatrix4fv(_uMVMatrixLoc, false, object.uniforms["uMVMatrix"]);

        //for (var u in object.uniforms) {
        //    if (object.uniforms.hasOwnProperty(u)) {
        //        _gl.uniformMatrix4fv(, false, object.uniforms[u]);
        //    }
        //}
    };

    this.initBuffers = function(object) {
        if (!(object.id in _vertexPositionBuffers)) {
            _vertexPositionBuffers[object.id] = _gl.createBuffer();
            _gl.bindBuffer(_gl.ARRAY_BUFFER, _vertexPositionBuffers[object.id]);
            _gl.bufferData(_gl.ARRAY_BUFFER, new Float32Array(object.vertices), _gl.STATIC_DRAW);


            var indices = [0, 1, 2];
            _vertexIndicesBuffers[object.id] = _gl.createBuffer();
            _gl.bindBuffer(_gl.ELEMENT_ARRAY_BUFFER, _vertexIndicesBuffers[object.id]);
            _gl.bufferData(_gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), _gl.STATIC_DRAW);
        }
    };

    var then = Date.now() / 1000;

    this.render = function() {
        var now = Date.now() / 1000;
        _scene.elapsedTime = now - then;
        then = now;

        var fps = 1 / scene.elapsedTime;
        $("#fps-meter")[0].innerText = fps.toFixed(2);

        requestAnimationFrame(this.render.bind(this));
        this.draw();
    };

    this.draw = function() {
        //_gl.clear(_gl.COLOR_BUFFER_BIT | _gl.DEPTH_BUFFER_BIT);
        //_gl.viewport(0, 0, _gl.viewportHeight, _gl.viewportWidth);

        _scene.updateObjects();
        if (_scene.needUpdate) {
            _gl.clear(_gl.COLOR_BUFFER_BIT | _gl.DEPTH_BUFFER_BIT);

            if (_scene.needCreateOrDelete) {
                _scene.needCreateOrDelete = false;

                var _vertexPositionBuffersData = [];
                var indicesData = [];
                var curIdx = 0;
                for (var j in _scene.gameObjects) {
                    if (_scene.gameObjects.hasOwnProperty(j)) {
                        Array.prototype.push.apply(_vertexPositionBuffersData, _scene.gameObjects[j].vertices);
                        for (var z = curIdx; z < curIdx + 2; z++) { indicesData.push(z) }
                        curIdx += 2;
                        this.setUniforms(_scene.gameObjects[j]);
                    }
                }

                _vertexPositionBuffers = _gl.createBuffer();
                _gl.bindBuffer(_gl.ARRAY_BUFFER, _vertexPositionBuffers);
                _gl.bufferData(_gl.ARRAY_BUFFER, new Float32Array(_vertexPositionBuffersData), _gl.STATIC_DRAW);


                 _vertexIndicesBuffers = _gl.createBuffer();
                 _gl.bindBuffer(_gl.ELEMENT_ARRAY_BUFFER, _vertexIndicesBuffers);
                 _gl.bufferData(_gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indicesData), _gl.STATIC_DRAW);

                for (var j in _scene.gameObjects) {
                    if (_scene.gameObjects.hasOwnProperty(j)) {
                        this.setUniforms(_scene.gameObjects[j]);
                    }
                }
                _gl.bindAttribLocation(_shaderProgram, 0, "aVertexPosition");
                _gl.vertexAttribPointer(0, 2, _gl.FLOAT, false, 0, 0);
                _gl.enableVertexAttribArray(0);
            }


            for (var j in _scene.gameObjects) {
                if (_scene.gameObjects.hasOwnProperty(j)) {
                    //_gl.uniformMatrix2fv(_uPMatrixLoc, false, object.uniforms["uPMatrix"]);
                    this.setUniforms(_scene.gameObjects[j]);
                }
            }

            //_gl.drawArrays(_gl.TRIANGLES, 0, 30000);

            _gl.drawElements(_gl.TRIANGLES, 1000, _gl.UNSIGNED_SHORT, 0);

            _scene.needUpdate = false;
        }



        //_gl.drawArrays(_gl.TRIANGLE_FAN, 0, 3);

        //for (var i in _scene.gameObjects) {
        //    if (_scene.gameObjects.hasOwnProperty(i)) {
        //        o = _scene.gameObjects[i];
//
//
//
        //        //this.initBuffers(o);
        //        this.setAttribs(o);
        //        this.setUniforms(o);
//
//
        //        //_gl.drawElements(_gl.TRIANGLES, o.vertCount, _gl.UNSIGNED_SHORT, 0);
        //        _gl.drawElements(_gl.TRIANGLES, 3, _gl.UNSIGNED_SHORT, 0);
        //    }
        //}

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
