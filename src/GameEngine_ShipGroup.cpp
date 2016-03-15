#define _USE_MATH_DEFINES 

#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"
#include <algorithm>

namespace GameEngine {

    ShipGroup::ShipGroup(PlanetList &planet_list, Planet &sender, Planet &dest, int ship_count) :
        sender_planet(sender), dest_planet(dest), planets(planet_list)
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

    void ShipGroup::Step() {
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