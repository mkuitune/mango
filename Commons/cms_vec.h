#pragma once
#include <iostream>
namespace cms {
struct Vec3f {
float x, y, z;
Vec3f operator+(const float& s) const {return {x + s, y + s, z + s};}
Vec3f operator-(const float& s) const {return {x - s, y - s, z - s};}
Vec3f operator*(const float& s) const {return {x * s, y * s, z * s};}
Vec3f operator/(const float& s) const {return {x / s, y / s, z / s};}
Vec3f& operator+=(const Vec3f& s){x += s.x; y += s.y; z += s.z; return *this;}
Vec3f& operator-=(const Vec3f& s){x -= s.x; y -= s.y; z -= s.z; return *this;}
Vec3f& operator*=(const Vec3f& s){x *= s.x; y *= s.y; z *= s.z; return *this;}
Vec3f& operator/=(const Vec3f& s){x /= s.x; y /= s.y; z /= s.z; return *this;}
Vec3f& operator+=(const float& s){x += s; y += s; z += s; return *this;}
Vec3f& operator-=(const float& s){x -= s; y -= s; z -= s; return *this;}
Vec3f& operator*=(const float& s){x *= s; y *= s; z *= s; return *this;}
Vec3f& operator/=(const float& s){x /= s; y /= s; z /= s; return *this;}
Vec3f operator+(const Vec3f& s) const {return {x + s.x, y + s.y, z + s.z};}
Vec3f operator-(const Vec3f& s) const {return {x - s.x, y - s.y, z - s.z};}
template <typename T, typename Traits>friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Vec3f& v){
return out << v.x << ", " << v.y << ", " << v.z;};
};

struct Vec3 {
double x, y, z;
Vec3 operator+(const double& s) const {return {x + s, y + s, z + s};}
Vec3 operator-(const double& s) const {return {x - s, y - s, z - s};}
Vec3 operator*(const double& s) const {return {x * s, y * s, z * s};}
Vec3 operator/(const double& s) const {return {x / s, y / s, z / s};}
Vec3& operator+=(const Vec3& s){x += s.x; y += s.y; z += s.z; return *this;}
Vec3& operator-=(const Vec3& s){x -= s.x; y -= s.y; z -= s.z; return *this;}
Vec3& operator*=(const Vec3& s){x *= s.x; y *= s.y; z *= s.z; return *this;}
Vec3& operator/=(const Vec3& s){x /= s.x; y /= s.y; z /= s.z; return *this;}
Vec3& operator+=(const double& s){x += s; y += s; z += s; return *this;}
Vec3& operator-=(const double& s){x -= s; y -= s; z -= s; return *this;}
Vec3& operator*=(const double& s){x *= s; y *= s; z *= s; return *this;}
Vec3& operator/=(const double& s){x /= s; y /= s; z /= s; return *this;}
Vec3 operator+(const Vec3& s) const {return {x + s.x, y + s.y, z + s.z};}
Vec3 operator-(const Vec3& s) const {return {x - s.x, y - s.y, z - s.z};}
template <typename T, typename Traits>friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Vec3& v){
return out << v.x << ", " << v.y << ", " << v.z;};
};

struct Vec4f {
float x, y, z, w;
Vec4f operator+(const float& s) const {return {x + s, y + s, z + s, w + s};}
Vec4f operator-(const float& s) const {return {x - s, y - s, z - s, w - s};}
Vec4f operator*(const float& s) const {return {x * s, y * s, z * s, w * s};}
Vec4f operator/(const float& s) const {return {x / s, y / s, z / s, w / s};}
Vec4f& operator+=(const Vec4f& s){x += s.x; y += s.y; z += s.z; w += s.w; return *this;}
Vec4f& operator-=(const Vec4f& s){x -= s.x; y -= s.y; z -= s.z; w -= s.w; return *this;}
Vec4f& operator*=(const Vec4f& s){x *= s.x; y *= s.y; z *= s.z; w *= s.w; return *this;}
Vec4f& operator/=(const Vec4f& s){x /= s.x; y /= s.y; z /= s.z; w /= s.w; return *this;}
Vec4f& operator+=(const float& s){x += s; y += s; z += s; w += s;return *this;}
Vec4f& operator-=(const float& s){x -= s; y -= s; z -= s; w -= s;return *this;}
Vec4f& operator*=(const float& s){x *= s; y *= s; z *= s; w *= s;return *this;}
Vec4f& operator/=(const float& s){x /= s; y /= s; z /= s; w /= s;return *this;}
Vec4f operator+(const Vec4f& s) const {return {x + s.x, y + s.y, z + s.z, w + s.w};}
Vec4f operator-(const Vec4f& s) const {return {x - s.x, y - s.y, z - s.z, w - s.w};}
template <typename T, typename Traits>friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Vec4f& v){
return out << v.x << ", " << v.y << ", " << v.z <<  << v.w;};
};

struct Vec4 {
double x, y, z, w;
Vec4 operator+(const double& s) const {return {x + s, y + s, z + s, w + s};}
Vec4 operator-(const double& s) const {return {x - s, y - s, z - s, w - s};}
Vec4 operator*(const double& s) const {return {x * s, y * s, z * s, w * s};}
Vec4 operator/(const double& s) const {return {x / s, y / s, z / s, w / s};}
Vec4& operator+=(const Vec4& s){x += s.x; y += s.y; z += s.z; w += s.w; return *this;}
Vec4& operator-=(const Vec4& s){x -= s.x; y -= s.y; z -= s.z; w -= s.w; return *this;}
Vec4& operator*=(const Vec4& s){x *= s.x; y *= s.y; z *= s.z; w *= s.w; return *this;}
Vec4& operator/=(const Vec4& s){x /= s.x; y /= s.y; z /= s.z; w /= s.w; return *this;}
Vec4& operator+=(const double& s){x += s; y += s; z += s; w += s;return *this;}
Vec4& operator-=(const double& s){x -= s; y -= s; z -= s; w -= s;return *this;}
Vec4& operator*=(const double& s){x *= s; y *= s; z *= s; w *= s;return *this;}
Vec4& operator/=(const double& s){x /= s; y /= s; z /= s; w /= s;return *this;}
Vec4 operator+(const Vec4& s) const {return {x + s.x, y + s.y, z + s.z, w + s.w};}
Vec4 operator-(const Vec4& s) const {return {x - s.x, y - s.y, z - s.z, w - s.w};}
template <typename T, typename Traits>friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Vec4& v){
return out << v.x << ", " << v.y << ", " << v.z <<  << v.w;};
};

}
