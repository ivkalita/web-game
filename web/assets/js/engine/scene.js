function Scene(canvas) {
    var _canvas = canvas;
    var _gameObjects = {};
    var _needCreateOrDelete = true;
    var _needUpdate = true;
    var _objectId = 0;
    var _elapsedTime = 0;

    this.render = function() {
        new Engine(this).render();
    };

    this.updateObjects = function() { updateObjects(this); };

    this.addObject = function(object) {
        object.id = _objectId;
        _gameObjects[_objectId] = object;
        ++_objectId;
    };

    this.removeObject = function(object) {
        var idx = object.id;
        if (idx in _gameObjects) {
            delete _gameObjects[idx];
        }
    };

    Object.defineProperties(this, {
        "gameObjects": { value: _gameObjects, writable: true },
        "canvas": { value: _canvas, writable: true },
        "needCreateOrDelete": { value: _needCreateOrDelete, writable: true },
        "needUpdate": { value: _needUpdate, writable: true},
        "elapsedTime": { value: _elapsedTime, writable: true }
    });
}

Scene.prototype.constructor = Scene;

function startGame() {
    var s = new Scene($("#game-screen")[0]);
    s.render();


}

function updateObjects(scene) {
    //the code to receive messages from server needs to be here

    if (scene.needCreateOrDelete) {
        //some testing code here

        for (var i = 0; i < 2000; i++) {
            var m = Math.random();
            var n = Math.random();
            scene.addObject(new Ship(1.0 + n, 1.1 + m, [m, n]));
        };

        //s = new Ship(0.0, 0.0, [3.0, 1.0]);
        //s.x = 1.0;
        //s.moveTo(-3.0, 0.0);
        //scene.addObject(s);
//
        //p = new Planet(2.0, 0.0);
        //p.radius = 1;
        //scene.addObject(p);
        //scene.addObject(new Planet(1.0, 1.0));
//
        //scene.addObject(new Ship(0.0, 1.0, [1.0, 1.0]));
//
        //scene.removeObjectAtCoords([1.0, 1.0]);
    }

    scene.needUpdate = true;
    var m = Math.random();
    if (m > 0.01) {
        for (var o in scene.gameObjects) {
            if (scene.gameObjects.hasOwnProperty(o)) {
                var curObj = scene.gameObjects[o];

                //curObj.vertices[0] = m;
                //curObj.moveTo(0.0, 0.0);
                curObj.rotate(1.0 * scene.elapsedTime);
            }
        }
    }


    //for (var o in scene.gameObjects) {
    //    if (scene.gameObjects.hasOwnProperty(o)) {
    //        var curObj = scene.gameObjects[o];
    //        var m = Math.random();
    //        curObj.moveTo(1 + m, m);
    //        //curObj.moveTo(-1.0, -0.1);
    //    }
    //}
}
