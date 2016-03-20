#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "Poco/JSON/Object.h"
#include "GameEngine.hpp"

class MapReader {
private:
    struct PlanetInfo {
        int x, y;
        int radius;
        int owner;
        int ships_count;
        std::string GetInfo();
    };
    struct MapInfo {
        int players_count;
        int width, heigth;
        std::vector<PlanetInfo> planets;
    };
    MapInfo map;
    void ReadPlanet(Poco::JSON::Object::Ptr &_planet);
    void Check();
public:
    void ReadMap(std::string &filename);
    void InitEngine(GameEngine::Engine &engine);
    void ReadAndInit(std::string &filename, GameEngine::Engine &engine);
};
