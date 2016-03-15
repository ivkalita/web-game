#define _USE_MATH_DEFINES 

#include "GameEngine.hpp"

#include <sstream>
#include <algorithm>

namespace GameEngine {

    bool ShipGroup::IsOverlapping(const ShipGroup& g) {
        tfloat l = g.left - Ship::CLOSE_RANGE;
        tfloat r = g.right + Ship::CLOSE_RANGE;
        tfloat t = g.top - Ship::CLOSE_RANGE;
        tfloat b = g.bot + Ship::CLOSE_RANGE;

        tfloat L = std::min(l, left);
        tfloat R = std::max(r, right);
        tfloat T = std::min(t, top);
        tfloat B = std::max(b, bot);

        bool X = (R - L <= r - l + right - left);
        bool Y = (B - T <= b - t + bot - top);
        return X && Y;
    }

    void ShipGroup::FindOverlappingGroups() {
        groups_overlapping.clear();
        for (auto it = engine.GetGroups().begin(); it != engine.GetGroups().end(); it++) {
            if (IsOverlapping(*it))
                groups_overlapping.push_back(it);
        }
    }

    ShipGroup::ShipGroup(Engine &engine_, Planet &sender, Planet &dest, int ship_count) :
        sender_planet(sender), dest_planet(dest), engine(engine_)
    {
        owner = sender.GetOwner();
        tfloat r = sender.GetRadius();
        Vector C = sender.GetPos();
        Vector goal = dest.GetPos() - C;
        tfloat delta = std::min<tfloat>(M_PI / ship_count, 0.4);
        tfloat alpha = atan2(goal.y, goal.x) - delta*(ship_count / 2);
        for (int i = 0; i < ship_count; i++) {
            tfloat a = alpha + delta*i;
            ships.emplace_back(*this, C.x + r*cos(a), C.y + r*sin(a));
        }
    }

    void ShipGroup::StepPrepare() {
        sum_pos = sum_speed = Vector(0, 0);
        top = bot = ships.begin()->GetY();
        left = right = ships.begin()->GetX();
        for (auto& ship : ships) {
            sum_pos += ship.GetPos();
            sum_speed += ship.GetSpeed();
            top = std::min(top, ship.GetY());
            bot = std::max(bot, ship.GetY());
            left  = std::min(left, ship.GetX());
            right = std::max(right, ship.GetX());
        }
    }

    
    void ShipGroup::Step() {
        FindOverlappingGroups();
        auto it = ships.begin();
        while (it != ships.end()) {
            it->Step();
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


}