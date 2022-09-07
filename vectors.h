#pragma once

#include <cmath>

template <class t> 
class Vector2 {
public:
    union {
        struct {t u, v;};
        struct {t x, y;};
        t raw[2];
    };
    Vector2() : u(0), v(0) {}
    Vector2(t _u, t _v) : u(_u), v(_v) {}
    Vector2<t> operator+ (const Vector2<t> &V) { 
        return Vector2<t>(u+V.u, v+V.v);
    }
    Vector2<t> operator- (const Vector2<t> &V) {
        return Vector2<t>(u-V.u, v+V.v);
    }
    Vector2<t> operator* (float value) {
        return Vector2<t>(value*u, value*v);
    }
};


template <class t>
class Vector3 {
public:
    union {
        struct {t x, y, z;};
        struct {t ivertex, iuv, inorm;};
        t raw[3];
    };
    Vector3() : x(0), y(0), z(0) {}
    Vector3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
    Vector3<t> operator ^(const Vector3<t> &v) { 
        return Vector3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
    }
	Vector3<t> operator +(const Vector3<t> &v) {
        return Vector3<t>(x+v.x, y+v.y, z+v.z);
    }
	Vector3<t> operator -(const Vector3<t> &v) {
        return Vector3<t>(x-v.x, y-v.y, z-v.z);
    }
	Vector3<t> operator *(float f) {
        return Vector3<t>(x*f, y*f, z*f);
    }
	t operator *(const Vector3<t> &v){ 
        return x*v.x + y*v.y + z*v.z;
    }
	float norm () const {
        return std::sqrt(x*x+y*y+z*z);
    }
	Vector3<t> & normalize(t l=1) {
        *this = (*this)*(l/norm()); return *this;
    }
    Vector3 crossProduct(Vector3<t> v2) {
        return Vector3<t>(y*v2.z - z*v2.y, z*v2.x - x*v2.z, x*v2.y - y*v2.x); 
    }
};

typedef Vector2<int> Vector2Int;
typedef Vector2<float> Vector2Float;
typedef Vector3<int> Vector3Int;
typedef Vector3<float> Vector3Float;