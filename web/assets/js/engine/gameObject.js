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

function Ship(x, y, angle) {
    var _scale = 0.1;
    this._x = x;
    this._y = y;
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
        void main(void) {\
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\
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
