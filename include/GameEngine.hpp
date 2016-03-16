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
    class ShipGroup;

    typedef std::list<Planet> PlanetList;
    typedef std::list<Ship> ShipList;
    typedef std::list<ShipGroup> ShipGroupList;

    class Engine {
    private:
        ShipGroupList groups;
        PlanetList planets;
        std::map<int, Planet*> planets_map;
    public:
        Engine() {}
        void Step();
        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner);
        void Launch(int count, Planet& sender_planet, Planet& dest_planet);
        const ShipGroupList& GetGroups() const { return groups; }
        const PlanetList& GetPlanets() const { return planets; }
        const std::map<int, Planet*>& GetPlanetsMap() const { return planets_map; }
        int ActiveShipsCount() const;
    };

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

    class ShipGroup {
    private:
        ShipList ships;
        tfloat top, bot, left, right;           // top < bot; left < right
        Engine &engine;
        Planet &sender_planet, &dest_planet;
        int owner;
        bool finished;
        Vector sum_pos;
        Vector sum_speed;
        std::list<ShipGroupList::const_iterator> groups_overlapping;
        bool IsOverlapping(const ShipGroup& group);
        void FindOverlappingGroups();
    public:
        ShipGroup(Engine &engine_, Planet &sender, Planet &dest, int ship_count);
        void StepPrepare();
        void Step();
        const ShipList& GetShips() const { return ships; }
        const PlanetList& GetPlanets() const { return engine.GetPlanets(); }
        const std::list<ShipGroupList::const_iterator>& 
            GetOverlappingGroups() const { return groups_overlapping; }
        int GetOwner() const { return owner; }
        int Size() const { return ships.size(); }
        Vector GetSumOfPos() const { return sum_pos; }
        Vector GetSumOfSpeed() const { return sum_speed; }
        bool IsFinished() const { return finished; }
        const Planet& GetSenderPlanet() const { return sender_planet; }
        const Planet& GetDestPlanet() const { return dest_planet; }
    };

    class Ship {
    private:
        Vector pos, speed;
        ShipGroup &group;
        bool finished;

        Vector BoidsRule1();
        Vector BoidsRule2();
        Vector BoidsRule3();
        Vector BoidsRule4();
        Vector BoidsRule5();
        void Aim();
    public:
        static const tfloat speed_length;
        static const tfloat CLOSE_RANGE;
        Ship(ShipGroup &group_, tfloat X, tfloat Y);

        void Step();
        int GetOwner() const { return group.GetOwner(); }
        tfloat GetX() const { return pos.x; }
        tfloat GetY() const { return pos.y; }
        Vector GetPos() const { return pos; }
        Vector GetSpeed() const { return speed; }
        bool Finished() const { return finished; }
        std::string GetInfo() const;
    };

};
