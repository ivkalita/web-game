#include "Map.hpp"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include <fstream>

static const std::string sPlayersCount = "playersCount";
static const std::string sWidth = "width";
static const std::string sHeight = "height";
static const std::string sPlanets = "planets";

static const std::string sPlanetX = "x";
static const std::string sPlanetY = "y";
static const std::string sPlanetRadius = "radius";
static const std::string sPlanetOwner = "owner";
static const std::string sPlanetShipscount = "shipsCount";
static const std::string sPlanetLimit = "limit";
static const std::string sPlanetProduction = "production";


void Map::ReadMap(Poco::Dynamic::Var &_file) {
    Poco::JSON::Object::Ptr _map, _planet;
    Poco::JSON::Array::Ptr _planets;
    Poco::Dynamic::Var var;

    _map = _file.extract<Poco::JSON::Object::Ptr>();

    map.players_count = _map->get(sPlayersCount).extract<int>();
    map.width = _map->get(sWidth).extract<int>();
    map.heigth = _map->get(sHeight).extract<int>();

    _planets = _map->get(sPlanets).extract<Poco::JSON::Array::Ptr>();
    map.planets.clear();
    for (int i = 0; i < _planets->size(); i++) {
        _planet = _planets->getObject(i);
        PlanetInfo p;
        p.x = _planet->get(sPlanetX).convert<int>();
        p.y = _planet->get(sPlanetY).convert<int>();
        p.radius = _planet->get(sPlanetRadius).convert<int>();
        p.owner = _planet->get(sPlanetOwner).convert<int>();
        p.ships_count = _planet->get(sPlanetShipscount).convert<int>();

        var = _planet->get(sPlanetLimit);
        p.limit = var.isEmpty() ? -1 : var.extract<int>();
        var = _planet->get(sPlanetProduction);
        p.production = var.isEmpty() ? -1 : var.extract<double>();

        map.planets.push_back(p);
    }
}

void Map::ReadFromString(const std::string &map_json) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var _file;
    _file = parser.parse(map_json);
    ReadMap(_file);
}

void Map::ReadFromFile(const std::string &filename) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var _file;
    std::ifstream in(filename);
    _file = parser.parse(in);
    ReadMap(_file);
}