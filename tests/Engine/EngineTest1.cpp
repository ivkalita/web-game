#include <gtest/gtest.h>
#include "GameEngine.hpp"

using namespace GameEngine;

TEST(TestEngine, SendToOwnPlanet1) {
    Engine e;
    auto& s = e.AddPlanet(0, 0, 10, 10, 1);
    auto& d = e.AddPlanet(40, 40, 10, 10, 1);
    e.Launch(1, s, d);
    while (e.ActiveShipsCount() > 0)
        e.Step();
    EXPECT_EQ(9, s.ShipCount());
    EXPECT_EQ(11, d.ShipCount());
}

TEST(TestEngine, SendToOwnPlanet2) {
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

TEST(TestEngine, SendToOwnPlanet3) {
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