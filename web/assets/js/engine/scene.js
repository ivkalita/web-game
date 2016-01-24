function Scene(canvas) {
    this._canvas = canvas;
    this._gameObjects = [];
    this._needUpdate = true;
    this._gl;
    
    this.draw = function() {
        console.log('Draw');
        this._gl.clear(this._gl.COLOR_BUFFER_BIT | this._gl.DEPTH_BUFFER_BIT);
        //this._gl.viewport(0, 0, this._gl.viewportHeight, this._gl.viewportWidth);
        var perspectMat = [];
        mat4.perspective(perspectMat, 90, this._gl.viewportWidth / this._gl.viewportHeight, 0.1, 100);
        for (var i = 0; i < this._gameObjects.length; i++) {
            switch (this._gameObjects[i].draw(this._gl, perspectMat)) {
                case null:
                    this._gameObjects.splice(i, 1);
                    //console.log('Objects count: ', this._gameObjects.length);
                    break;
                default:
            }
        }
        this._needUpdate = false;
    };

    this.pushObject = function(object) {
        this._gameObjects.push(object);
    };
    
    this.unshiftObject = function(object) {
        this._gameObjects.unshift(object);
    };
    
    this.invalidate = function() {
        this._needUpdate = true;
    }
    
    this.initGl();
    
    this.render = function() {
        //if (this._needUpdate)
            this.draw();
        requestAnimationFrame(this.render.bind(this));
    };
    this.render();

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
    var names = ["webgl", "experimental-webgl", "webkit-3d", "moz-webgl", '3d'];
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
    this._gl.clearColor(0, 0, 0, 1);
    //this._gl.enable(this._gl.DEPTH_TEST);
    this._gl.enable(this._gl.BLEND);
    this._gl.blendFunc(this._gl.SRC_ALPHA, this._gl.ONE_MINUS_SRC_ALPHA);
}

var movingShip, starsCount = 0;

function startGame() {
    scene = new Scene($("#game-screen")[0]);
    
    starsCount += 10;
    for (var i = 0; i < 10; i++) {
        scene.pushObject(new Star(Math.random() * 4 - 2, Math.random() * 3 - 1.5));
    }
    
    scene.pushObject(new Planet(-1, 0, [1, 0, 0]));
    scene.pushObject(new Planet(1, -1, [0, 1, 0]));
    scene.pushObject(new Planet(0, -1, [1, 0, 0], 1));
    scene.pushObject(new Planet(-1, 1, [0, 1, 0], 2));
    
    scene.pushObject(new Ship(0, 0, [1, 0, 0], 0));
    scene.pushObject(new Ship(0, 1, [0, 1, 0], 0));
    scene.pushObject(new Ship(1, 0.5, [0, 0, 1], 0));
    
    movingShip = new Ship(0, 0, [0, 1, 1], 0);
    scene.pushObject(movingShip);
    movingShip.x = -0.5;
    movingShip.y = 1;
    
    setInterval(actions, 5000);
}

var flag = 0;
function actions() {
    console.log('Action');
    switch (flag) {
    case 0:
        movingShip.x = 0;
        movingShip.y = 0;
        break;
    case 1:
        movingShip.x = 0;
        movingShip.y = 1;
        break;
    case 2:
        movingShip.x = 0.5;
        movingShip.y = 1;
        break;
    case 3:
        movingShip.x = 1;
        movingShip.y = 0;
        break;
    }
    flag = (flag + 1) % 4;
    if (Star.prototype._count < 50) {
        scene.unshiftObject(new Star(Math.random() * 4 - 2, Math.random() * 3 - 1.5));
    }
    scene.invalidate();
}
