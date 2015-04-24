#pragma once
namespace cms {
struct Vec3f {
float x, y, z;
Vec3f operator+(const float& s){return {x + s, y + s, z + s};}
};
}
