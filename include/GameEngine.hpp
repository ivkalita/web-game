#pragma once

#include <cmath>
#include <map>
#include <list>

namespace GameEngine {

    typedef double tfloat;

    class Planet {
    private:
        tfloat x, y;
        tfloat radius;
        int ships_num, owner, id;
        static int id_generator;
        static int gen_id() { return Planet::id_generator++; }
    public:
        static const tfloat CLOSE_RANGE;
        Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner) :
            x(_x), y(_y), radius(_radius), ships_num(_ships_num), owner(_owner) {
            id = gen_id();
        }
        // явные конструкторы для отслеживания копирования при работе со сслыками
        Planet(const Planet& a): x(a.x), y(a.y), radius(a.radius), ships_num(a.ships_num), owner(a.owner), id(a.id) { }
        Planet& operator = (const Planet& a) { x = a.x; y = a.y; radius = a.radius; ships_num = a.ships_num; owner = a.owner; id = a.id; }
        bool operator == (const Planet& a) { return id == a.id; }
        bool IsNear(tfloat _x, tfloat _y) { return sqrt(pow(x - _x, 2) + pow(y - _y, 2)) < radius + CLOSE_RANGE; }
        bool IsInside(tfloat _x, tfloat _y) { return sqrt(pow(x - _x, 2) + pow(y - _y, 2)) < radius; }
        int ReceiveShips(int count, int ships_owner);
        int RemoveShips(int count) { return ships_num -= count; }
        tfloat GetX() { return x; }
        tfloat GetY() { return y; }
        int GetOwner() { return owner; }
        int ShipCount() { return ships_num; }
        tfloat GetRadius() { return radius; }
        int GetID() { return id; }
        std::string GetInfo();
    };

    class Ship {
    private:
        tfloat x, y, vx, vy;
        Planet & sender_planet, & dest_planet;
        int owner;
        bool finished;
        void aim();
    public:
        static const tfloat speed;
        Ship(Planet& _sender_planet, Planet& _dest_planet);
        void Step(std::list<Planet>& planets);
        int GetOwner() { return owner; }
        tfloat GetX() { return x; }
        tfloat GetY() { return y; }
        bool Finished() const { return finished; }
        std::string GetInfo();
    };

    class Engine {
    private:
        std::list<Ship> ships;
        std::list<Planet> planets;
        std::map<int, Planet*> planets_map;
        void RemoveFinishedFromFront();
    public:
        Engine() {}
        void Step();
        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner);
        void Launch(int count, Planet& sender_planet, Planet& dest_planet);
        std::list<Ship>& GetShips() { return ships; }
        std::list<Planet>& GetPlanets() { return planets; }
        std::map<int, Planet*>& GetPlanetsMap() { return planets_map; }
        int ActiveShipsCount() { return (int)ships.size(); }
    };

};
