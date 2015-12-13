#include <gtest/gtest.h>
#include "GameEngine.hpp"
#include <vector>
#include <functional>

using namespace GameEngine;

TEST(TestEngine, SendToOwnPlanet1Ship) {
    Engine e;
    auto& s = e.AddPlanet(0, 0, 10, 10, 1);
    auto& d = e.AddPlanet(40, 40, 10, 10, 1);
    e.Launch(1, s, d);
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(9, s.ShipCount());
    EXPECT_EQ(11, d.ShipCount());
}

TEST(TestEngine, SendToOwnPlanet2Ships) {
    Engine e;
    auto& s = e.AddPlanet(0, 0, 10, 10, 1);
    auto& d = e.AddPlanet(40, 40, 10, 10, 1);
    e.Launch(1, s, d);
    e.Step();
    e.Launch(1, s, d);
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(8, s.ShipCount());
    EXPECT_EQ(12, d.ShipCount());
}

TEST(TestEngine, SendToOwnPlanetEachOther) {
    Engine e;
    auto& s = e.AddPlanet(0, 0, 10, 10, 1);
    auto& d = e.AddPlanet(40, 40, 10, 10, 1);
    e.Launch(1, d, s);
    e.Launch(1, s, d);
    EXPECT_EQ(9, d.ShipCount());
    EXPECT_EQ(9, s.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(10, s.ShipCount());
    EXPECT_EQ(10, d.ShipCount());
}

TEST(TestEngine, SendFromOwnPlanetsEachOtherWithObstacle) {
    Engine e;
    auto& s = e.AddPlanet(-50, -15.2, 10, 10, 1);
    auto& d = e.AddPlanet(21, 40, 10, 10, 1);
    e.AddPlanet(-20, 0, 20, 3, 1);
    e.Launch(1, d, s);
    e.Launch(1, s, d);
    EXPECT_EQ(9, d.ShipCount());
    EXPECT_EQ(9, s.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(10, s.ShipCount());
    EXPECT_EQ(10, d.ShipCount());
}

TEST(TestEngine, SendToOwnPlanetFromTwoOwnPlanets) {
    Engine e;
    auto& s1 = e.AddPlanet(-50, -64, 20, 16, 1);
    auto& s2 = e.AddPlanet(100, 40, 12, 28, 1);
    auto& d = e.AddPlanet(0, -15, 20, 3, 1);
    e.Launch(5, s1, d);
    e.Launch(5, s2, d);
    EXPECT_EQ(11, s1.ShipCount());
    EXPECT_EQ(23, s2.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(13, d.ShipCount());
}

TEST(TestEngine, SendToEnemyPlanetLessThanItHas) {
    Engine e;
    auto& s = e.AddPlanet(50, -21, 10, 1, 1);
    auto& d = e.AddPlanet(21, 40, 10, 4, 2);
    e.Launch(1, s, d);
    EXPECT_EQ(0, s.ShipCount());
    EXPECT_EQ(4, d.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(0, s.ShipCount());
    EXPECT_EQ(3, d.ShipCount());
    EXPECT_EQ(2, d.GetOwner());
}

TEST(TestEngine, SendToEnemyPlanetMoreThanItHas) {
    Engine e;
    auto& s = e.AddPlanet(50, -21, 10, 51, 1);
    auto& d = e.AddPlanet(21, 40, 10, 4, 2);
    e.Launch(31, s, d);
    EXPECT_EQ(20, s.ShipCount());
    EXPECT_EQ(4, d.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(20, s.ShipCount());
    EXPECT_EQ(27, d.ShipCount());
    EXPECT_EQ(1, d.GetOwner());
}

TEST(TestEngine, SendToOwnPlanetWithManyObstacles) {
    Engine e;
    int radius = 7;
    auto& s = e.AddPlanet(-100, -200, 10, 1, 1);
    auto& d = e.AddPlanet(200, 315, 10, 15, 1);
    for (tfloat i = -80; i < 180; i += 2 * (radius + Planet::CLOSE_RANGE))
        for (tfloat j = -180; j < 290; j += 2 * (radius + Planet::CLOSE_RANGE))
            e.AddPlanet(i, j, radius, 1, 2);

    e.Launch(1, s, d);
    EXPECT_EQ(0, s.ShipCount());
    EXPECT_EQ(15, d.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(0, s.ShipCount());
    EXPECT_EQ(16, d.ShipCount());
}

TEST(TestEngine, SendEachOtherWithEnemy) {
    Engine e;
    auto& s = e.AddPlanet(19, -25, 15, 31, 1);
    auto& d = e.AddPlanet(42, 17, 8, 22, 2);
    e.Launch(20, s, d);
    e.Launch(21, d, s);
    EXPECT_EQ(11, s.ShipCount());
    EXPECT_EQ(1, d.ShipCount());
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(10, s.ShipCount());
    EXPECT_EQ(2, s.GetOwner());
    EXPECT_EQ(19, d.ShipCount());
    EXPECT_EQ(1, d.GetOwner());
}

TEST(TestEngine, SendToEnemyFromMany) {
    Engine e;
    tfloat radius = 6;
    std::vector<std::reference_wrapper<Planet> > s;
    for (tfloat j = -100; j < 100; j += 2 * (radius + Planet::CLOSE_RANGE))
        s.push_back(e.AddPlanet(-20, j, radius, 8, 1));

    auto& d = e.AddPlanet(21, 40, 10, 4, 2);
    
    for (Planet& p : s)
        e.Launch(5, p, d);

    while (e.ActiveShipsCount() > 0)
        e.Step();

    EXPECT_EQ(5 * s.size() - 4, d.ShipCount());
    EXPECT_EQ(1, d.GetOwner());
}