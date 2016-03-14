#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    const tfloat Planet::CLOSE_RANGE = 5;
    const tfloat Ship::speed_length = 3;
    int Planet::id_generator = 0;

    static bool IsHypotLessThen(tfloat dx, tfloat dy, tfloat val) {
        return dx*dx + dy*dy < val*val;
    }

    static bool IsHypotLessThen(Vector v1, Vector v2, tfloat val) {
        return IsHypotLessThen(v1.x - v2.x, v1.y - v2.y, val);
    }

    Planet::Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner) :
        radius(_radius), ships_num(_ships_num), owner(_owner) 
    {
        id = gen_id();
        pos = Vector(_x, _y);
    }

    Planet::Planet(const Planet& a) : 
        pos(a.pos), radius(a.radius), ships_num(a.ships_num), owner(a.owner), id(a.id) { }

    const Planet& Planet::operator = (const Planet& a) {
        pos = a.pos; 
        radius = a.radius; 
        ships_num = a.ships_num; 
        owner = a.owner; 
        id = a.id;
        return a;
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

    void Ship::aim() {
        speed = dest_planet.GetPos() - GetPos();
        speed.SetLength(speed_length);
    }

    Ship::Ship(Planet& _sender_planet, Planet& _dest_planet) :
        sender_planet(_sender_planet), dest_planet(_dest_planet), finished(false) {
        pos.x = sender_planet.GetX() + sender_planet.GetRadius();
        pos.y = sender_planet.GetY();
        owner = sender_planet.GetOwner();
        aim();
    }

    void Ship::Step(std::list<Planet>& planets) {
        aim();
        for (auto& p : planets) {
            if (p.IsNear(pos)) {
                if (p.IsInside(pos + speed)) {
                    if (p == dest_planet) {
                        p.ReceiveShips(1, owner);
                        finished = true;
                    }
                    Vector c = p.GetPos() - pos;
                    if (c.x * speed.y - speed.x * c.y >= 0) {
                        c = Vector(-c.y, c.x);
                    }
                    else {
                        c = Vector(c.y, -c.x);
                    }
                    speed = c;
                    speed.SetLength(speed_length);
                    break;
                }
            }
        }
        pos += speed;
    }

    std::string Ship::GetInfo() const {
        std::stringstream s;
        s << " X: " << pos.x << " Y: " << pos.y << " VX: " << speed.x << " VY: " << speed.y
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

}
