#pragma once
#include <cmath>

constexpr double PI = 3.141592657;
double clamp(double x)
{
    return x < 0 ? 0 : (x > 1 ? 1 : x);
}
int GammaEncoding(double x)
{
    return static_cast<int>(pow(clamp(x), 1 / 2.2) * 255 + .5);
}

template<typename T>
class Vector2
{
public:
    T x, y;
    Vector2(T x_ = 0, T y_ = 0)
    {
        x = x_;
        y = y_;
    }
};

using Vector2i = Vector2<int>;

class Vector3
{
public:
    double x, y, z;
    Vector3(double x_ = 0, double y_ = 0, double z_ = 0)
    {
        x = x_;
        y = y_;
        z = z_;
    }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }
    Vector3 operator+(const Vector3& b) const { return Vector3(x + b.x, y + b.y, z + b.z); }
    Vector3 operator+=(const Vector3& b) const { return *this + b; }
    Vector3 operator-(const Vector3& b) const { return Vector3(x - b.x, y - b.y, z - b.z); }
    Vector3 operator*(double b) const { return Vector3(x * b, y * b, z * b); }
    Vector3 operator/(double b) const { return Vector3(x / b, y / b, z / b); }
    // only for Color
    Vector3 operator*(const Vector3& b) const { return Vector3(x * b.x, y * b.y, z * b.z); }
    Vector3& Normalize() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    double Dot(const Vector3& b) const { return x * b.x + y * b.y + z * b.z; }
    Vector3 Cross(Vector3& b) const { return Vector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
    friend Vector3 operator*(double b, Vector3 v) { return v * b; }
    friend double Dot(const Vector3& a, const Vector3& b) { return a.Dot(b); }
};

typedef Vector3 point3;
typedef Vector3 color;
