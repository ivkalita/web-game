#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    const tfloat Ship::speed_length = 3;
    
    void Ship::aim() {
        speed = group.GetDestPlanet().GetPos() - GetPos();
        speed.SetLength(speed_length);
    }

    Ship::Ship(ShipGroup &group_, tfloat X, tfloat Y) :
        group(group_), finished(false), pos(Vector(X, Y)) {}

    void Ship::Step() {
        aim();
        for (auto& p : group.GetPlanets()) {
            if (p.IsNear(pos)) {
                if (p.IsInside(pos + speed)) {
                    if (p == group.GetDestPlanet()) {
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
        s << " X: " << pos.x << " Y: " << pos.y 
            << " VX: " << speed.x << " VY: " << speed.y
            << " Sender: " << group.GetSenderPlanet().GetInfo() 
            << " Dest: " << group.GetDestPlanet().GetInfo()
            << " owner: " << group.GetOwner() 
            << " finished: " << finished;
        return s.str();
    }

}