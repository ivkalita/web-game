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
            const int MAX_X = 250;
            const int MAX_Y = 250;
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
