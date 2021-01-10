#ifndef RENESAS_H_
#define RENESAS_H_

// includes
#include <stdio.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/position_sensor.h>

#include "renesas_controller.h"

// defines and macros
#define TIME_STEP 8
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// function prototypes
void handle(int angle);
void motor(int left, int right);
void motor2(int back_right, int back_left, int front_right, int front_left);
unsigned short* line_sensor();
void renesas_mcu_init();
void update_sensors();
float compute_torque(float u, float u_max, float om, float om_max, float t_max, float gear);
double get_gear_ratio(char *motor_node_name);

#endif