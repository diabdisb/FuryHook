#pragma once
#include "../Overlay/overlay.hpp"

#define pow(n) (n)*(n)
class Vector2
{
public:
    float x, y;

    __forceinline Vector2()
    {
        x = y = 0.0f;
    }

    __forceinline Vector2(float X, float Y)
    {
        x = X; y = Y;
    }


    float distance(Vector2 b)
    {
        const float dx = b.x - x;
        const float dy = b.y - y;
        return sqrt(dx * dx + dy * dy);
    }

    __forceinline Vector2 operator+(float v) const
    {
        return Vector2(x + v, y + v);
    }

    __forceinline Vector2 operator-(float v) const
    {
        return Vector2(x - v, y - v);
    }

    __forceinline Vector2& operator+=(float v)
    {
        x += v; y += v; return *this;
    }

    __forceinline Vector2& operator*=(float v)
    {
        x *= v; y *= v; return *this;
    }

    __forceinline Vector2& operator/=(float v)
    {
        x /= v; y /= v; return *this;
    }

    __forceinline Vector2 operator-(const Vector2& v) const
    {
        return Vector2(x - v.x, y - v.y);
    }

    __forceinline Vector2 operator+(const Vector2& v) const
    {
        return Vector2(x + v.x, y + v.y);
    }

    __forceinline Vector2& operator+=(const Vector2& v)
    {
        x += v.x; y += v.y; return *this;
    }

    __forceinline Vector2& operator-=(const Vector2& v) {
        x -= v.x; y -= v.y; return *this;
    }

    __forceinline Vector2 operator/(float v) const {
        return Vector2(x / v, y / v);
    }

    __forceinline Vector2 operator*(float v) const {
        return Vector2(x * v, y * v);
    }

    __forceinline Vector2 operator/(const Vector2& v) const {
        return Vector2(x / v.x, y / v.y);
    }

    __forceinline bool Zero() const {
        return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f);
    }

    __forceinline ImVec2 ToImVec2() const {
        return ImVec2(x, y);
    }

};

class Vector3
{
public:

    static float sqrtf(float number)
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y = number;
        i = *(long*)&y;
        i = 0x5f3759df - (i >> 1);
        y = *(float*)&i;
        y = y * (threehalfs - (x2 * y * y));
        y = y * (threehalfs - (x2 * y * y));

        return 1 / y;
    }

    Vector3()
    {
        x = y = z = 0.f;
    }

    Vector3(float fx, float fy, float fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }

    float operator[](int i) const {
        return ((float*)this)[i];
    }

    inline float Length() const
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    float  x, y, z;

    Vector3 operator+(const Vector3& input) const
    {
        return Vector3{ x + input.x, y + input.y, z + input.z };
    }

    Vector3 operator-(const Vector3& input) const
    {
        return Vector3{ x - input.x, y - input.y, z - input.z };
    }

    Vector3 operator/(float input) const
    {
        return Vector3{ x / input, y / input, z / input };
    }

    Vector3 operator*(float input) const
    {
        return Vector3{ x * input, y * input, z * input };
    }

    Vector3& operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    Vector3& operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    Vector3& operator/=(float input)
    {
        x /= input;
        y /= input;
        z /= input;
        return *this;
    }

    Vector3& operator*=(float input)
    {
        x *= input;
        y *= input;
        z *= input;
        return *this;
    }

    Vector3 midPoint(Vector3 v2)
    {
        return Vector3((x + v2.x) / 2, (y + v2.y) / 2, (z + v2.z) / 2);
    }

    bool operator==(const Vector3& input) const
    {
        return x == input.x && y == input.y && z == input.z;
    }


    float distance(Vector3 vec)
    {
        return Vector3::sqrtf(pow(vec.x - x) + pow(vec.y - y) + pow(vec.z - z));
    }

    float clamp0to1(float value)
    {
        float result;
        if (value < 0)
        {
            result = 0;
        }
        else if (value > 1.f)
        {
            result = 1.f;
        }
        else
        {
            result = value;
        }
        return result;
    }

    float Lerp()
    {
        return x + (y - x) * clamp0to1(z);
    }

    inline  float length_sqr() const
    {
        return (x * x) + (y * y) + (z * z);
    }

    inline float length() const
    {
        return (float)sqrt(length_sqr());
    }

    inline float length_2d() const
    {
        return (float)sqrt((x * x) + (y * y));
    }

    inline float length() { return sqrtf((x * x) + (y * y) + (z * z)); }

    Vector3 normalized()
    {
        float len = length();
        return Vector3(x / len, y / len, z / len);
    }

    __forceinline bool Zero() const {
        return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f && z > -0.1f && z < 0.1f);
    }

    Vector3 cross(Vector3 rhs)
    {
        return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }

    float Dot(const Vector3& Vec2) {
        return x * Vec2[0] + y * Vec2[1] + z * Vec2[2];
    }

    float dot(Vector3 input) const
    {
        return (x * input.x) + (y * input.y) + (z * input.z);
    }


    bool empty() { return x == 0.000000 && y == 0.000000 && z == 0.000000; }

    bool is_valid() const
    {
        return !(x == 0.f && y == 0.f && z == 0.f) || (x == -1.f && y == -1.f && z == -1.f);
    }

};

class Vector4
{
public:
    Vector4()
    {
        x = y = z = w = 0.f;
    }

    Vector4(float fx, float fy, float fz, float fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;
    }

    float x, y, z, w;

