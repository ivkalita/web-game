#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    ShipGroup::ShipGroup(PlanetList &planet_list, Planet &sender, Planet &dest, int ship_count) :
        sender_planet(sender), dest_planet(dest), planets(planet_list)
    {
        owner = sender.GetOwner();
        tfloat r = sender.GetRadius();
        Vector C = sender.GetPos();
        for (int i = 0; i < ship_count; i++) {
            ships.emplace_back(*this, C.x + r, C.y);
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