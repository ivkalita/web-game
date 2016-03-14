#pragma once

#include <cmath>
#include <map>
#include <list>
#include "MathVector.hpp"

namespace GameEngine {

    typedef double tfloat;
    typedef Vector2<tfloat> Vector;

    class Planet;
    class Ship;

    typedef std::list<Planet> PlanetList;
    typedef std::list<Ship> ShipList;

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

        bool operator == (const Planet& a) const { return id == a.id; }
        int ReceiveShips(int count, int ships_owner);
        int RemoveShips(int count) { return ships_num -= count; }

        bool IsNear(Vector v) const;
        bool IsInside(Vector v) const;
        tfloat GetX() const { return pos.x; }
        tfloat GetY() const { return pos.y; }
        Vector GetPos() const { return pos; }
        int GetOwner() const { return owner; }
        int ShipCount() const { return ships_num; }
        tfloat GetRadius() const { return radius; }
        int GetID() const { return id; }
        std::string GetInfo() const;
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
        int GetOwner() const { return owner; }
        tfloat GetX() const { return pos.x; }
        tfloat GetY() const { return pos.y; }
        Vector GetPos() const { return pos; }
        bool Finished() const { return finished; }
        std::string GetInfo() const;
    };

    class Engine {
    private:
        ShipList ships;
        PlanetList planets;
        std::map<int, Planet*> planets_map;
        void RemoveFinishedFromFront();
    public:
        Engine() {}
        void Step();
        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner);
        void Launch(int count, Planet& sender_planet, Planet& dest_planet);
        const ShipList& GetShips() const { return ships; }
        const PlanetList& GetPlanets() const { return planets; }
        const std::map<int, Planet*>& GetPlanetsMap() const { return planets_map; }
        int ActiveShipsCount() const { return (int)ships.size(); }
    };

};
