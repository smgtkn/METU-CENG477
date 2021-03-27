#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream>
#include <vector>
#include <cmath>
namespace parser {

struct Vec3i {
  int x, y, z;
};

struct Vec3f {
  float x, y, z;

  Vec3f() {
    x = 0;
    y = 0;
    z = 0;
  }

  Vec3f(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  Vec3f operator+(const Vec3f& rhs) const {
    Vec3f temp(x + rhs.x, y + rhs.y, z + rhs.z);
    return temp;
  }

  Vec3f operator-(const Vec3f &rhs) const {
     Vec3f temp(x - rhs.x, y - rhs.y, z - rhs.z);
     return temp;
  }

  float operator*(const Vec3f& rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  Vec3f operator*(float rhs) const {
      Vec3f temp(x * rhs, y * rhs, z * rhs);
      return temp;
  }


  Vec3f& operator/=(float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }

  Vec3f cross(const Vec3f rhs) const {
    Vec3f temp(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
                 x * rhs.y - y * rhs.x);
   return temp;
  }




  const Vec3f& normalization() {
    *this /= sqrt(x*x+y*y+z*z);
    return *this;
  }
};

struct Vec4f {
  float x, y, z, w;
};


}

#endif
