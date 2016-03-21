#include "GameEngine.hpp"

#include <sstream>

namespace GameEngine {

    const tfloat Planet::CLOSE_RANGE = 20;
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

}