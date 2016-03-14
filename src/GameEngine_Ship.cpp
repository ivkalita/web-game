#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    const tfloat Ship::speed_length = 3;

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

}