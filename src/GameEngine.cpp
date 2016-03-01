#define _USE_MATH_DEFINES 

#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    const tfloat Planet::CLOSE_RANGE = 5;
    const tfloat Ship::speed_length = 3;
    int Planet::id_generator = 0;

    static bool IsHypotLessThen(tfloat dx, tfloat dy, tfloat hyp) {
        return dx*dx + dy*dy < hyp*hyp;
    }

    static bool IsHypotLessThen(Vector A, Vector B, tfloat hyp) {
        return IsHypotLessThen(A.x - B.x, A.y - B.y, hyp);
    }




    Planet::Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner) :
        radius(_radius), ships_num(_ships_num), owner(_owner) 
    {
        id = gen_id();
        pos = Vector(_x, _y);
    }

    Planet::Planet(const Planet& a) 
        : pos(a.pos), radius(a.radius), ships_num(a.ships_num), owner(a.owner), id(a.id) { }

    const Planet& Planet::operator = (const Planet& a) {
        pos = a.pos; radius = a.radius; ships_num = a.ships_num; owner = a.owner; id = a.id;
        return *this;
    }
    
    /*bool Planet::IsNear(tfloat _x, tfloat _y) const {
        return IsHypotLessThen(x - _x, y - _y, radius + CLOSE_RANGE);
    }*/

    /*bool Planet::IsInside(tfloat _x, tfloat _y) const {
        return IsHypotLessThen(x - _x, y - _y, radius);
    }*/

    bool Planet::IsNear(Vector v) const {
        return IsHypotLessThen(pos, v, radius + CLOSE_RANGE);
    }

    bool Planet::IsInside(Vector v) const {
        return IsHypotLessThen(pos, v, radius);
    }

    int Planet::ReceiveShips(int count, int ships_owner) {
        if (owner == ships_owner) {
            ships_num += count;
        }
        else {
            ships_num -= count;
            if (ships_num < 0) {
                owner = ships_owner;
                ships_num = -ships_num;
            }
        }
        return ships_num;
    }

    std::string Planet::GetInfo() const {
        std::stringstream s;
        s << "X: " << pos.x << " Y: " << pos.y << " radius: " << radius
            << " ships_num: " << ships_num << " owner: " << owner << " id: " << id;
        return s.str();
    }




    /*void Ship::aim() {
        tfloat dx = dest_planet.GetX() - x, dy = dest_planet.GetY() - y;
        tfloat path_length = hypot(dx, dy);
        vx = speed * dx / path_length;
        vy = speed * dy / path_length;
    }*/
    
    /*Ship::Ship(Planet& _sender_planet, Planet& _dest_planet) :
        sender_planet(_sender_planet), dest_planet(_dest_planet), finished(false) {
        x = sender_planet.GetX() + sender_planet.GetRadius();
        y = sender_planet.GetY();
        owner = sender_planet.GetOwner();
        aim();
    }*/

    Ship::Ship(ShipGroup &group_, tfloat X, tfloat Y) : 
        pos(Vector(X, Y)), group(group_), speed(Vector(0,0)), finished(false) {};
    
    Vector Ship::BoidsRule1() {
        int n = group.Size() - 1;
        return n == 0 ? Vector(0,0) : (group.GetSumOfPos() - GetPos()) / n;
    }

    Vector Ship::BoidsRule2() {
        const tfloat close_enough = 2;
        Vector v;
        for (auto &ship : group.GetShips()) {
            if (IsHypotLessThen(GetPos(), ship.GetPos(), close_enough)) {
                Vector t = GetPos() - ship.GetPos();
                v += t;
            }
        }
        return v;
    }

    Vector Ship::BoidsRule3() {
        int n = group.Size() - 1;
        return n == 0 ? Vector(0, 0) : (group.GetSumOfSpeeds() - GetPos()) / n;
    }

    Vector Ship::BoidsRule4() {
        const tfloat close_enough = Planet::CLOSE_RANGE;
        Vector v;
        for (auto &planet : group.GetPlanets()) {
            if (planet == group.GetDestPlanet())
                continue;
            if (IsHypotLessThen(GetPos(), planet.GetPos(), planet.GetRadius() + close_enough)) {
                Vector t = GetPos() - planet.GetPos();
                v += t;
            }
        }
        return v;
    }

    Vector Ship::BoidsRule5() {
        const tfloat c = 0.85;
        Vector v = group.GetDestPlanet().GetPos() - GetPos();
        v.SetLength(c);
        return v;
    }

    void Ship::CalcSpeed() {
        Vector v;
        const tfloat c1 = 1.0/160;
        const tfloat c2 = 1.0/35;
        const tfloat c3 = 1.0/18;
        const tfloat c4 = 1;
        const tfloat c5 = 1.0/8;
        v += BoidsRule1()*c1;
        v += BoidsRule2()*c2;
        v += BoidsRule3()*c3;
        v += BoidsRule4()*c4;
        v += BoidsRule5()*c5;
        speed += v;
        speed.SetLength(speed_length);
    }

    void Ship::Step() {
        /*aim();
        
        for (auto& p : planets) {
            if (p.IsNear(x, y)) {
                if (p.IsInside(x + vx, y + vy)) {
                    if (p == dest_planet) {
                        p.ReceiveShips(1, owner);
                        finished = true;
                    }
                    tfloat cx = p.GetX() - x;
                    tfloat cy = p.GetY() - y;
                    if (cx * vy - vx * cy >= 0) {
                        tfloat tmp_x = cx;
                        cx = -cy;
                        cy = tmp_x;
                    }
                    else {
                        tfloat tmp_x = cx;
                        cx = cy;
                        cy = -tmp_x;
                    }
                    tfloat length = sqrt(pow(cx, 2) + pow(cy, 2));
                    vx = cx / length * speed;
                    vy = cy / length * speed;
                    break;
                }
            }
        }
        x += vx;
        y += vy;
        */
        pos += speed;
        finished = group.GetDestPlanet().IsInside(GetPos());
    }


    std::string Ship::GetInfo() {
        std::stringstream s;
        s << " X: " << pos.x << " Y: " << pos.y
            << " VX: " << speed.x << " VY: " << speed.y
            << " Sender: " << group.GetSenderPlanet().GetInfo()
            << " Dest: " << group.GetDestPlanet().GetInfo()
            << " owner: " << group.GetOwner() << " finished: " << finished;
        return s.str();
    }




    ShipGroup::ShipGroup(PlanetList &planet_list, Planet &sender, Planet &dest, int ship_count) :
        sender_planet(sender), dest_planet(dest), planets(planet_list)
    {
        Vector V = sender.GetPos();
        tfloat r = sender.GetRadius() + 1;
        for (int i = 0; i < ship_count; i++) {
            tfloat a = rand() * 2 * M_PI / RAND_MAX;
            ships.emplace_back(*this, V.x + r*cos(a), V.y + r*sin(a));
        }
    }
    
    void ShipGroup::StepPrepare() {
        sum_pos = sum_speed = Vector(0,0);
        for (auto& ship : ships) {
            sum_speed += ship.GetSpeed();
            sum_pos += ship.GetPos();
        }
    }

    void ShipGroup::Step() {
        StepPrepare();
        for (auto& ship : ships)
            ship.CalcSpeed();

        auto it = ships.begin();
        while (it != ships.end()) {
            it->Step();
            if (it->Finished()) {
                auto it2 = it++;
                ships.erase(it2);
            }
            else
                it++;
        }
    }
    
    /*void Engine::StepForGroup(ShipList::iterator itBegin, ShipList::iterator itEnd) {
        Vector sum_pos, sum_speed;
        int cnt = -1;
        for (auto it = itBegin; it != itEnd; it++) {
            sum_pos += it->GetPos();
            sum_speed += it->GetSpeed();
            cnt++;
        }
        for (auto ship = itBegin; ship != itEnd; ship++) {
            Vector v1 = (sum_pos - ship->GetPos()) / cnt;
            Vector v3 = (sum_speed - ship->GetSpeed()) / cnt;
        }
    }*/

    void Engine::Step() {
        /*
        for (auto& i : ships) {
            if (i.Finished())
                continue;
            i.Step(planets);

#if _DEBUG
            const int MAX_X = 10000;
            const int MAX_Y = 10000;
            for (auto& i : ships) {
                for (auto& p : planets) {
                    if (p.IsInside(i.GetX(), i.GetY()))
                        throw Poco::Exception("Ship is inside a planet\nShip info: "
                            + i.GetInfo() + "\nPlanet info: " + p.GetInfo() + "\n");
                }
                if (abs(i.GetX()) > MAX_X || abs(i.GetY()) > MAX_Y)
                    throw Poco::Exception("Ship flew away\nShip info: " + i.GetInfo());
            }
#endif
        }
        RemoveFinishedFromFront();
        */
        
        for (auto it = groups.begin(); it != groups.end(); it++) {
            it->Step();
        }
        /*
        auto it = ships.begin();
        while (it != ships.end()) {
            auto itBegin = it;
            auto itEnd = ++it;
            while ( (itEnd != ships.end()) && (itBegin->GetOwner() == itEnd->GetOwner()) )
                itEnd++;
            StepForGroup(itBegin, itEnd);
            it = itEnd;
        }*/

    }

    Planet& Engine::AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner) {
        planets.emplace_back(x, y, radius, ships_num, owner);
        Planet& p = planets.back();
        planets_map[p.GetID()] = &p;
        return p;
    }

    void Engine::Launch(int count, Planet& sender_planet, Planet& dest_planet) {
        if (sender_planet.ShipCount() < count)
            throw Poco::Exception("There aren't so many ships (" + std::to_string(count)
                + " on a planet:\n" + sender_planet.GetInfo());

        sender_planet.RemoveShips(count);
        /*for (int i = 0; i < count; i++) {
            ships.emplace_back(sender_planet, dest_planet);
        }*/
        groups.emplace_back(planets, sender_planet, dest_planet, count);
    }

    /*void Engine::RemoveFinishedFromFront() {
        auto i = ships.begin();
        while (i != ships.end() && i->Finished())
            i++;
        ships.erase(ships.begin(), i);
    }*/


}

