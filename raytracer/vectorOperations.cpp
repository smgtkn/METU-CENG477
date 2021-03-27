#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
using namespace std;
using namespace parser;


Vec3f Vec3f::operator+(const Vec3f &v) const {

     Vec3f result;
     result.x=v.x+x;
     result.y=v.y+y;
     result.z=v.z+z;
     return result;

}
Vec3f Vec3f::operator-(const Vec3f &v) {

    Vec3f result;
    result.x=x-v.x;
    result.y=y-v.y;
    result.z=z-v.z;
    return result;

}
Vec3f Vec3f::operator*(double d) const {
    Vec3f result;
    result.x=x*d;
    result.y=y*d;
    result.z=z*d;
    return result;
}
Vec3f Vec3f::operator/(double d) {

    Vec3f result;
    result.x=x/d;
    result.y=y/d;
    result.z=z/d;
    return result;

}


Vec3f Vec3f::cross(const Vec3f &v) {
    Vec3f result;
    result.x=y*v.z- v.y * z;
    result.y= v.x * z - x * v.z;
    result.z=x * v.y - v.x * y;

    return result;
}
float Vec3f::dot(const Vec3f &v) {
    return x * v.x + y * v.y + z * v.z;
}
double Vec3f::length(){
    return sqrt(x*x+y*y+z*z);
}
Vec3f Vec3f::normalization(){
    double l= length();
    Vec3f result;
    if(l>0){
        result.x=x/l;
        result.y=y/l;
        result.z=z/l;

        return result;
    }

}

