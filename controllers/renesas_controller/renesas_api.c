#include "renesas_api.h"
#include "math.h"
#include "string.h"

/**
 * @brief Global variables.
 * 
 */
WbDeviceTag back_right_motor;
WbDeviceTag back_left_motor;
WbDeviceTag front_right_motor;
WbDeviceTag front_left_motor;
WbDeviceTag back_right_encoder;
WbDeviceTag back_left_encoder;
WbDeviceTag front_right_encoder;
WbDeviceTag front_left_encoder;
double br_velocity = 0.0;
double bl_velocity = 0.0;
double fr_velocity = 0.0;
double fl_velocity = 0.0;
double br_encoder_previous = 0.0;
double bl_encoder_previous = 0.0;
double fr_encoder_previous = 0.0;
double fl_encoder_previous = 0.0;
WbDeviceTag sensor_handle[N_SENSORS];
unsigned short sensor[N_SENSORS];

/**
 * @brief Servo steering operation.
 * 
 * @param angle Servo operation angle: -90 to 90
 *              -90: 90-degree turn to left, 0: straight, 
 *              90: 90-degree turn to right 
 */
void handle(int angle)
{
  WbDeviceTag center_motor = wb_robot_get_device("center_motor");
  wb_motor_set_position(center_motor, constrain(angle, -60, 60) * M_PI / 180.0);
}

void motor(int left, int right)
{
  motor_all(right, left, right, left);
}

/**
 * @brief Motor speed control
 *        (0 is stopped, 100 is forward, and -100 is reverse)
 * 
 * @param back_right Back right motor
 * @param back_left Back left motor
 * @param front_right Front right motor
 * @param front_left  Front left motor
 */
void motor_all(int back_right, int back_left, int front_right, int front_left)
{
  wb_motor_set_torque(back_right_motor, compute_torque((float)back_right, 100.0, br_velocity, 1.0, 0.1, 1));
  wb_motor_set_torque(back_left_motor, compute_torque((float)back_left, 100.0, bl_velocity, 1.0, 0.1, 1));
  wb_motor_set_torque(front_right_motor, compute_torque((float)front_right, 100.0, fr_velocity, 1.0, 0.1, 1));
  wb_motor_set_torque(front_left_motor, compute_torque((float)front_left, 100.0, fl_velocity, 1.0, 0.1, 1));
}

/**
 * @brief Returns the values of the line sensors.
 * 
 * @return float* Sensor values.
 */
unsigned short *line_sensor()
{
  return sensor;
}

/**
 * @brief Initializes the Renesas MCU controller.
 * 
 */
void init()
{
  char str[100];

  back_right_motor = wb_robot_get_device(strcat(strcpy(str, BR_MOTOR_NAME), "_motor"));
  back_left_motor = wb_robot_get_device(strcat(strcpy(str, BL_MOTOR_NAME), "_motor"));
  front_right_motor = wb_robot_get_device(strcat(strcpy(str, FR_MOTOR_NAME), "_motor"));
  front_left_motor = wb_robot_get_device(strcat(strcpy(str, FL_MOTOR_NAME), "_motor"));

  // motor init
  wb_motor_set_position(back_left_motor, INFINITY);
  wb_motor_set_position(back_right_motor, INFINITY);
  wb_motor_set_position(front_left_motor, INFINITY);
  wb_motor_set_position(front_right_motor, INFINITY);

  wb_motor_set_torque(back_left_motor, 0.0);
  wb_motor_set_torque(back_right_motor, 0.0);
  wb_motor_set_torque(front_left_motor, 0.0);
  wb_motor_set_torque(front_right_motor, 0.0);

  // encoder init
  back_right_encoder = wb_robot_get_device(strcat(strcpy(str, BR_MOTOR_NAME), "_encoder"));
  back_left_encoder = wb_robot_get_device(strcat(strcpy(str, BL_MOTOR_NAME), "_encoder"));
  front_right_encoder = wb_robot_get_device(strcat(strcpy(str, FR_MOTOR_NAME), "_encoder"));
  front_left_encoder = wb_robot_get_device(strcat(strcpy(str, FL_MOTOR_NAME), "_encoder"));

  wb_position_sensor_enable(back_left_encoder, TIME_STEP);
  wb_position_sensor_enable(back_right_encoder, TIME_STEP);
  wb_position_sensor_enable(front_left_encoder, TIME_STEP);
  wb_position_sensor_enable(front_right_encoder, TIME_STEP);

  bl_encoder_previous = wb_position_sensor_get_value(back_left_encoder);
  br_encoder_previous = wb_position_sensor_get_value(back_right_encoder);
  fl_encoder_previous = wb_position_sensor_get_value(front_left_encoder);
  fr_encoder_previous = wb_position_sensor_get_value(front_right_encoder);

  // sensor init
  char name[20];
  for (int i = 0; i < N_SENSORS; i++)
  {
    sprintf(name, "sensor_%d", i);
    sensor_handle[i] = wb_robot_get_device(name); /* line sensors */
    wb_distance_sensor_enable(sensor_handle[i], TIME_STEP);
  }
}

/**
 * @brief Updates sensor values, should be called in main controller while loop.
 * 
 */
void update()
{
  // encoder update
  double br_encoder = wb_position_sensor_get_value(back_right_encoder);
  double fr_encoder = wb_position_sensor_get_value(front_right_encoder);
  double bl_encoder = wb_position_sensor_get_value(back_left_encoder);
  double fl_encoder = wb_position_sensor_get_value(front_left_encoder);
  br_velocity = -(br_encoder - br_encoder_previous) / TIME_STEP;
  bl_velocity = -(bl_encoder - bl_encoder_previous) / TIME_STEP;
  fr_velocity = -(fr_encoder - fr_encoder_previous) / TIME_STEP;
  fl_velocity = -(fl_encoder - fl_encoder_previous) / TIME_STEP;
  bl_encoder_previous = bl_encoder;
  br_encoder_previous = br_encoder;
  fl_encoder_previous = fl_encoder;
  fr_encoder_previous = fr_encoder;

  // line sensor update
  for (int i = 0; i < N_SENSORS; i++)
    sensor[i] = wb_distance_sensor_get_value(sensor_handle[i]);
}