/*
#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

int Planet::ReceiveShips(int count, int ships_owner) {
if (owner == ships_owner) {
ships_num += count;
}
else {
ships_num -= count;
if (ships_num < 0) {
owner = ships_owner;
ships_num = -ships_num;
}
}
return ships_num;
}

std::string Planet::GetInfo() {
std::stringstream s;
s << "X: " << x << " Y: " << y << " radius: " << radius
<< " ships_num: " << ships_num << " owner: " << owner << " id: " << id;
return s.str();
}

void Ship::aim() {
tfloat dx = dest_planet.GetX() - x, dy = dest_planet.GetY() - y;
tfloat path_length = sqrt(pow(dx, 2) + pow(dy, 2));
vx = speed * dx / path_length;
vy = speed * dy / path_length;
}

Ship::Ship(Planet& _sender_planet, Planet& _dest_planet) :
sender_planet(_sender_planet), dest_planet(_dest_planet), finished(false) {
x = sender_planet.GetX() + sender_planet.GetRadius();
y = sender_planet.GetY();
owner = sender_planet.GetOwner();
aim();
}

void Ship::Step(std::list<Planet>& planets) {
aim();
for (auto& p : planets) {
if (p.IsNear(x, y)) {
if (p.IsInside(x + vx, y + vy)) {
if (p == dest_planet) {
p.ReceiveShips(1, owner);
finished = true;
}
tfloat cx = p.GetX() - x;
tfloat cy = p.GetY() - y;
if (cx * vy - vx * cy >= 0) {
tfloat tmp_x = cx;
cx = -cy;
cy = tmp_x;
}
else {
tfloat tmp_x = cx;
cx = cy;
cy = -tmp_x;
}
tfloat length = sqrt(pow(cx, 2) + pow(cy, 2));
vx = cx / length * speed;
vy = cy / length * speed;
break;
}
}
}
x += vx;
y += vy;
}

std::string Ship::GetInfo() {
std::stringstream s;
s << " X: " << x << " Y: " << y << " VX: " << vx << " VY: " << vy
<< " Sender: " << sender_planet.GetInfo() << " Dest: " << dest_planet.GetInfo()
<< " owner: " << owner << " finished: " << finished;
return s.str();
}

void Engine::Step() {
for (auto& i : ships) {
if (i.Finished())
continue;
i.Step(planets);

#if _DEBUG
const int MAX_X = 10000;
const int MAX_Y = 10000;
for (auto& i : ships) {
for (auto& p : planets) {
if (p.IsInside(i.GetX(), i.GetY()))
throw Poco::Exception("Ship is inside a planet\nShip info: "
+ i.GetInfo() + "\nPlanet info: " + p.GetInfo() + "\n");
}
if (abs(i.GetX()) > MAX_X || abs(i.GetY()) > MAX_Y)
throw Poco::Exception("Ship flew away\nShip info: " + i.GetInfo());
}
#endif
}
RemoveFinishedFromFront();
}

Planet& Engine::AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner) {
planets.emplace_back(x, y, radius, ships_num, owner);
Planet& p = planets.back();
planets_map[p.GetID()] = &p;
return p;
}

void Engine::Launch(int count, Planet& sender_planet, Planet& dest_planet) {
if (sender_planet.ShipCount() < count)
throw Poco::Exception("There aren't so many ships (" + std::to_string(count)
+ " on a planet:\n" + sender_planet.GetInfo());

sender_planet.RemoveShips(count);
for (int i = 0; i < count; i++) {
ships.emplace_back(sender_planet, dest_planet);
}
}

void Engine::RemoveFinishedFromFront() {
auto i = ships.begin();
while (i != ships.end() && i->Finished())
i++;
ships.erase(ships.begin(), i);
}

const tfloat Planet::CLOSE_RANGE = 5;
const tfloat Ship::speed = 3;
int Planet::id_generator = 0;

}
*/