#pragma once

#include <array>
#include <iostream>

constexpr double INF = 1e10;
constexpr double EPSILON = 1e-6;

template<typename T>
class Vec3 {
    std::array<T, 3> p;

public:
    Vec3(): p{{0,0,0}} {}
    Vec3(T t): p{{t, t, t}} {}
    Vec3(T x, T y, T z): p{{x,y,z}} {}
    Vec3(std::array<T, 3> &arr): p{arr} {}
    T &x() { return p[0]; }
    T &y() { return p[1]; }
    T &z() { return p[2]; }
    T &r() { return p[0]; }
    T &g() { return p[1]; }
    T &b() { return p[2]; }
    void print() const { std::cout << p[0] << " : " << p[1] << " : " << p[2] << std::endl;}

    T norm() const {
        T sum = 0;
        for(const T &t : p) sum += t * t;
        return sqrt(sum);
    }
    void normalize() {
        T mag = norm();
        if(mag > EPSILON)
            for(T &t : p) t /= mag;
    }

    T dot(const Vec3<T> &other) const { return p[0] * other.p[0] + p[1] * other.p[1] + p[2] * other.p[2]; }
    Vec3<T> cross(const Vec3<T> &other) const {
        return {
            p[1] * other.p[2] - p[2] * other.p[1],
            p[2] * other.p[0] - p[0] * other.p[2],
            p[0] * other.p[1] - p[1] * other.p[0]
        };
    }

    Vec3<T> operator*(const double &s) const { return {s * p[0], s * p[1], s * p[2]}; }
    // Vec3<T> operator*(const T &s) const { return {s * p[0], s * p[1], s * p[2]}; }
    Vec3<T> operator+(const Vec3<T> &other) const {return {p[0] + other.p[0], p[1] + other.p[1], p[2] + other.p[2]}; }
    Vec3<T> operator-(const Vec3<T> &other) const {return {p[0] - other.p[0], p[1] - other.p[1], p[2] - other.p[2]}; }
};

typedef Vec3<double> Vec3d;
typedef Vec3<int> Color;

const Color red{255, 0, 0};
const Color green{0, 255, 0};
const Color blue{0, 0, 255};
const Color black{0, 0, 0};
const Color white{255, 255, 255};
const Color background{160, 255, 253};