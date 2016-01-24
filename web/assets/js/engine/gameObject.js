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
    this._dest = vec2.fromValues(x, y);
    this._curr = vec2.clone(this._dest);
    this._color = color;
    this._angle = angle;
    this._currAngle = angle;
    this._transMat = [];
    
    this.updateVertices();
    
    Object.defineProperties(this, {
        "x": {
            get: function() { return this._dest[0]; },
            set: function(x) {
                vec2.set(this._curr, this._dest[0], this._curr[1]);
                vec2.set(this._dest, x, this._dest[1]);
                this.updateAngle();
                this.updateVertices();
            }
        },
        "y": {
            get: function() { return this._dest[1]; },
            set: function(y) {
                vec2.set(this._curr, this._curr[0], this._dest[1]);
                vec2.set(this._dest, this._dest[0], y);
                this.updateAngle();
                this.updateVertices();
            }
        },
    });
}

Ship.prototype._speed = 0.01;
Ship.prototype._speedAngle = 0.1;
Ship.prototype._vertices = [
    0, -0.5,
    -1, -1,
    0, 1.5,
    1, -1];
Ship.prototype._vertexShader = '\
    attribute vec2 aVertexPosition;\
    uniform mat4 uMVMatrix;\
    uniform mat4 uPMatrix;\
    void main(void) {\
        gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 0.0, 1.0);\
    }';
Ship.prototype._fragmentShader = '\
    precision mediump float;\
    uniform vec3 uVertexColor;\
    void main(void) {\
        gl_FragColor = vec4(uVertexColor, 1.0);\
    }';
Ship.prototype._program = null;

Ship.prototype.updateAngle = function () {
    var s = vec2.subtract([], this._dest, this._curr);
    this._angle = Math.atan2(s[1], s[0]) - Math.PI / 2;
};

Ship.prototype.updateVertices = function () {
    var _scale = 0.03;
    mat4.identity(this._transMat);
    mat4.translate(this._transMat, this._transMat, [this._curr[0], this._curr[1], -1.0]);
    mat4.rotate(this._transMat, this._transMat, this._currAngle, [0, 0, 1]);
    mat4.scale(this._transMat, this._transMat, [_scale, _scale, _scale])
};

vec2.equals = function (a, b) {
    var a0 = a[0], a1 = a[1];
    var b0 = b[0], b1 = b[1];
    return (Math.abs(a0 - b0) <= glMatrix.EPSILON*Math.max(1.0, Math.abs(a0), Math.abs(b0)) &&
            Math.abs(a1 - b1) <= glMatrix.EPSILON*Math.max(1.0, Math.abs(a1), Math.abs(b1)));
};

Ship.prototype.draw = function(gl, perspectMat){
    if (!this._program) {
        var shaders = [];
        shaders.push(compileShader(gl, this._vertexShader, gl.VERTEX_SHADER));
        shaders.push(compileShader(gl, this._fragmentShader, gl.FRAGMENT_SHADER));
        Ship.prototype._program = linkShaderProgram(gl, shaders);
        console.log('Planet shader program created');
    }
    gl.useProgram(this._program);
    
    if (vec2.squaredDistance(this._curr, this._dest) > 0.0001) {
        var t = vec2.subtract([], this._dest, this._curr);
        t = vec2.normalize([0, 0], t);
        vec2.scaleAndAdd (this._curr, this._curr, t, this._speed)
        //vec2.scale(this._curr, vec2.normalize([], vec2.subtract([], this._dest, this._curr)), this._speed)
        if (Math.abs(this._currAngle - this._angle) > 0.2) {
            this._currAngle += this._speedAngle * Math.sign(this._angle - this._currAngle);
        }
    }
    this.updateVertices();
    
    var _vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this._vertices), gl.STATIC_DRAW);
    var attribLocation = gl.getAttribLocation(this._program, 'aVertexPosition');
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.enableVertexAttribArray(attribLocation);
    gl.vertexAttribPointer(attribLocation, 2, gl.FLOAT, false, 0, 0);
    
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uPMatrix'), false, perspectMat);
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uMVMatrix'), false, this._transMat);
    gl.uniform3fv(gl.getUniformLocation(this._program, 'uVertexColor'), this._color);
    
    gl.drawArrays(gl.TRIANGLE_FAN, 0, this._vertices.length / 2);
};


function Planet(x, y, color, type = 0, angle = 0) {
    var _scales = [0.4, 0.3, 0.2];
    this._scale = _scales[type];
    this._angle = angle;
    this._x = x;
    this._y = y;
    this._color = color;
    this._transMat = [];
    
    this.updateVertices();
}

Planet.prototype._vertices = []
Planet.prototype._vertices = Planet.prototype._vertices.concat([0, 0]);
Planet.prototype._vertices = Planet.prototype._vertices.concat([1, 0]);
var polyNum = 50;
var degPerPoly = (2 * Math.PI) / polyNum;
for (var i = 1; i <= polyNum; i++) {
    var angle = degPerPoly * i;
    Planet.prototype._vertices = Planet.prototype._vertices.concat([Math.cos(angle), Math.sin(angle)]);
}

Planet.prototype._vertexShader = '\
    attribute vec2 aVertexPosition;\
    uniform mat4 uMVMatrix;\
    uniform mat4 uPMatrix;\
    varying float vDistance;\
    void main(void) {\
        vDistance = (aVertexPosition == vec2(0, 0)) ? 0.0 : 1.0;\
        gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 0.0, 1.0);\
    }';
