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





    Ship::Ship(ShipGroup &group_, tfloat X, tfloat Y) :
        pos(Vector(X, Y)), group(group_), speed(Vector(0, 0)), finished(false) {};

    Vector Ship::BoidsRule1() {
        Vector v = group.GetDestPlanet().GetPos() - GetPos();
        v.SetLength(1);
        return v;
    }

    Vector Ship::BoidsRule2() {
        const tfloat close_enough = 4;
        Vector v;
        for (auto &ship : group.GetShips()) {
            Vector r = GetPos() - ship.GetPos();
            if (r.GetLength() < close_enough) {
                r.SetLength(0.8);
                v += r;
            }
        }
        v.SetLength(0.4);
        return v;
    }

    Vector Ship::BoidsRule3() {
        Vector v;
        for (auto &planet : group.GetPlanets()) {
            if (planet == group.GetDestPlanet())
                continue;
            if (planet.IsNear(GetPos())) {
                Vector t = GetPos() - planet.GetPos();
                Vector r = group.GetDestPlanet().GetPos() - planet.GetPos();
                if (r.x*t.y + r.y*t.x < 0) {
                    r = Vector(r.y, -r.x);
                    if (r.x*t.y + r.y*t.x < 0)
                        r = -r;
                    t = r;
                }
                t.SetLength(1);
                v += t;
            }
        }
        return v;
    }

    Vector Ship::BoidsRule4() {
        return (group.GetAveragePos() - GetPos())*0.01;
    }

    void Ship::CalcSpeed() {
        Vector v;
        v += BoidsRule1();
        v += BoidsRule2();
        v += BoidsRule3();
        v += BoidsRule4();
        speed = v;
        speed.SetLength(speed_length);
    }

    void Ship::Move() {
        pos += speed;
        finished = group.GetDestPlanet().IsInside(GetPos());
#if _DEBUG
        const int MAX_X = 10000;
        const int MAX_Y = 10000;
        if (abs(pos.x) > MAX_X || abs(pos.y) > MAX_Y)
            throw Poco::Exception("Ship flew away\nShip info: " + GetInfo());
        for (auto& p : group.GetPlanets()) {
            if (p.IsInside(pos))
                throw Poco::Exception("Ship is inside a planet\nShip info: "
                    + GetInfo() + "\nPlanet info: " + p.GetInfo() + "\n");
        }
#endif
    }

    std::string Ship::GetInfo() const {
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
        owner = sender.GetOwner();
        tfloat r = sender.GetRadius() + 1;

        Vector C = sender.GetPos();
        Vector goal = dest.GetPos() - C;
        tfloat delta = min(M_PI / ship_count, 0.2);
        tfloat alpha = atan2(goal.y, goal.x) - delta*(ship_count / 2);

        for (int i = 0; i < ship_count; i++) {
            tfloat a = alpha + delta*i;
            ships.emplace_back(*this, C.x + r*cos(a), C.y + r*sin(a));
        }
    }

    void ShipGroup::StepPrepare() {
        average_pos = Vector(0, 0);
        for (auto& ship : ships) {
            average_pos += ship.GetPos();
        }
        average_pos /= ships.size();
    }

    void ShipGroup::Step() {
        StepPrepare();
        for (auto& ship : ships)
            ship.CalcSpeed();

        auto it = ships.begin();
        while (it != ships.end()) {
            it->Move();
            if (it->Finished()) {
                auto it2 = it++;
                dest_planet.ReceiveShips(1, owner);
                ships.erase(it2);
            }
            else
                it++;
        }
        finished = ships.empty();
    }





    void Engine::Step() {
        auto it = groups.begin();
        while (it != groups.end()) {
            it->Step();
            if (it->IsFinished()) {
                auto it2 = it++;
                groups.erase(it2);
            }
            else
                it++;
        }
    }

    Planet& Engine::AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner) {
        planets.emplace_back(x, y, radius, ships_num, owner);
        Planet& p = planets.back();
        planets_map[p.GetID()] = &p;
        return p;
    }

    void Engine::Launch(int count, Planet& sender_planet, Planet& dest_planet) {
        if (count == 0)
            return;
        if (sender_planet.ShipCount() < count)
            throw Poco::Exception("There aren't so many ships (" + std::to_string(count)
                + " on a planet:\n" + sender_planet.GetInfo());

        sender_planet.RemoveShips(count);
        groups.emplace_back(planets, sender_planet, dest_planet, count);
    }

    int Engine::ActiveShipsCount() const {
        int ans = 0;
        for (auto &group : groups)
            ans += group.Size();
        return ans;
    }


}