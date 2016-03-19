#pragma once

#include <cmath>

template <typename tfloat>
struct Vector2 {
    tfloat x, y;
    Vector2() : x(0), y(0) {};
    Vector2(tfloat X, tfloat Y) : x(X), y(Y) {};

    tfloat GetLength() { return hypot(x, y); }

    Vector2 operator + (const Vector2 &p) const { return Vector2(x + p.x, y + p.y); }
    Vector2 operator - (const Vector2 &p) const { return Vector2(x - p.x, y - p.y); }
    Vector2 operator * (tfloat k) const { return Vector2(x * k, y * k); }
    Vector2 operator / (tfloat k) const { return Vector2(x / k, y / k); }
    Vector2 operator - () const { return Vector2(-x, -y); }

    Vector2 operator += (const Vector2 &p) {
        x += p.x; y += p.y;
        return *this;
    }
    Vector2 operator -= (const Vector2 &p) {
        x -= p.x; y -= p.y;
        return *this;
    }
    Vector2 operator *= (tfloat k) {
        x *= k; y *= k;
        return *this;
    }
    Vector2 operator /= (tfloat k) {
        x /= k; y /= k;
        return *this;
    }

    void SetLength(tfloat len) {
        if (x == 0 && y == 0)
            return;
        tfloat k = len / GetLength();
        x *= k; y *= k;
    }
};