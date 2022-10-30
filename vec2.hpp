#ifndef VEC2_HPP
#define VEC2_HPP
#include <cmath>
#include <iostream>

template <typename T>
struct vec2{
    T x;
    T y;
    /*operator sf::Vector2f() const{
        return {x,y};
    }*/
    template <typename S>
    operator vec2<S>(){
        return {static_cast<S>(x), static_cast<S>(y)};
    }
    void print(){
        std::cout << x << " " << y << "\n";
    }
};

using vec2i = vec2<int>;
using vec2f = vec2<float>;

template <typename T>
vec2<T> operator+(const vec2<T> a, const vec2<T> b){
    return {a.x + b.x, a.y + b.y};
}

template <typename T>
vec2<T> operator-(const vec2<T> a, const vec2<T> b){
    return {a.x - b.x, a.y - b.y};
}

template <typename T>
vec2<T> operator*(const T a, const vec2<T> b){
    return {a*b.x, a*b.y};
}

template <typename T>
vec2<T> operator/(const vec2<T> a, T b){
    return {a.x/b, a.y/b};
}

template <typename T>
vec2<T> vabs(const vec2<T> a){
    return {std::abs(a.x), std::abs(a.y)};
}

template <typename T>
T dot(const vec2<T> a, const vec2<T> b){
    return a.x * b.x + a.y * b.y;
}

template <typename T>
T cross(const vec2<T> a, const vec2<T> b){
    return a.x * b.y - a.y * b.x;
}

template <typename T>
T length(const vec2<T> a){
    return std::sqrt(a.x * a.x + a.y * a.y);
}

template <typename T>
T length_squared(const vec2<T> a){
    return a.x * a.x + a.y * a.y;
}

template <typename T>
vec2<T> normalize(const vec2<T> a){
    return a / length(a);
}

#endif
