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

// function prototypes
void init();
void update();
void handle(int angle);
void motor(int back_right, int back_left, int front_right, int front_left);
unsigned short *line_sensor();
double time();

#endif
