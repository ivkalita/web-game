function Scene(canvas) {
    this._canvas = canvas;
    this._gameObjects = [];
    this._needUpdate = true;
    this._gl;
    
    this.render = function() {
        if (this._needUpdate) this.draw();
    };
    
    this.draw = function() {
        this._gl.clear(this._gl.COLOR_BUFFER_BIT | this._gl.DEPTH_BUFFER_BIT);
        //this._gl.viewport(0, 0, this._gl.viewportHeight, this._gl.viewportWidth);
        var perspectMat = [];
        mat4.perspective(perspectMat, 90, this._gl.viewportWidth / this._gl.viewportHeight, 0.1, 100);
        for (var i = 0; i < this._gameObjects.length; i++) {
            this._gameObjects[i].draw(this._gl, perspectMat);
        }
        this._needUpdate = false;
    };

    this.addObject = function(object) {
        this._gameObjects.push(object);
    };
    
    this.invalidate = function() {
        this._needUpdate = true;
    }
    
    this.initGl();

    Object.defineProperties(this, {
    });
}

Scene.prototype.initGl = function() {
    var width = this._canvas.clientWidth;
    var height = this._canvas.clientHeight;
    if (this._canvas.width != width || this._canvas.height != height) {
        this._canvas.width = width;
        this._canvas.height = height;
    }
    this._gl = null;
    var names = ["webgl", "experimental-webgl", "webkit-3d", "moz-webgl"];
    for (var i = 0; i < names.length; i++) {
        try {
            this._gl = this._canvas.getContext(names[i]);
        } catch(e) {}

        if (this._gl) {
            this._gl.viewportWidth = this._canvas.width;
            this._gl.viewportHeight = this._canvas.height;

            break;
        } else {
            alert("Could not initialize WebGL");
        }
    }
    this._gl.clearColor(0.2, 0.2, 0.2, 1);
    this._gl.enable(this._gl.DEPTH_TEST);
    requestAnimationFrame(this.render.bind(this));
    this.draw();
}

function startGame() {
    scene = new Scene($("#game-screen")[0]);
    
    scene.addObject(new Ship(0, 0, 0, [1, 0, 0]));
    scene.addObject(new Ship(0, 1, 0, [0, 1, 0]));
    scene.addObject(new Ship(1, 1, 0, [0, 0, 1]));
    
    scene.addObject(new Planet(1, 0));
    scene.addObject(new Planet(1, 1));
    
    scene.invalidate();
    scene.render();
}
