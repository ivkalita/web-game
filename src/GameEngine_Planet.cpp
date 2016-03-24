#include "GameEngine.hpp"

#include <sstream>
#include <algorithm>

namespace GameEngine {

    const tfloat Planet::CLOSE_RANGE = 12;
    const tfloat Planet::SHIPS_INCREASE = 0.1;
    int Planet::id_generator = 0;

    static bool IsHypotLessThen(tfloat dx, tfloat dy, tfloat val) {
        return dx*dx + dy*dy < val*val;
    }

    static bool IsHypotLessThen(Vector v1, Vector v2, tfloat val) {
        return IsHypotLessThen(v1.x - v2.x, v1.y - v2.y, val);
    }

    Planet::Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner) :
        radius(_radius), ships_num(_ships_num), owner(_owner), new_ships(0)
    {
        id = gen_id();
        pos = Vector(_x, _y);
    }

    Planet::Planet(const Planet& a) :
        pos(a.pos), radius(a.radius), ships_num(a.ships_num), owner(a.owner), id(a.id), new_ships(a.new_ships) { }

    const Planet& Planet::operator = (const Planet& a) {
        pos = a.pos;
        radius = a.radius;
        ships_num = a.ships_num;
        new_ships = a.new_ships;
        owner = a.owner;
        id = a.id;
        return a;
    }

    void Planet::Step() {
        if (owner == NEUTRAL_PLAYER_ID)
            return;

        if (ships_num >= ShipsMaxCount()) {
            new_ships = 0;
            return;
        }
        new_ships += ShipsIncrease();
        int t = floor(new_ships);
        new_ships -= t;
        ships_num = std::min(ships_num + t, ShipsMaxCount());
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
                new_ships = 0;
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