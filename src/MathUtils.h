#pragma once

#include <cmath>
#include <array>
#include <iostream>
#include <cstdlib>

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
    T const &operator[](size_t i) const { return p[i]; };
    T &operator[](size_t i) { return p[i]; };
    
    void print() const { std::cout << p[0] << " : " << p[1] << " : " << p[2] << std::endl;}

    T norm() const {
        T sum = 0;
        for(const T &t : p) sum += t * t;
        return sqrt(sum);
    }

    T sqrNorm() const {
        T sum = 0;
        for(const T &t : p) sum += t * t;
        return sum;
    }

    Vec3<T> normalize() {
        T mag = norm();
        if(mag > EPSILON)
            for(T &t : p) t /= mag;
        return *this;
    }

    Vec3<T> mix(const Vec3<T> &other, T factor){
        return (*this) * factor + other * (1 - factor);
    }

    T dot(const Vec3<T> &other) const { return p[0] * other.p[0] + p[1] * other.p[1] + p[2] * other.p[2]; }
    Vec3<T> cross(const Vec3<T> &other) const {
        return {
            p[1] * other.p[2] - p[2] * other.p[1],
            p[2] * other.p[0] - p[0] * other.p[2],
            p[0] * other.p[1] - p[1] * other.p[0]
        };
    }

    Vec3<T> operator*(const T &s) const { return {s * p[0], s * p[1], s * p[2]}; }
    Vec3<T> operator*(const Vec3<T> &s) const { return {s[0] * p[0], s[1] * p[1], s[2] * p[2]}; }
    Vec3<T> operator+(const Vec3<T> &other) const {return {p[0] + other.p[0], p[1] + other.p[1], p[2] + other.p[2]}; }
    Vec3<T> operator-(const Vec3<T> &other) const {return {p[0] - other.p[0], p[1] - other.p[1], p[2] - other.p[2]}; }
    
    void clamp(T min, T max) {
        for(T &t : p) t = t < min ? min : t > max ? max : t;
    }

    void correct_gamma(double factor = 1.5) {
        clamp(0.0, 1.0);
        double power = 1.0 / factor;
        for(T &t : p) t = pow(t, power);
    }
};

typedef Vec3<double> Vec3d;
typedef Vec3<double> Color;

const Color red{1, 0, 0};
const Color green{0, 1, 0};
const Color blue{0, 0, 1};
const Color black{0, 0, 0};
const Color white{1, 1, 1};
const Color background{160/255.0, 1, 1};

inline double RandomFloat01()
{
    return rand() / double(RAND_MAX);
}

inline Vec3d random_in_unit_sphere() {
    Vec3d p;
    do {
        p = Vec3d(RandomFloat01(),RandomFloat01(),RandomFloat01()) * 2.0 - Vec3d(1,1,1);
    } while (p.sqrNorm() >= 1.0);
    return p;
}

inline Vec3d reflect(const Vec3d &in, const Vec3d &n) {
    return in - n * 2 * in.dot(n);
}

inline bool refract(const Vec3d& v, const Vec3d& n, float ni_over_nt, Vec3d& refracted) {
    Vec3d uv = v;
    uv.normalize();
    float dt = uv.dot(n);
    float discriminat = 1.0 - ni_over_nt * ni_over_nt * (1-dt*dt);
    if(discriminat > 0){
        refracted = (uv-n*dt)*ni_over_nt - n*sqrt(discriminat);
        return true;
    }
    else
        return false; // no refracted ray
}

inline Vec3d refractVector(const Vec3d &I, const Vec3d &N, const float &ior) 
{ 
    float cosi = I.dot(N);
    float etai = 1, etat = ior; 
    Vec3d n = N; 
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= N*-1; } 
    float eta = etai / etat; 
    float k = 1 - eta * eta * (1 - cosi * cosi); 
    return k < 0 ? 0 : I * eta +  n * (eta * cosi - sqrtf(k)); 
} 


inline double schlick(double c, double refract_ind) {
    double r0 = (1.0-refract_ind) / (1.0+refract_ind);
    r0 = r0*r0;
    return r0 + (1-r0)*pow(1-c, 5);
}