#include "renesas_api_webots.h"
#include "math.h"
#include "string.h"

/**
 * @brief Computes the torque to be applied to the motor.
 * 
 * @param u Voltage
 * @param u_max Max. motor supply voltage
 * @param om Current motor velocity
 * @param om_max Free run velocity of the motor
 * @param t_max Stall torque of the motor
 * @param gear Gear
 * @return float 
 */
float compute_torque(float u, float u_max, float om, float om_max, float t_max, float gear)
{
  return -(t_max * constrain(u, -u_max, u_max) / u_max - (t_max * gear) * om / (om_max / gear));
}

/**
 * @brief Get the gear ratio of a motor from node definition.
 * 
 * @param motor_node_name MCUMotor node name.
 * @return double Gear ratio.
 */
double get_gear_ratio(char *motor_node_name)
{
  WbNodeRef robot_node = wb_supervisor_node_get_self();
  WbFieldRef back_right_motor_field = wb_supervisor_node_get_field(robot_node, strcat(motor_node_name, "_motor"));
  WbNodeRef back_right_motor_node = wb_supervisor_field_get_sf_node(back_right_motor_field);
  WbFieldRef back_right_gear_ratio = wb_supervisor_node_get_field(back_right_motor_node, "gearRatio");
  return wb_supervisor_field_get_sf_float(back_right_gear_ratio);
}
