#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

    void Engine::InitMap(Map map_) {
        for (auto &p : map_.GetPlanets()) {
            AddPlanet(p.x, p.y, p.radius, p.ships_count, p.owner);
        }
    }

    void Engine::Step() {
        for (auto& g : groups) {
            g.StepPrepare();
        }

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
#if _DEBUG
            const int MAX_X = 10000;
            const int MAX_Y = 10000;
            for (auto& g : groups) for (auto i : g.GetShips()) {
                for (auto& p : planets) {
                    if (p.IsInside(i.GetPos()))
                        throw Poco::Exception("Ship is inside a planet\nShip info: "
                            + i.GetInfo() + "\nPlanet info: " + p.GetInfo() + "\n");
                }
                if (abs(i.GetX()) > MAX_X || abs(i.GetY()) > MAX_Y)
                    throw Poco::Exception("Ship flew away\nShip info: " + i.GetInfo());
            }
#endif _DEBUG
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
        groups.emplace_back(*this, sender_planet, dest_planet, count);
    }

    int Engine::ActiveShipsCount() const {
        int count = 0;
        for (auto& group : groups)
            count += group.Size();
        return count;
    }

}
