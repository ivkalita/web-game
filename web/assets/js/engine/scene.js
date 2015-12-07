function Scene(canvas) {
    var _canvas = canvas;
    var _gameObjects = {};
    var _needUpdate = true;

    this.render = function() {
        new Engine(this).render();
    };

    this.updateObjects = function() { updateObjects(this); };

    this.addObject = function(object) {
        var coordsKey = String(object.x) + ":" + String(object.y);
        _gameObjects[coordsKey] = object;
    };

    this.removeObjectAtCoords = function(coords) { //bad function, it needs to think how to kill objects properly
        var coordsKey = String(coords[0]) + ":" + String(coords[1]);
        delete _gameObjects[coordsKey];
    };

    Object.defineProperties(this, {
        "gameObjects": { get: function() { return _gameObjects; }, set: function(gameObjects) { _gameObjects = gameObjects; } },
        "canvas": { get: function() { return _canvas; }, set: function(canvas) { _canvas = canvas; } },
        "needUpdate": { get: function() { return _needUpdate; }, set: function(needUpdate) { _needUpdate = needUpdate; } }
    });
}

Scene.prototype.constructor = Scene;

function startGame() {
    s = new Scene($("#game-screen")[0]);
    s.render();
}

function updateObjects(scene) {
    //the code to receive messages from server needs to be here

    if (scene.needUpdate) {
        //some testing code here

    }

    scene.needUpdate = false;
}