#pragma once
#include <cmath>
#include <iostream>

template <typename T>
struct vec3{
    T x;
    T y;
    T z;
};


using vec3i = vec3<int>;
using vec3f = vec3<float>;

template <typename T>
vec3<T> operator+(const vec3<T> a, const vec3<T> b){
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
vec3<T> operator-(const vec3<T> a, const vec3<T> b){
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
vec3<T> operator*(const T a, const vec3<T> b){
    return {a*b.x, a*b.y, a*b.z};
}

template <typename T>
vec3<T> operator*(const vec3<T> a, const T b){
    return {a.x*b, a.y*b, a.z*b};
}

template <typename T>
vec3<T> operator/(const vec3<T> a, const T b){
    return {a.x/b, a.y/b, a.z/b};
}

template <typename T>
vec3<T>& operator+=(vec3<T>& a, const vec3<T> b){
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

template <typename T>
vec3<T>& operator-=(vec3<T>& a, const vec3<T> b){
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}


template <typename T>
vec3<T> vabs(const vec3<T> a){
    return {std::abs(a.x), std::abs(a.y), std::abs(a.z)};
}

template <typename T>
T dot(const vec3<T> a, const vec3<T> b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
T cross(const vec3<T> a, const vec3<T> b){
    return {a.y * b.z - b.y * a.z,
            a.z * b.x - b.z * a.x,
            a.x * b.y - b.x * a.y};
}

template <typename T>
T length(const vec3<T> a){
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

template <typename T>
T length_squared(const vec3<T> a){
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

template <typename T>
vec3<T> normalize(const vec3<T> a){
    return a / length(a);
}
