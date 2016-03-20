#include "MapReader.hpp"
#include "Poco/JSON/Parser.h"
#include "GameEngine.hpp"
#include <sstream>
#include <cmath>

static const std::string sPlayersCount = "playersCount";
static const std::string sWidth = "width";
static const std::string sHeigth = "heigth";
static const std::string sPlanets = "planets";

static const std::string sPlanetX = "x";
static const std::string sPlanetY = "y";
static const std::string sPlanetRadius = "radius";
static const std::string sPlanetOwner = "owner";
static const std::string sPlanetShipscount = "shipsCount";

std::string MapReader::PlanetInfo::GetInfo() {
    std::stringstream s;
    s << "x=" << x << " y=" << y
        << " radius=" << radius
        << " owner=" << owner
        << " ships_count=" << ships_count;
    return s.str();
}

void MapReader::ReadPlanet(Poco::JSON::Object::Ptr &_planet) {
    PlanetInfo p;
    p.x = _planet->get(sPlanetX).convert<int>();
    p.y = _planet->get(sPlanetY).convert<int>();
    p.radius = _planet->get(sPlanetRadius).convert<int>();
    p.owner = _planet->get(sPlanetOwner).convert<int>();
    p.ships_count = _planet->get(sPlanetShipscount).convert<int>();
    map.planets.push_back(p);
}

void MapReader::Check() {
    for (int i = 0; i < map.planets.size(); i++) {
        PlanetInfo p = map.planets[i];
        if (p.owner < 0 || p.owner > map.players_count)
            throw Poco::Exception("Bad owner for planet: " + p.GetInfo());
        if (p.ships_count < 0)
            throw Poco::Exception("Negative ships count for planet: " + p.GetInfo());
        if (p.x - p.radius < 0 || p.x + p.radius > map.width 
            || p.y - p.radius < 0 || p.y + p.radius > map.heigth)
            throw Poco::Exception("Planet gets out of game bounds: " + p.GetInfo());
        if (p.radius <= 0)
            throw Poco::Exception("Bad radius for planet: " + p.GetInfo());

        for (int j = i + 1; j < map.planets.size(); j++) {
            PlanetInfo q = map.planets[j];
            double d = hypot(p.x - q.x, p.y - q.y) - p.radius - q.radius;
            if (d < 2 * GameEngine::Planet::CLOSE_RANGE)
                throw Poco::Exception("Planets are too close: " + p.GetInfo() + "; " + q.GetInfo());
        }
    }
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
    map.width = _map->get(sWidth).extract<int>();
    map.heigth = _map->get(sHeigth).extract<int>();

    _planets = _map->get(sPlanets).extract<Poco::JSON::Array::Ptr>();
    for (int i = 0; i < _planets->size(); i++) {
        _planet = _planets->getObject(i);
        ReadPlanet(_planet);
    }
    Check();
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
