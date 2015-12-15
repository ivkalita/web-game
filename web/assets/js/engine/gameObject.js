//class GameObject
function GameObject(x, y, vertices, vertSize, vertCount, attribs, uniforms, boundingBox) {
    this._x = x;
    this._y = y;
    var _vertices = vertices;
    var _vertSize = vertSize;
    var _vertCount = vertCount;
    var _attribs = attribs;
    var _uniforms = uniforms;
    var _boundingBox = boundingBox;

    Object.defineProperties(this, {
        "vertices": { value: _vertices, writable: true },
        "vertSize": { value: _vertSize, writable: true },
        "vertCount": { value: _vertCount, writable: true },
        "attribs": { value: _attribs, writable: true },
        "uniforms": { value: _uniforms, writable: true },
        "boundingBox": { value: _boundingBox, writable: true }
    });
}

GameObject.prototype.constructor = GameObject;

//class Ship
function Ship(x, y, speedVector) {
    var _board = 0.1;

    this.calcShipVertices = function (x, y, l) {
        return [
            x - l / 2, y - Math.sqrt(3) / 6 * l, 0.0,
            x,         y + Math.sqrt(3) / 3 * l, 0.0,
            x + l / 2, y - Math.sqrt(3) / 6 * l, 0.0
        ];
    };

    GameObject.apply(this, [
        x, y, this.calcShipVertices(x, y, _board), 3, 3, {"aVertexPosition": 0}, {"uPMatrix": [], "uMVMatrix": []},
        null]);

    this.calcDirection = function(v) {
        if (v[0] == 0) return 0;
        var rad = Math.acos(v[0] / Math.sqrt(Math.pow(v[0], 2) + Math.pow(v[1], 2)));
        return 180.0 / Math.PI * rad;
    };

    var _direction = this.calcDirection(speedVector);

    Object.defineProperties(this, {
        "x": {
            get: function() { return this._x; },
            set: function(newX) {
                this._x = newX; this.vertices = this.calcShipVertices(newX, y, _board);
            }},
        "y": {
            get: function() { return this._y; },
            set: function(newY) {
                this._y = newY; this.vertices = this.calcShipVertices(x, newY, _board);
            }},
        "board": { value: _board, writable: true },
        "direction": { value: _direction, writable: true  }
    });
}

Ship.prototype = Object.create(GameObject.prototype);
Ship.prototype.constructor = Ship;

//class Planet
function Planet(x, y) {
    var _radius = 0.5;
    var _shipsCount = 0;

    this.calcPlanetVertices = function(x, y, r) {
        var vertices = [
            x,     y, 0.0,
            x + r, y, 0.0
        ];
        var polyNum = 100;
        var degPerPoly = (2 * Math.PI) / polyNum;

        for (var i = 0; i <= polyNum; i++) {
            var index = 2 * 3 + i * 3;
            var angle = degPerPoly * (i + 1);

            vertices[index] = x + r * Math.cos(angle);
            vertices[index + 1] = y + r * Math.sin(angle);
            vertices[index + 2] = 0;
        }

        return vertices;
    };

    this.calcBoundingBox = function(x, y, r) {
        return {"x": x - r, "y": y + r, "width": 2 * r, "height": 2 * r };
    };

    GameObject.apply(this, [
        x, y, this.calcPlanetVertices(x, y, _radius), 3, 103, {"aVertexPosition": 0, "aColor": [1.0, 1.0, 1.0, 1.0]},
        {"uPMatrix": [], "uMVMatrix": []}, this.calcBoundingBox(x, y, _radius)]);

    Object.defineProperties(this, {
        "x": {
            get: function() { return this._x; },
            set: function(newX) {
                this._x = newX; this.vertices = this.calcPlanetVertices(newX, y, _radius);
            }},
        "y": {
            get: function() { return this._y; },
            set: function(newY) {
                this._y = newY; this.vertices = this.calcPlanetVertices(x, newY, _radius);
            }},
        "radius": {
            get: function() { return _radius; },
            set: function(radius) {
                _radius = radius; this.vertices = this.calcPlanetVertices(x, y, radius);
            }},
        "shipsCount": { value: _shipsCount, writable: true }
    });
}

Planet.prototype = Object.create(GameObject.prototype);
Planet.prototype.constructor = Planet;