Planet.prototype._fragmentShader = '\
    precision mediump float;\
    uniform vec3 uVertexColor;\
    varying float vDistance;\
    float brightness;\
    float brightness2;\
    void main(void) {\
        if (vDistance < 0.87) {\
            brightness = 0.90 - pow(vDistance, 4.0);\
            gl_FragColor = vec4(uVertexColor * brightness, 1.0);\
        }\
        if (vDistance >= 0.87 && vDistance <= 0.90) {\
            brightness2 = 0.90 - pow(vDistance, 4.0);\
            gl_FragColor = vec4(uVertexColor * brightness2, 1.0);\
            brightness = pow((vDistance - 0.87) / 0.03, 2.0);\
            gl_FragColor += vec4(1.0, 1.0, 0.7, 0.0) * brightness;\
        }\
        if (vDistance > 0.90) {\
            brightness = 1.0 - pow((vDistance - 0.90) / 0.10, 0.2);\
            gl_FragColor = vec4(1.0, 1.0, 0.7, 1.0 * brightness);\
        }\
    }';
Planet.prototype._program = null;

Planet.prototype.draw = function(gl, perspectMat){
    if (!this._program) {
        var shaders = [];
        shaders.push(compileShader(gl, this._vertexShader, gl.VERTEX_SHADER));
        shaders.push(compileShader(gl, this._fragmentShader, gl.FRAGMENT_SHADER));
        Planet.prototype._program = linkShaderProgram(gl, shaders);
        console.log('Planet shader program created');
    }
    gl.useProgram(this._program);
    
    var _vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this._vertices), gl.STATIC_DRAW);
    var attribLocation = gl.getAttribLocation(this._program, 'aVertexPosition');
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.enableVertexAttribArray(attribLocation);
    gl.vertexAttribPointer(attribLocation, 2, gl.FLOAT, false, 0, 0);
    
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uPMatrix'), false, perspectMat);
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uMVMatrix'), false, this._transMat);
    gl.uniform3fv(gl.getUniformLocation(this._program, 'uVertexColor'), this._color);
    
    gl.drawArrays(gl.TRIANGLE_FAN, 0, this._vertices.length / 2);
};

Planet.prototype.updateVertices = function () {
    mat4.identity(this._transMat);
    mat4.translate(this._transMat, this._transMat, [this._x, this._y, -1.0]);
    mat4.rotate(this._transMat, this._transMat, this._angle, [0, 0, 1]);
    mat4.scale(this._transMat, this._transMat, [this._scale, this._scale, this._scale])
};


function Star(x, y) {
    this._x = x;
    this._y = y;
    //this._frames_show = Math.floor(Math.random() * 2000);
    this._frameShow = - Math.floor(Math.random() * 500) - 50;
    this._frameCurr = this._frameShow;
    this._frameHide = Math.floor(Math.random() * 2000) + 150;
    this._transMat = [];
    this.updateVertices();
    Star.prototype._count++;
}

Star.prototype._count = 0;
Star.prototype._vertices = [
    0, 0, 0, 1, 1, 0,
    0, 0, 0, -1, 1, 0,
    0, 0, 0, 1, -1, 0,
    0, 0, 0, -1, -1, 0];
Star.prototype._vertexShader = '\
    attribute vec2 aVertexPosition;\
    uniform mat4 uMVMatrix;\
    uniform mat4 uPMatrix;\
    varying float vDistance;\
    void main(void) {\
        vDistance = (aVertexPosition == vec2(0, 0)) ? 0.0 : 1.0;\
        gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 0.0, 1.0);\
    }';
Star.prototype._fragmentShader = '\
    precision mediump float;\
    varying float vDistance;\
    uniform float uBrightness;\
    float brightness;\
    void main(void) {\
        brightness = uBrightness * pow(1.0 - vDistance, 6.0);\
        gl_FragColor = vec4(1.0, 1.0, 1.0, brightness);\
    }';
Star.prototype._program = null;

Star.prototype.draw = function(gl, perspectMat){
    if (!this._program) {
        var shaders = [];
        shaders.push(compileShader(gl, this._vertexShader, gl.VERTEX_SHADER));
        shaders.push(compileShader(gl, this._fragmentShader, gl.FRAGMENT_SHADER));
        Star.prototype._program = linkShaderProgram(gl, shaders);
        console.log('Star shader program created');
    }
    gl.useProgram(this._program);
    
    this._frameCurr++;
    if (this._frameHide == this._frameCurr) {
        Star.prototype._count--;
        return null;
    }
    var brightness;
    brightness = (this._frameCurr < 0) ?
        (this._frameShow - this._frameCurr) / this._frameShow :
        (this._frameHide - this._frameCurr) / this._frameHide;
    
    var _vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this._vertices), gl.STATIC_DRAW);
    var attribLocation = gl.getAttribLocation(this._program, 'aVertexPosition');
    gl.bindBuffer(gl.ARRAY_BUFFER, _vertexPositionBuffer);
    gl.enableVertexAttribArray(attribLocation);
    gl.vertexAttribPointer(attribLocation, 2, gl.FLOAT, false, 0, 0);
    
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uPMatrix'), false, perspectMat);
    gl.uniformMatrix4fv(gl.getUniformLocation(this._program, 'uMVMatrix'), false, this._transMat);
    gl.uniform1f(gl.getUniformLocation(this._program, 'uBrightness'), brightness);
    
    gl.drawArrays(gl.TRIANGLES, 0, this._vertices.length / 2);
};

Star.prototype.updateVertices = function() {
    var _scale = 0.04;
    mat4.identity(this._transMat);
    mat4.translate(this._transMat, this._transMat, [this._x, this._y, -1.0]);
    mat4.scale(this._transMat, this._transMat, [_scale, _scale, _scale]);
}
