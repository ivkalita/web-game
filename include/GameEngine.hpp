#pragma once

#include <cmath>
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

        Vector operator += (const Vector &p) {
            x += p.x; y += p.y;
            return *this;
        }
        Vector operator -= (const Vector &p) {
            x -= p.x; y -= p.y;
            return *this;
        }

        void SetLength(tfloat len) {
            if (x == 0 && y == 0)
                return;
            tfloat k = len / GetLength();
            x *= k; y *= k;      
        }
    };
    //Vector operator * (tfloat k, Vector v) { return v*k; }

    class Planet {
    private:
        //tfloat x, y;
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
        //bool IsNear(tfloat _x, tfloat _y) const;
        //bool IsInside(tfloat _x, tfloat _y) const;
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
        Vector sum_pos;
        Vector sum_speed;
        void StepPrepare();
    public:
        ShipGroup(PlanetList &planet_list, Planet &sender, Planet &dest, int ship_count);
        void Step();
        Vector GetAveragePos() const { return sum_pos / ships.size(); }
        Vector GetAverageSpeed() const { return sum_speed / ships.size(); }
        Vector GetSumOfPos() const { return sum_pos; }
        Vector GetSumOfSpeeds() const { return sum_speed; }
        const ShipList& GetShips() const { return ships; }
        const PlanetList& GetPlanets() const { return planets; }
        int GetOwner() const { return owner; }
        //int GetShipsCount() const { return ships.size(); }
        int Size() const { return ships.size(); }
        Planet GetSenderPlanet() const { return sender_planet; }
        Planet GetDestPlanet() const { return dest_planet; }
    };

    class Ship {
    private:
        //tfloat x, y, vx, vy;
        //Planet & sender_planet, & dest_planet;
        //int owner;
        Vector speed, pos;
        ShipGroup &group;
        bool finished;
        //void aim();

        //friend void ShipGroup::Step();

        Vector BoidsRule1();
        Vector BoidsRule2();
        Vector BoidsRule3();
        Vector BoidsRule4();
        Vector BoidsRule5();
    public:
        static const tfloat speed_length;
        //Ship(Planet& _sender_planet, Planet& _dest_planet);
        Ship(ShipGroup &group_, tfloat X, tfloat Y);
        void Step();
        void CalcSpeed();
        int GetOwner() const { return group.GetOwner(); }
        tfloat GetX() const { return pos.x; }
        tfloat GetY() const { return pos.y; }
        Vector GetPos() const { return pos; }
        Vector GetSpeed() const { return speed; }
        bool Finished() const { return finished; }
        std::string GetInfo();
    };

    class Engine {
    private:
        //std::list<Ship> ships;
        std::list<Planet> planets;
        ShipGroupList groups;
        std::map<int, Planet*> planets_map;
        //void RemoveFinishedFromFront();
        //void StepForGroup(ShipList::iterator itBegin, ShipList::iterator itEnd);
    public:
        Engine() {}
        void Step();
        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner);
        void Launch(int count, Planet& sender_planet, Planet& dest_planet);
        //std::list<Ship>& GetShips() { return ships; }
        //std::list<Planet>& GetPlanets() { return planets; }
        PlanetList& GetPlanets() { return planets; }
        const ShipGroupList& GetGroups() const { return groups; }
        std::map<int, Planet*>& GetPlanetsMap() { return planets_map; }
        //int ActiveShipsCount() { return (int)ships.size(); }
    };

};

/*
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
const Planet& operator = (const Planet& a) {
x = a.x; y = a.y; radius = a.radius; ships_num = a.ships_num; owner = a.owner; id = a.id;
return a;
}
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
*/