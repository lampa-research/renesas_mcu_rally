/*
 * File:          timing_gate.c
 * Date: 
 * Description: 
 * Author: 
 * Modifications:
 */

#include <stdio.h>
#include <string.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/display.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define TIME_STEP 10
#define DETECTION_LIMIT 950 // distance sensor detects objects when closer than this value

// timing
double previous_distance = 1000.0;
long laptimes[3] = {0, 0, 0};
int current_lap = 0;
long current_lap_time = 0;
bool started = false;

// friction coefficients
WbFieldRef cf_soft_field;
float cf_soft = 0.55;
float cf_soft_delta = 0.00005;
WbFieldRef cf_medium_field;
float cf_medium = 0.45;
float cf_medium_delta = 0.00003;
WbFieldRef cf_hard_field;
float cf_hard = 0.35;
float cf_hard_delta = 0.00001;

// leds
WbDeviceTag led_front[5];
WbDeviceTag led_back[5];

void display_init(WbDeviceTag display)
{
  wb_display_set_font(display, "Lucida Sans Unicode", 20, true);
}

void display_clear(WbDeviceTag display)
{
  wb_display_set_color(display, 0xE5E5E5);
  wb_display_fill_rectangle(display, 0, 0, wb_display_get_width(display), wb_display_get_height(display));
  wb_display_set_color(display, 0x0000FF);
}

void display_timings(WbDeviceTag display)
{
  char str[100];
  display_clear(display);
  wb_display_draw_text(display, "Renesas MCU Rally", 2, 2);
  sprintf(str, "%05.2f", current_lap_time / 100.0);
  wb_display_draw_text(display, str, 2, 27);
  sprintf(str, "%05.2f", laptimes[0] / 100.0);
  wb_display_draw_text(display, str, 2, 52);
  sprintf(str, "%05.2f", laptimes[1] / 100.0);
  wb_display_draw_text(display, str, 2, 77);
  sprintf(str, "%05.2f", laptimes[2] / 100.0);
  wb_display_draw_text(display, str, 2, 102);
}

int main(int argc, char **argv)
{
  wb_robot_init();

  // init devices
  WbDeviceTag distance_sensor = wb_robot_get_device("distance_sensor");
  wb_distance_sensor_enable(distance_sensor, TIME_STEP);

  WbDeviceTag display_front = wb_robot_get_device("display_front");
  WbDeviceTag display_back = wb_robot_get_device("display_back");
  display_init(display_front);
  display_init(display_back);

  char name[20];
  for (int i = 0; i < 5; i++)
  {
    sprintf(name, "led_front_%d", i);
    led_front[i] = wb_robot_get_device(name); /* line sensors */
    wb_led_set(led_front[i], 1);
    sprintf(name, "led_back_%d", i);
    led_back[i] = wb_robot_get_device(name); /* line sensors */
    wb_led_set(led_back[i], 1);
  }

  // update contact_property nodes
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  WbNodeRef world_node = wb_supervisor_field_get_mf_node(root_children_field, 0);
  WbFieldRef contact_props_field = wb_supervisor_node_get_field(world_node, "contactProperties");
  int contactPropertyCount = wb_supervisor_field_get_count(contact_props_field);
  for (int i = 0; i < contactPropertyCount; i++)
  {
    WbNodeRef contact_props_node = wb_supervisor_field_get_mf_node(contact_props_field, i);
    WbFieldRef name_field = wb_supervisor_node_get_field(contact_props_node, "material2");
    if (strcmp(wb_supervisor_field_get_sf_string(name_field), "soft") == 0)
    {
      cf_soft_field = wb_supervisor_node_get_field(contact_props_node, "coulombFriction");
    }
    else if (strcmp(wb_supervisor_field_get_sf_string(name_field), "medium") == 0)
    {
      cf_medium_field = wb_supervisor_node_get_field(contact_props_node, "coulombFriction");
    }
    else if (strcmp(wb_supervisor_field_get_sf_string(name_field), "hard") == 0)
    {
      cf_hard_field = wb_supervisor_node_get_field(contact_props_node, "coulombFriction");
    }
  }

  wb_supervisor_field_set_mf_float(cf_soft_field, 0, cf_soft);
  wb_supervisor_field_set_mf_float(cf_medium_field, 0, cf_medium);
  wb_supervisor_field_set_mf_float(cf_hard_field, 0, cf_hard);

  // 10 ms steps
  while (wb_robot_step(TIME_STEP) != -1)
  {
    // handle timings
    if (started)
      current_lap_time++;
    double distance = wb_distance_sensor_get_value(distance_sensor);
    if (!started && distance < DETECTION_LIMIT)
    {
      started = true;
      for (int i = 0; i < 5; i++)
      {
        wb_led_set(led_front[i], 0);
        wb_led_set(led_back[i], 0);
      }
    }
    if (distance < DETECTION_LIMIT && previous_distance >= DETECTION_LIMIT && current_lap_time > 100)
    {
      laptimes[2] = laptimes[1];
      laptimes[1] = laptimes[0];
      laptimes[0] = current_lap_time;
      current_lap++;
      current_lap_time = 0;
      if (current_lap > 5)
        current_lap = 5;
      for (int i = 0; i < current_lap; i++)
      {
        wb_led_set(led_front[i], 1);
        wb_led_set(led_back[i], 1);
      }
    }
    display_timings(display_front);
    display_timings(display_back);

    // handle friction
    cf_soft = constrain(cf_soft - cf_soft_delta, 0.1, 1);
    cf_medium = constrain(cf_medium - cf_medium_delta, 0.11, 1);
    cf_hard = constrain(cf_hard - cf_hard_delta, 0.12, 1);
    wb_supervisor_field_set_mf_float(cf_soft_field, 0, cf_soft);
    wb_supervisor_field_set_mf_float(cf_medium_field, 0, cf_medium);
    wb_supervisor_field_set_mf_float(cf_hard_field, 0, cf_hard);
  };

  wb_robot_cleanup();

  return 0;
}
