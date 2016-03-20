#include "MapReader.hpp"
#include "Poco/JSON/Parser.h"
#include "GameEngine.hpp"

static const std::string sPlayersCount = "playersCount";
static const std::string sWidth = "width";
static const std::string sHeigth = "heigth";
static const std::string sPlanets = "planets";

static const std::string sPlanetX = "x";
static const std::string sPlanetY = "y";
static const std::string sPlanetRadius = "radius";
static const std::string sPlanetOwner = "owner";
static const std::string sPlanetShipscount = "shipsCount";

void MapReader::ReadPlanet(Poco::JSON::Object::Ptr &_planet) {
    PlanetInfo p;
    p.x = _planet->get(sPlanetX).convert<int>();
    p.y = _planet->get(sPlanetY).convert<int>();
    p.radius = _planet->get(sPlanetRadius).convert<int>();
    p.owner = _planet->get(sPlanetOwner).convert<int>();
    p.ships_count = _planet->get(sPlanetShipscount).convert<int>();
    map.planets.push_back(p);
}

void MapReader::ReadMap(std::string &filename) {
    Poco::JSON::Object::Ptr _map, _planet;
    Poco::JSON::Array::Ptr _planets;
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var _file;

    std::ifstream in(filename);
    _file = parser.parse(in);
    _map = _file.extract<Poco::JSON::Object::Ptr>();

    map.players_count = _map->get(sPlayersCount).extract<int>();

    _planets = _map->get(sPlanets).extract<Poco::JSON::Array::Ptr>();
    for (int i = 0; i < _planets->size(); i++) {
        _planet = _planets->getObject(i);
        ReadPlanet(_planet);
    }
}

void MapReader::InitEngine(GameEngine::Engine &engine) {
    for (auto &p : map.planets) {
        engine.AddPlanet(p.x, p.y, p.radius, p.ships_count, p.owner);
    }
}

void MapReader::ReadAndInit(std::string &filename, GameEngine::Engine &engine) {
    ReadMap(filename);
    InitEngine(engine);
}
