#include "renesas_api_webots.h"
#include "math.h"
#include "string.h"

/**
 * @brief Motor parameters
 * 
 */
#define W_NO_LOAD 1000.0
#define V_NOMINAL 3.0
#define T_STALL 0.005
#define I_STALL 5.0
#define I_NO_LOAD 0.2

/**
 * @brief Computes the torque to be applied to the motor.
 * 
 * @param u Voltage
 * @param om Current motor velocity
 * @param gear Gear
 * @return float 
 */
float compute_torque(float u, float om, float gear)
{
  float KW = V_NOMINAL / (W_NO_LOAD / gear);
  float ubemf = KW * om;
  float ua = u - ubemf;
  float R = V_NOMINAL / I_STALL;
  float i = ua / R + I_NO_LOAD;
  float KT = T_STALL / I_STALL;
  float t = (i - I_NO_LOAD) * KT * gear;
  if (t != t)
    return 0;
  return -t;
}

/**
 * @brief Get the value of a int-type proto field
 * 
 * @param field_name 
 * @return int Field value
 */
int get_int_field(char *field_name)
{
  WbNodeRef robot_node = wb_supervisor_node_get_self();
  WbFieldRef field = wb_supervisor_node_get_field(robot_node, field_name);
  return wb_supervisor_field_get_sf_int32(field);
}

/**
 * @brief Get the value of a float-type proto field
 * 
 * @param field_name 
 * @return double Field value
 */
double get_float_field(char *field_name)
{
  WbNodeRef robot_node = wb_supervisor_node_get_self();
  WbFieldRef field = wb_supervisor_node_get_field(robot_node, field_name);
  return wb_supervisor_field_get_sf_float(field);
}

/**
 * @brief Get the value of a string-type proto field
 * 
 * @param field_name 
 * @return double Field value
 */
const char *get_string_field(char *field_name)
{
  WbNodeRef robot_node = wb_supervisor_node_get_self();
  WbFieldRef field = wb_supervisor_node_get_field(robot_node, field_name);
  return wb_supervisor_field_get_sf_string(field);
}
