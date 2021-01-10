#ifndef RENESAS_API_H_
#define RENESAS_API_H_

// includes
#include <stdio.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/motor.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/position_sensor.h>

#include "renesas_api_webots.h"

// defines and macros
#define TIME_STEP 10
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// function prototypes
void handle(int angle);
void motor(int left, int right);
void motor_all(int back_left, int back_right, int front_left, int front_right);
unsigned short* line_sensor();
void init();
void update();

#endif
