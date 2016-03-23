#include "GameEngine.hpp"

#include <sstream>

namespace GameEngine {

    const tfloat Ship::speed_length = 1.0;
    const tfloat Ship::CLOSE_RANGE = 20;

    static bool IsHypotLessThen(tfloat dx, tfloat dy, tfloat val) {
        return dx*dx + dy*dy < val*val;
    }

    static bool IsHypotLessThen(Vector v1, Vector v2, tfloat val) {
        return IsHypotLessThen(v1.x - v2.x, v1.y - v2.y, val);
    }

    static const tfloat l1 = 0.85;
    static const tfloat w2 = 1.0 / 60;
    static const tfloat w3 = 1.0 / 18;
    static const tfloat w4 = 1.0 / 12;
    static const tfloat w5 = 1.0 / 15;

    // Using Boids algorithm to calculate speed of ship
    // Move to DestPlanet
    Vector Ship::BoidsRule1() {
        Vector v = group.GetDestPlanet().GetPos() - GetPos();
        v.SetLength(l1);
        return v;
    }

    // Move to MassCenter of group
    Vector Ship::BoidsRule2() {
        if (group.Size() <= 1)
            return Vector();
        Vector v = group.GetSumOfPos() - pos;
        v /= group.Size() - 1;
        v -= pos;
        return v * w2;
    }

    // Average speed of all other ships from group
    Vector Ship::BoidsRule3() {
        if (group.Size() <= 1)
            return Vector();
        Vector v = group.GetSumOfSpeed() - speed;
        v /= group.Size() - 1;
        v -= speed;
        return v * w3;
    }

    // Avoid collision with other planets
    Vector Ship::BoidsRule4() {
        const tfloat A = 1.5;
        const tfloat B = 0;
        const tfloat D = 2;
        for (auto& p : group.GetPlanets()) {
            if (p == group.GetDestPlanet() || !p.IsNear(pos))
                continue;
            Vector v = pos - p.GetPos();
            Vector t = v;
            t.SetLength(speed_length + D);
            if (p.IsInside(pos - t))
                return v * 10000;
            v.SetLength(p.GetRadius() + Planet::CLOSE_RANGE*A + B - v.GetLength());
            return v * w4;
        }
        return Vector();
    }

    // Avoid collision with other ships form group
    Vector Ship::BoidsRule5() {
        const tfloat A = 1.2;
        const tfloat B = 4;
        Vector v;
        for (auto& it : group.GetOverlappingGroups()) {
            for (auto& s : it->GetShips()) {
                if (!IsHypotLessThen(pos, s.pos, CLOSE_RANGE))
                    continue;
                Vector t = pos - s.pos;
                t.SetLength(CLOSE_RANGE*A + B - t.GetLength());
                v += t;
            }
        }
        return v*w5;
    }

    void Ship::Aim() {
        Vector v;
        v += BoidsRule1();
        v += BoidsRule2();
        v += BoidsRule3();
        v += BoidsRule4();
        v += BoidsRule5();
        speed.SetLength(1);
        speed += v;
        speed.SetLength(speed_length);
    }

    Ship::Ship(ShipGroup &group_, tfloat X, tfloat Y) :
        group(group_), finished(false), pos(Vector(X, Y)) {}

    void Ship::Step() {
        Aim();
        pos += speed;
        finished = group.GetDestPlanet().IsInside(pos);
    }

    std::string Ship::GetInfo() const {
        std::stringstream s;
        s << " X: " << pos.x << " Y: " << pos.y
            << " VX: " << speed.x << " VY: " << speed.y
            << " Sender: " << group.GetSenderPlanet().GetInfo()
            << " Dest: " << group.GetDestPlanet().GetInfo()
            << " owner: " << group.GetOwner()
            << " finished: " << finished;
        return s.str();
    }

}