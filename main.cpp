#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include "rk4.hpp"
#include "vec2.hpp"
#include "vec3.hpp"

float simulate_newtonian_drag(vec3f turret_pos, vec3f target_pos, vec3f velocity, std::ofstream* of = nullptr){
    std::array<float, 6> initial_state = {turret_pos.x, velocity.x, turret_pos.y, velocity.y, turret_pos.z, velocity.z};
    
    auto func = [](float t, std::array<float, 6> X) -> std::array<float, 6>{
        //float u1 = X[0];
        float u2 = X[1];
        //float u3 = X[2];
        float u4 = X[3];
        //float u5 = X[4];
        float u6 = X[5];
        
        constexpr float km = 0.2041f;
        constexpr float g = 9.81f;
        
        const float v = std::sqrt(u2*u2 + u4*u4 + u6*u6);
        
        float du1 = u2;
        float du2 = -km * v * u2;
        float du3 = u4;
        float du4 = -km * v * u4;
        float du5 = u6;
        float du6 = -km * v * u6 - g;
        
        return {du1, du2, du3, du4, du5, du6};
    };
    
    RKF45 integrator(initial_state, 0.0f, func, 0.01f);
    int i = 0;
    float last_x = turret_pos.x;
    float last_y = turret_pos.y;
    float last_z = turret_pos.z;
    
    float last_distance = length_squared(target_pos - vec3f{integrator.state[0], integrator.state[2], integrator.state[4]});
    float improvement = 0.0f;
    
    do{
        last_x = integrator.state[0];
        last_y = integrator.state[2];
        last_z = integrator.state[4];
        if(of != nullptr){
            *of << integrator.t << " ";
            *of << integrator.state[0] << " ";
            *of << integrator.state[2] << " ";
            *of << integrator.state[4] << " ";
            *of << "\n";
        }
        integrator.step();
        i++;
        float new_distance = length_squared(target_pos - vec3f{integrator.state[0], integrator.state[2], integrator.state[4]});
        improvement = last_distance - new_distance;
        last_distance = new_distance;
    }while(improvement >= 0.0f and i < 1000);
    
    float lerp_factor = (target_pos.z-last_z) / (integrator.state[4] - last_z);
    lerp_factor = std::clamp(lerp_factor, 0.0f, 1.0f);
    
    float z_intercept = last_z + lerp_factor * (integrator.state[4] - last_z); //should always be target_height
    float y_intercept = last_y + lerp_factor * (integrator.state[2] - last_y);
    float x_intercept = last_x + lerp_factor * (integrator.state[0] - last_x);
    
    if(of != nullptr){
        *of << integrator.t << " ";
        *of << x_intercept << " ";
        *of << y_intercept << " ";
        *of << z_intercept << " ";
    }
    
    return last_distance;
}

vec2f get_shot_direction(vec3f turret_position, vec3f target_position, vec2f turret_velocity, float elevation_angle, float power){
    float sin_theta = std::sin(elevation_angle);
    vec3f target_direction = target_position - turret_position;
    
    
    float vx = turret_velocity.x;
    float vy = turret_velocity.y;
    
    float c = power * sin_theta;
    
    float dx = target_direction.x;
    float dy = target_direction.y;
    
    float u_part_one = std::sqrt(c*c * dx*dx * (-vx*vx * dy*dy + 2.0f * vx * vy * dx * dy - vy*vy * dx * dx + c*c * (dx*dx + dy*dy)));
    
    float u_part_two = -vx * c * dy*dy + vy * c * dx * dy;
    
    float u = (u_part_one + u_part_two) / (c*c * (dx*dx + dy*dy));
    
    if(std::signbit(turret_velocity.x + c * u) != std::signbit(target_direction.x)){
        u = (-u_part_one + u_part_two) / (c*c * (dx*dx + dy*dy));
    }
    
    float v = std::sqrt(1.0f - u*u);
    
    if(std::signbit(turret_velocity.y + c * v) != std::signbit(target_direction.y)){
        v = -v;
    }
    
    
    return {u,v};
}

void print_simulation(vec3f turret_position, vec3f target_position, vec2f turret_velocity, float angle, float power, const std::string& file_name){
    vec2f shot_direction = get_shot_direction(turret_position, target_position, turret_velocity, angle, power);
    
    float vx = turret_velocity.x + power * std::sin(angle) * shot_direction.x;
    float vy = turret_velocity.y + power * std::sin(angle) * shot_direction.y;
    float vz = power * std::cos(angle);
    
    std::ofstream of(file_name);
    
    of << turret_position.x << " " << turret_position.y << " " << turret_position.z << " ";
    of << target_position.x << " " << target_position.y << " " << target_position.z << "\n";
    
    vec3f turret_pos = {turret_position.x, turret_position.y, 0.0f};
    vec3f target_pos = {target_position.x, target_position.y, 2.46f};
    vec3f velocity = {vx, vy, vz};
    std::cout << "final error: " << simulate_newtonian_drag( turret_pos, target_pos, velocity, &of) << std::endl;
}

float get_error(vec3f turret_position, vec3f target_position, vec2f turret_velocity, float angle, float power){
    vec2f shot_direction = get_shot_direction(turret_position, target_position, turret_velocity, angle, power);
    
    float vx = turret_velocity.x + power * std::sin(angle) * shot_direction.x;
    float vy = turret_velocity.y + power * std::sin(angle) * shot_direction.y;
    float vz = power * std::cos(angle);
    
    vec3f velocity = {vx, vy, vz};
    return simulate_newtonian_drag(turret_position, target_position, velocity);
}

void find_root(vec3f turret_position, vec3f target_position, vec2f turret_velocity, float max_power, const std::string& file_prefix){
    float power = max_power;
    
    vec3f dist = target_position - turret_position;
    float angle = M_PI/2.0 - std::atan2(dist.z, std::sqrt(dist.x*dist.x + dist.y*dist.y)); //initial angle -> just point directly at the target
    
    
    float initial_angle_step = -0.3f * M_PI / 180.0f;
    
    print_simulation(turret_position, target_position, turret_velocity, angle, power, file_prefix + "initial.dat");
    
    float last_error = get_error(turret_position, target_position, turret_velocity, angle, power);
    
    float last_angle = angle;
    angle = angle + initial_angle_step;
    
    float error = get_error(turret_position, target_position, turret_velocity, angle, power);
    
    for(int i = 0; i < 10; i++){
        float new_angle = angle - error * (angle - last_angle)/(error-last_error);
        last_error = error;
        last_angle = angle;
        angle = new_angle;
        error = get_error(turret_position, target_position, turret_velocity, angle, power);
        if(error < 0.01f) break;
    }
    
    print_simulation(turret_position, target_position, turret_velocity, angle, power, file_prefix + "final.dat");

    
}

int main(int argc, char** argv){
    
    if(argc != 10){
        std::cout << "usage turret_x turret_y turret_z target_x target_y target_z turret_vel_x turret_vel_y power\n";
        return 0;
    }
    
    
    vec3f turret_position = {std::stof(argv[1]), std::stof(argv[2]), std::stof(argv[3])};
    vec3f target_position = {std::stof(argv[4]), std::stof(argv[5]), std::stof(argv[6])};
    vec2f turret_velocity = {std::stof(argv[7]), std::stof(argv[8])};
    float max_power = std::stof(argv[9]);
    
    
    find_root(turret_position, target_position, turret_velocity, max_power, "results");
    
    
    
}
