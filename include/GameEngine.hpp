#pragma once

#include <cmath>
#include <map>
#include <list>
#include "MathVector.hpp"

namespace GameEngine {

    typedef double tfloat;
    typedef Vector2<tfloat> Vector;

    class Planet {
    private:
        Vector pos;
        tfloat radius;
        int ships_num, owner, id;
        static int id_generator;
        static int gen_id() { return Planet::id_generator++; }
    public:
        static const tfloat CLOSE_RANGE;
        Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner);
        // явные конструкторы для отслеживания копирования при работе со сслыками
        Planet(const Planet& a);
        const Planet& operator = (const Planet& a);

        bool operator == (const Planet& a) { return id == a.id; }
        int ReceiveShips(int count, int ships_owner);
        int RemoveShips(int count) { return ships_num -= count; }

        bool IsNear(Vector v);
        bool IsInside(Vector v);
        tfloat GetX() { return pos.x; }
        tfloat GetY() { return pos.y; }
        Vector GetPos() { return pos; }
        int GetOwner() { return owner; }
        int ShipCount() { return ships_num; }
        tfloat GetRadius() { return radius; }
        int GetID() { return id; }
        std::string GetInfo();
    };

    class Ship {
    private:
        Vector pos, speed;
        Planet &sender_planet, &dest_planet;
        int owner;
        bool finished;
        void aim();
    public:
        static const tfloat speed_length;
        Ship(Planet& _sender_planet, Planet& _dest_planet);
        void Step(std::list<Planet>& planets);
        int GetOwner() { return owner; }
        tfloat GetX() { return pos.x; }
        tfloat GetY() { return pos.y; }
        Vector GetPos() { return pos; }
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