    bool empty() { return x == 0.000000 && y == 0.000000 && z == 0.000000 && w == 0.000000; }
};

inline Vector2 cos_tan_horizontal(float angle, float range, float x, float y, int length) {
    float our_angle = (angle + 45.f);

    float yaw = our_angle * (3.14159265358979323846 / 180.0);

    float view_cosinus = cos(yaw);
    float view_sinus = sin(yaw);

    float x2 = range * (-view_cosinus) + range * view_sinus;
    float y2 = range * (-view_cosinus) - range * view_sinus;

    int screen_x = x + static_cast<int>(x2 / range * length);
    int screen_y = y + static_cast<int>(y2 / range * length);

    return Vector2(screen_x, screen_y);
}



static bool Normalize(float& Yaw, float& Pitch) {
    if (Pitch < -89)
        return false;

    else if (Pitch > 89)
        return false;

    if (Yaw < -360)
        return false;

    else if (Yaw > 360)
        return false;

    return true;
}
static float to_degree(float radian)
{
    return radian * 180.f / 3.141592f;
}
inline const float Length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
static Vector3 CalcAngle(Vector3 LocalPos, Vector3 EnemyPos)
{
    Vector3 dir;

    dir.x = LocalPos.x - EnemyPos.x;
    dir.y = LocalPos.y - EnemyPos.y;
    dir.z = LocalPos.z - EnemyPos.z;

    float Pitch = to_degree(asin(dir.y / Length(dir)));
    float Yaw = to_degree(-atan2(dir.x, -dir.z));


    Vector3 Return;
    Return.x = Pitch;
    Return.y = Yaw;


    return Return;
}
inline const double ToRad(double degree)
{
    double pi = 3.14159265359;
    return (degree * (pi / 180));
}
inline const double ToDeg(double radians) {
    return radians * (180.0 / 3.14);
}
inline const Vector4 ToQuat(Vector3 Euler) {
    float c1 = cos(ToRad(Euler.x) / 2);
    float s1 = sin(ToRad(Euler.x) / 2);
    float c2 = cos(ToRad(Euler.y) / 2);
    float s2 = sin(ToRad(Euler.y) / 2);
    float c3 = cos(ToRad(Euler.z) / 2);
    float s3 = sin(ToRad(Euler.z) / 2);
    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;
    float c1s2 = c1 * s2;
    float s1c2 = s1 * c2;
    Vector4 Quat = {
        s1c2 * c3 + c1s2 * s3,
        c1s2 * c3 - s1c2 * s3,
        c1c2 * s3 - s1s2 * c3,
        c1c2 * c3 + s1s2 * s3
    };
    return Quat;
}

struct ViewMatrix
{
public:
    float matrix[4][4];
};



inline Vector2 WorldToScreen(const Vector3& position, const ViewMatrix& viewMatrix) {
    float m00 = viewMatrix.matrix[0][0], m01 = viewMatrix.matrix[0][1], m02 = viewMatrix.matrix[0][2], m03 = viewMatrix.matrix[0][3];
    float m10 = viewMatrix.matrix[1][0], m11 = viewMatrix.matrix[1][1], m12 = viewMatrix.matrix[1][2], m13 = viewMatrix.matrix[1][3];
    float m20 = viewMatrix.matrix[2][0], m21 = viewMatrix.matrix[2][1], m22 = viewMatrix.matrix[2][2], m23 = viewMatrix.matrix[2][3];
    float m30 = viewMatrix.matrix[3][0], m31 = viewMatrix.matrix[3][1], m32 = viewMatrix.matrix[3][2], m33 = viewMatrix.matrix[3][3];

    float w = m03 * position.x + m13 * position.y + m23 * position.z + m33;

    if (w < 0.1f) // Entity is behind camera
        return Vector2(-1, -1);

    float invW = 1.0f / w;

    float x = m00 * position.x + m10 * position.y + m20 * position.z + m30;
    float y = m01 * position.x + m11 * position.y + m21 * position.z + m31;

    float screenX = (Width / 2.0f) * (1.0f + x * invW);
    float screenY = (Height / 2.0f) * (1.0f - y * invW);

    return Vector2(screenX, screenY);
}

//std::string FilterDroppedItem(std::string input)
//{
//    size_t start = input.find(_memdup(skCrypt(" (")));
//    size_t end = input.find(_memdup(skCrypt(")")));
//    if (start != std::string::npos && end != std::string::npos)
//        input.erase(start, end - start + 1);
//    bool skip_item = false;
//    std::string skip_keywords[] = { _memdup(skCrypt("torch")), _memdup(skCrypt("rock")), _memdup(skCrypt("asset")), _memdup(skCrypt("fire")), _memdup(skCrypt("dung")), _memdup(skCrypt("arrow")), _memdup(skCrypt("nail")), _memdup(skCrypt("movepoint")), _memdup(skCrypt("-")), _memdup(skCrypt("_")) };
//    for (const std::string& keyword : skip_keywords) {
//        if (input.find(keyword) != std::string::npos) {
//            skip_item = true;
//            break;
//        }
//    }
//    if (skip_item)
//        return (std::string)skCrypt("");
//
//    return input;
//}
//
//float AimFovv(bool WORLD, Vector3 ScreenPos)
//{
//    if (WORLD) {
//        Vector2 center(globals::ScreenMidSize.right, globals::ScreenMidSize.bottom);
//        return center.distance({ ScreenPos.x, ScreenPos.y });
//    }
//    else {
//        return 1000.f;
//    }
//}
