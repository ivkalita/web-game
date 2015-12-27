function compileShader(gl, code, type) { //type: gl.VERTEX_SHADER or gl.FRAGMENT_SHADER
    var shader = shader = gl.createShader(type);
    gl.shaderSource(shader, code);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.log(gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}

function linkShaderProgram(gl, shaders) {
    var program = gl.createProgram();
    for (var i = 0; i < shaders.length; i++) {
        gl.attachShader(program, shaders[i]);
    }
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        alert("Could not initialize shader program");
    }
    return program;
}

function Ship(x, y, color, angle) {
    var _scale = 0.02;
    this._x = x;
    this._y = y;
    this._color = color;
    this._angle = angle;
    this._transMat = [];
    this._verticles = [];
    this._vertexShader = '\
        attribute vec2 aVertexPosition;\
        uniform mat4 uMVMatrix;\
        uniform mat4 uPMatrix;\
        void main(void) {\
            gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 0.0, 1.0);\
        }';
    this._fragmentShader = '\
        precision mediump float;\
        uniform vec3 uVertexColor;\
        void main(void) {\
            gl_FragColor = vec4(uVertexColor, 1.0);\
        }';
    this.program = null;
    
    this.draw = function(gl, perspectMat){
        if (!this._program) {
            var shaders = [];
            shaders.push(compileShader(gl, this._vertexShader, gl.VERTEX_SHADER));
            shaders.push(compileShader(gl, this._fragmentShader, gl.FRAGMENT_SHADER));
            this._program = linkShaderProgram(gl, shaders);
        }
        gl.useProgram(this._program);
        
        var _vertexPositionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this._verticles), gl.STATIC_DRAW);
        var attribLocation = gl.getAttribLocation(this._program, 'aVertexPosition');
        gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
        gl.enableVertexAttribArray(attribLocation);
        gl.vertexAttribPointer(attribLocation, 2, gl.FLOAT, false, 0, 0);
        
        gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uPMatrix'), false, perspectMat);
        gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uMVMatrix'), false, this._transMat);
        gl.uniform3fv(gl.getUniformLocation(this._program, 'uVertexColor'), this._color);
        
        gl.drawArrays(gl.TRIANGLE_FAN, 0, this._verticles.length / 2);
    };
    
    this.updateVertices = function () {
        this._verticles = [];
        this._verticles = this._verticles.concat([0, -0.5]);
        this._verticles = this._verticles.concat([-1, -1]);
        this._verticles = this._verticles.concat([0, 1.5]);
        this._verticles = this._verticles.concat([1, -1]);
        mat4.identity(this._transMat);
        mat4.translate(this._transMat, this._transMat, [this._x, this._y, -1.0]);
        mat4.rotate(this._transMat, this._transMat, this._angle, [0, 0, 1]);
        mat4.scale(this._transMat, this._transMat, [_scale, _scale, _scale])
    };
    this.updateVertices();
}

function Planet(x, y, color, type = 0, angle = 0) {
    var _scales = [0.4, 0.2, 0.1];
    var _scale = _scales[type];
    this._angle = angle;
    this._x = x;
    this._y = y;
    this._color = color;
    this._transMat = [];
    this._verticles = [];
    this._vertexShader = '\
        attribute vec2 aVertexPosition;\
        uniform mat4 uMVMatrix;\
        uniform mat4 uPMatrix;\
        varying float vDistance;\
        void main(void) {\
            vDistance = distance(aVertexPosition, vec2(0, 0));\
            gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 0.0, 1.0);\
        }';
    this._fragmentShader = '\
        precision mediump float;\
        uniform vec3 uVertexColor;\
        varying float vDistance;\
        float brightes;\
        void main(void) {\
            if (vDistance < 0.90) {\
                brightes = 0.90 - pow(vDistance, 4.0);\
                gl_FragColor = vec4(uVertexColor * brightes, 1.0);\
            }\
            if (vDistance > 0.87) {\
                if (vDistance < 0.90) {\
                    brightes = pow((vDistance - 0.87) / 0.03, 2.0);\
                    gl_FragColor += vec4(1.0, 1.0, 0.7, 0.0) * brightes;\
                } else {\
                    brightes = 1.0 - pow((vDistance - 0.90) / 0.10, 0.2);\
                    gl_FragColor = vec4(1.0, 1.0, 0.7, 1.0 * brightes);\
                }\
            }\
        }';
    this.program = null;
    
    this.draw = function(gl, perspectMat){
        if (!this._program) {
            var shaders = [];
            shaders.push(compileShader(gl, this._vertexShader, gl.VERTEX_SHADER));
            shaders.push(compileShader(gl, this._fragmentShader, gl.FRAGMENT_SHADER));
            this._program = linkShaderProgram(gl, shaders);
        }
        gl.useProgram(this._program);
        
        var _vertexPositionBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this._verticles), gl.STATIC_DRAW);
        var attribLocation = gl.getAttribLocation(this._program, 'aVertexPosition');
        gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
        gl.enableVertexAttribArray(attribLocation);
        gl.vertexAttribPointer(attribLocation, 2, gl.FLOAT, false, 0, 0);
        
        gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uPMatrix'), false, perspectMat);
        gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uMVMatrix'), false, this._transMat);
        gl.uniform3fv(gl.getUniformLocation(this._program, 'uVertexColor'), this._color);
        
        gl.drawArrays(gl.TRIANGLE_FAN, 0, this._verticles.length / 2);
    };
    
    this.updateVertices = function () {
        this._verticles = [];
        this._verticles = this._verticles.concat([0, 0]);
        this._verticles = this._verticles.concat([1, 0]);
        var polyNum = 100;
        var degPerPoly = (2 * Math.PI) / polyNum;
        for (var i = 1; i <= polyNum; i++) {
            var angle = degPerPoly * i;
            this._verticles = this._verticles.concat([Math.cos(angle), Math.sin(angle)]);
        }
        mat4.identity(this._transMat);
        mat4.translate(this._transMat, this._transMat, [x, y, -1.0]);
        mat4.rotate(this._transMat, this._transMat, this._angle, [0, 0, 1]);
        mat4.scale(this._transMat, this._transMat, [_scale, _scale, _scale])
    };
    this.updateVertices();
}
