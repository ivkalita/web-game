#pragma once

#include <vector>
#include <string>
#include "Poco/JSON/Parser.h"

class Map {
private:
    struct PlanetInfo {
        int x, y;
        int radius;
        int owner;
        int ships_count;
        int limit;
        double production;
    };
    struct MapInfo {
        int players_count;
        int width, heigth;
        std::vector<PlanetInfo> planets;
    };
    MapInfo map;
    void ReadMap(Poco::Dynamic::Var &_file);
public:
    Map() {}
    void ReadFromString(const std::string &map_json);
    void ReadFromFile(const std::string &filename);
    int GetPlayersCount() const { return map.players_count; }
    int GetWidth() const { return map.width; }
    int GetHeight() const { return map.heigth; }
    const std::vector<PlanetInfo>& GetPlanets() const { return map.planets; }
};