#include "renesas_api.h"
#include "math.h"
#include "string.h"

/**
 * @brief Global variables.
 * 
 */
WbDeviceTag motor_handle[4]; // br, bl, fr, fl
WbDeviceTag encoder_handle[4];
double encoder_current[4] = {0.0, 0.0, 0.0, 0.0};
double encoder_previous[4] = {0.0, 0.0, 0.0, 0.0};
double velocity[4] = {0.0, 0.0, 0.0, 0.0};
int n_sensors = 32;
WbDeviceTag sensor_handle[32];
unsigned short sensor[32];
double gear_ratio = 10.0;
double current_time = 0.0;
WbDeviceTag center_motor;
WbDeviceTag imu_handle;
const double *rpy;

/**
 * @brief Initializes the Renesas MCU controller.
 * 
 */
void init()
{
  char str[100];

  // motor init
  motor_handle[0] = wb_robot_get_device(strcat(strcpy(str, BR_MOTOR_NAME), "_motor"));
  motor_handle[1] = wb_robot_get_device(strcat(strcpy(str, BL_MOTOR_NAME), "_motor"));
  motor_handle[2] = wb_robot_get_device(strcat(strcpy(str, FR_MOTOR_NAME), "_motor"));
  motor_handle[3] = wb_robot_get_device(strcat(strcpy(str, FL_MOTOR_NAME), "_motor"));

  for (int i = 0; i < 4; i++)
  {
    wb_motor_set_position(motor_handle[i], INFINITY);
    wb_motor_set_torque(motor_handle[i], 0.0);
  }

  // encoder init
  encoder_handle[0] = wb_robot_get_device(strcat(strcpy(str, BR_MOTOR_NAME), "_encoder"));
  encoder_handle[1] = wb_robot_get_device(strcat(strcpy(str, BL_MOTOR_NAME), "_encoder"));
  encoder_handle[2] = wb_robot_get_device(strcat(strcpy(str, FR_MOTOR_NAME), "_encoder"));
  encoder_handle[3] = wb_robot_get_device(strcat(strcpy(str, FL_MOTOR_NAME), "_encoder"));

  for (int i = 0; i < 4; i++)
  {
    wb_position_sensor_enable(encoder_handle[i], TIME_STEP);
    encoder_previous[i] = wb_position_sensor_get_value(encoder_handle[i]);
  }

  // sensor init
  n_sensors = get_number_of_sensors();
  char name[20];
  for (int i = 0; i < n_sensors; i++)
  {
    sprintf(name, "sensor_%d", i);
    sensor_handle[i] = wb_robot_get_device(name); /* line sensors */
    wb_distance_sensor_enable(sensor_handle[i], TIME_STEP);
  }

  gear_ratio = get_gear_ratio();

  // handle motor
  center_motor = wb_robot_get_device("center_motor");

  // imu
  imu_handle = wb_robot_get_device("imu");
  wb_inertial_unit_enable(imu_handle, TIME_STEP);
}

/**
 * @brief Updates sensor values, should be called in main controller while loop.
 * 
 */
void update()
{
  // encoder update
  for (int i = 0; i < 4; i++)
  {
    encoder_current[i] = wb_position_sensor_get_value(encoder_handle[i]);
    velocity[i] = -1000.0 * (encoder_current[i] - encoder_previous[i]) / TIME_STEP;
    encoder_previous[i] = encoder_current[i];
  }

  // line sensor update
  for (int i = 0; i < n_sensors; i++)
    sensor[i] = wb_distance_sensor_get_value(sensor_handle[i]);

  // imu update
  rpy = wb_inertial_unit_get_roll_pitch_yaw(imu_handle);

  // time update
  current_time += TIME_STEP / 1000.0;
}

/**
 * @brief Servo steering operation.
 * 
 * @param angle Servo operation angle: -60 to 60
 *              -60: 60-degree turn to left, 0: straight, 
 *              60: 60-degree turn to right 
 */
void handle(int angle)
{
  wb_motor_set_position(center_motor, constrain(angle, -60, 60) * M_PI / 180.0);
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
void motor(int back_right, int back_left, int front_right, int front_left)
{
  wb_motor_set_torque(motor_handle[0], compute_torque((float)constrain(back_right, -100, 100) * 3.7 / 100, velocity[0], gear_ratio));
  wb_motor_set_torque(motor_handle[1], compute_torque((float)constrain(back_left, -100, 100) * 3.7 / 100, velocity[1], gear_ratio));
  wb_motor_set_torque(motor_handle[2], compute_torque((float)constrain(front_right, -100, 100) * 3.7 / 100, velocity[2], gear_ratio));
  wb_motor_set_torque(motor_handle[3], compute_torque((float)constrain(front_left, -100, 100) * 3.7 / 100, velocity[3], gear_ratio));
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
 * @brief Returns the current velocities.
 * 
 * @return double* Motor velocities (in rad/s)
 */
double *encoders()
{
  return velocity;
}

/**
 * @brief Returns roll, pitch, and yaw angles.
 * 
 * @return double* Roll, pitch, and yaw in rad.
 */
double *imu()
{
  return (double *)rpy;
}

/**
 * @brief Returns the current time in seconds.
 * 
 * @return double Time in seconds.
 */
double time()
{
  return current_time;
}

/**
 * @brief Get the track
 * 
 * @return double Track
 */
double get_track()
{
  return get_float_field("track");
}

/**
 * @brief Get the wheel base
 * 
 * @return double Wheel base
 */
double get_wheel_base()
{
  return get_float_field("wheelBase");
}

/**
 * @brief Get the sensor base
 * 
 * @return double Sensor base
 */
double get_sensor_base()
{
  return get_float_field("sensorBase");
}

/**
 * @brief Get the gear ratio
 * 
 * @return double Gear ratio
 */
double get_gear_ratio()
{
  return get_float_field("gearRatio");
}

/**
 * @brief Get the tyre type
 * 
 */
const char *get_tyre_type()
{
  return get_string_field("tyreType");
}

/**
 * @brief Get the tyre width
 * 
 * @return double Tyre width
 */
double get_tyre_width()
{
  return get_float_field("tyreWidth");
}

/**
 * @brief Get the tyre radius
 * 
 * @return double Tyre radius
 */
double get_tyre_radius()
{
  return get_float_field("tyreRadius");
}

/**
 * @brief Get the number of sensors
 * 
 * @return int Number of sensors
 */
int get_number_of_sensors()
{
  return get_int_field("numberOfSensors");
}

/**
 * @brief Get the distance between the sensors
 * 
 * @return double Distance between the sensors
 */
double get_distance_between_sensors()
{
  return get_float_field("distanceBetweenSensors");
}

/**
 * @brief Get the weight penalty
 * 
 * @return double Weight penalty
 */
double get_weight_penalty()
{
  return get_float_field("weightPenalty");
}
