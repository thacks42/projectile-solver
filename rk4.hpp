#ifndef RK4_HPP
#define RK4_HPP

#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>


template <typename Real, size_t S>
std::array<Real, S> operator+(const std::array<Real, S>& lhs, const std::array<Real, S>& rhs){
    std::array<Real, S> result;
    for(size_t i = 0; i < S; i++){
        result[i] = lhs[i] + rhs[i];
    }
    return result;
}

template <typename Real, size_t S>
std::array<Real, S> operator-(const std::array<Real, S>& lhs, const std::array<Real, S>& rhs){
    std::array<Real, S> result;
    for(size_t i = 0; i < S; i++){
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}


template <typename Real, size_t S>
std::array<Real, S> operator*(const Real lhs, const std::array<Real, S>& rhs){
    std::array<Real, S> result;
    for(size_t i = 0; i < S; i++){
        result[i] = lhs * rhs[i];
    }
    return result;
}

template <typename Real, size_t S>
Real abs_max(const std::array<Real, S>& arr){
    return std::abs(*std::max_element(arr.begin(), arr.end(),
                            [](Real lhs, Real rhs){
                                return std::abs(lhs) < std::abs(rhs);
                            }));
}


template <typename Func, typename Real, size_t S>
struct Integrator{
    
    using state_t = std::array<Real, S>;
    state_t state;
    Real t;
    Func f;
    Real h;
    Integrator(state_t initial_state, Real t0, Func f, Real h = Real(0.00001)) : state(initial_state), t(t0), f(f), h(h){
        
    }
    void step(){
        state_t k1 = f(t, state);
        state_t k2 = f(t + h/Real(2.0), state + h/Real(2.0) * k1);
        state_t k3 = f(t + h, state - h * k1 + Real(2.0) * h * k2);
        
        state = state + h * (Real(1.0) / Real(6.0) * k1 + Real(4.0) / Real(6.0) * k2 + Real(1.0) / Real(6.0) * k3);
        t+=h;
    }
};


template <typename Func, typename Real, size_t S>
struct RKF45{
    
    using state_t = std::array<Real, S>;
    state_t state;
    Real t;
    Func f;
    Real tol; //tolerance
    Real h = 0.00001;
    RKF45(state_t initial_state, Real t0, Func f, Real tol = Real(0.001)) :
                            state(initial_state), t(t0), f(f), tol(tol){
        
    }
    
    bool step(){
        state_t k1 = h * f(t, state);
        
        state_t k2 = h * f( t + h / Real(4.0),
                            state + h/Real(4.0) * k1);
        
        state_t k3 = h * f( t + h * Real(3.0) / Real(8.0),
                            state + Real(3.0) / Real(32.0) * k1
                                  + Real(9.0) / Real(32.0) * k2);
        
        state_t k4 = h * f( t + h * Real(12.0) / Real(13.0),
                            state + Real(1932.0) / Real(2197.0) * k1
                                  - Real(7200.0) / Real(2197.0) * k2
                                  + Real(7296.0) / Real(2197.0) * k3);
        
        state_t k5 = h * f( t + h,
                            state + Real(439.0) / Real(216.0) * k1
                                  - Real(8.0) * k2
                                  + Real(3680.0) / Real(513.0) * k3
                                  - Real(845.0) / Real(4104.0) * k4);
        
        state_t k6 = h * f( t + h * Real(1.0) / Real(2.0),
                            state - Real(8.0) / Real(27.0) * k1
                                  + Real(2.0) * k2
                                  + Real(3544.0) / Real(2565.0) * k3
                                  + Real(1859.0) / Real(4104.0) * k4
                                  - Real(11.0) / Real(40.0) * k5);
        
        /*state_t rk4 = state + Real(25.0) / Real(216.0) * k1
                            + Real(1408.0) / Real(2565.0) * k3
                            + Real(2197.0) / Real(4101.0) * k4
                            - Real(1.0) / Real(5.0) * k5;*/
        
        state_t error_s = Real(1.0) / Real(36.0) * k1
                        - Real(128.0) / Real(4275.0) * k3
                        - Real(2197.0) / Real(75240.0) * k4
                        + Real(1.0) / Real(50.0) * k5
                        + Real(2.0) / Real(55.0) * k6;
        
        auto error = abs_max(error_s);
        
        if(error <= tol){
            state = state   + Real(16.0) / Real(135.0) * k1
                            + Real(6656.0) / Real(12825.0) * k3
                            + Real(28561.0) / Real(56430.0) * k4
                            - Real(9.0) / Real(50.0) * k5
                            + Real(2.0) / Real(55.0) * k6;
            
            t += h;
            h *= Real(0.84) * std::pow(tol / error, Real(0.25));
            return true;
        }
        h *= Real(0.84) * std::pow(tol / error, Real(0.25));
        return false;
    }
};













#endif
