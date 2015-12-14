var gl;
var vertexPositionBuffer;
var shaderProgram;

var vertices = [
     0.0,  1.0, 0.0,
    -1.0, -1.0, 0.0,
     1.0, -1.0, 0.0
];

var mvMatrix = mat4.create();
var pMatrix = mat4.create();

function webGLStart() {
    var canvas = document.getElementById("canvas");

    initGL(canvas);
    initShaders();
    initBuffers();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.enable(gl.DEPTH_TEST);

    drawScene();
}

function initGL(canvas) {
    var names = ["webgl", "experimental-webgl", "webkit-3d", "moz-webgl"];

    gl = null;
    for (var i = 0; i < names.length; i++) {
        try {
            gl = canvas.getContext(names[i]);
        } catch(e) {}

        if (gl) {
            gl.viewportWidth = canvas.width;
            gl.viewportHeight = canvas.height;

            break;
        } else {
            alert("Could not initialize WebGL");
        }
    }
}

function initShaders() {
    shaderProgram = gl.createProgram();

    var vertexShader = getShader(gl, "vertex-shader");
    var fragmentShader = getShader(gl, "fragment-shader");
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);

    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        //alert("Could not initialize shaders");
    }

    gl.useProgram(shaderProgram);

    shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);

    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");

}

function getShader(gl, id) {
    var shaderText = $("#" + id).html();
    var shaderType = $("#" + id).attr("type");

    var shader;
    if (shaderType == "x-shader/x-vertex") {
        shader = gl.createShader(gl.VERTEX_SHADER);
    } else if (shaderType == "x-shader/x-fragment") {
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else {
        return null;
    }

    gl.shaderSource(shader, shaderText);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        return null;
    }

    return shader;
}

function initBuffers() {
    vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    vertexPositionBuffer.itemSize = 3;
    vertexPositionBuffer.itemsCount = 3;
}

function drawScene() {
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    mat4.perspective(pMatrix, 45, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);
    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix,  mvMatrix, [-1.5, 0.0, -7.0]);

    gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, vertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);

    setMatrixUniforms();

    gl.drawArrays(gl.TRIANGLES, 0, vertexPositionBuffer.itemsCount);
}

function setMatrixUniforms() {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}
