#pragma once

//#include <cmath>
#include <map>
#include <list>

namespace GameEngine {

    typedef double tfloat;

    class Ship;
    class ShipGroup;
    class Planet;

    typedef std::list<Ship> ShipList;
    typedef std::list<ShipGroup> ShipGroupList;
    typedef std::list<Planet> PlanetList;

    struct Vector {
        tfloat x, y;
        Vector() : x(0), y(0) {};
        Vector(tfloat X, tfloat Y) : x(X), y(Y) {};

        tfloat GetLength() { return hypot(x, y); }

        Vector operator + (const Vector &p) const { return Vector(x + p.x, y + p.y); }
        Vector operator - (const Vector &p) const { return Vector(x - p.x, y - p.y); }
        Vector operator * (tfloat k) const { return Vector(x * k, y * k); }
        Vector operator / (tfloat k) const { return Vector(x / k, y / k); }
        Vector operator - () const { return Vector(-x, -y); }

        Vector operator += (const Vector &p) {
            x += p.x; y += p.y;
            return *this;
        }
        Vector operator -= (const Vector &p) {
            x -= p.x; y -= p.y;
            return *this;
        }
        Vector operator *= (tfloat k) {
            x *= k; y *= k;
            return *this;
        }
        Vector operator /= (tfloat k) {
            x /= k; y /= k;
            return *this;
        }

        void SetLength(tfloat len) {
            if (x == 0 && y == 0)
                return;
            tfloat k = len / GetLength();
            x *= k; y *= k;
        }
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
        bool IsNear(Vector v) const;
        bool IsInside(Vector v) const;
        int ReceiveShips(int count, int ships_owner);
        int RemoveShips(int count) { return ships_num -= count; }

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
        Planet &sender_planet, &dest_planet;
        PlanetList &planets;
        int owner;
        Vector average_pos;
        bool finished;

        void StepPrepare();
    public:
        ShipGroup(PlanetList &planet_list, Planet &sender, Planet &dest, int ship_count);
        void Step();
        Vector GetAveragePos() const { return average_pos; }
        const ShipList& GetShips() const { return ships; }
        const PlanetList& GetPlanets() const { return planets; }
        int GetOwner() const { return owner; }
        int Size() const { return ships.size(); }
        bool IsFinished() const { return finished; }
        Planet GetSenderPlanet() const { return sender_planet; }
        Planet GetDestPlanet() const { return dest_planet; }
    };

    class Ship {
    private:
        Vector speed, pos;
        ShipGroup &group;
        bool finished;

        Vector BoidsRule1();
        Vector BoidsRule2();
        Vector BoidsRule3();
        Vector BoidsRule4();
    public:
        static const tfloat speed_length;
        Ship(ShipGroup &group_, tfloat X, tfloat Y);
        void CalcSpeed();
        void Move();
        int GetOwner() const { return group.GetOwner(); }
        tfloat GetX() const { return pos.x; }
        tfloat GetY() const { return pos.y; }
        Vector GetPos() const { return pos; }
        Vector GetSpeed() const { return speed; }
        bool Finished() const { return finished; }
        std::string GetInfo() const;
    };

    class Engine {
    private:
        std::list<Planet> planets;
        ShipGroupList groups;
        std::map<int, Planet*> planets_map;
    public:
        Engine() {}
        void Step();
        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner);
        void Launch(int count, Planet& sender_planet, Planet& dest_planet);
        PlanetList& GetPlanets() { return planets; }
        const ShipGroupList& GetGroups() const { return groups; }
        std::map<int, Planet*>& GetPlanetsMap() { return planets_map; }
        int ActiveShipsCount() const;
    };

};
