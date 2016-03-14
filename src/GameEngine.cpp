#include "GameEngine.hpp"

#include <sstream>
#include "Poco/Exception.h"

namespace GameEngine {

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
