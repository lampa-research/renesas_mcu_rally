#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <webots/robot.h>
#include <webots/supervisor.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>

#include "mcu.h"

// competition state machine
enum states_t
{
  WELCOME,
  SPAWN_SAFETY_CAR,
  TRACK_SAFETY_CAR,
  PAUSE,
  SPAWN_COMPETITOR,
  TRACK_COMPETITOR,
  PRACTICE
} competition_state = WELCOME;
double competition_state_init_time = 0.0;
bool competition_state_init = true;
WbNodeRef robot_node = NULL;
WbFieldRef trans_field = NULL;

// timing
double distance_gate = 1000.0;
double distance_previous = 1000.0;
long current_time = 0;
double laptimes[5] = {0, 0, 0, 0, 0};
int current_lap = 0;
long current_lap_time = 0;

// safety car
bool started = false;
bool safety_car = true;
long safety_car_counter = 0;
int safety_car_waypoint_nb = 0;
double safety_car_waypoints[2048][2];

// competition
int competitor_waypoint_nb = 0;
double competitor_position[2];
double competitor_waypoint_time = 0.0;
int competitor_nb = 0;
char competitors[50][50];
int competitor_current = 0;
char team_names[50][50];
char robot_names[50][50];
double laps[50];
double times[50];
double best_laps[50];

// friction coefficients
float cf_soft = CF_SOFT_MAX;
float cf_medium = CF_MEDIUM_MAX;
float cf_hard = CF_HARD_MAX;

int main(int argc, char **argv)
{
  wb_robot_init();
  srand(time(NULL));

  // get mode
  WbNodeRef self_node = wb_supervisor_node_get_self();
  WbFieldRef mode_field = wb_supervisor_node_get_field(self_node, "mode");
  const char *mode_value = wb_supervisor_field_get_sf_string(mode_field);
  if (strcmp(mode_value, "practice") == 0)
  {
    competition_state = PRACTICE;
  }

  // get competitors from file
  competitor_nb = mcu_competitor_nb();
  mcu_competitor_list(competitors);

  // init devices
  WbDeviceTag distance_sensor = wb_robot_get_device("distance_sensor");
  wb_distance_sensor_enable(distance_sensor, TIME_STEP);
  WbDeviceTag display_front = wb_robot_get_device("display_front");
  WbDeviceTag display_back = wb_robot_get_device("display_back");
  mcu_display_init(display_front);
  mcu_display_init(display_back);
  WbDeviceTag led_front[5];
  WbDeviceTag led_back[5];
  char name[20];
  for (int i = 0; i < 5; i++)
  {
    sprintf(name, "led_front_%d", i);
    led_front[i] = wb_robot_get_device(name);
    wb_led_set(led_front[i], 1);
    sprintf(name, "led_back_%d", i);
    led_back[i] = wb_robot_get_device(name);
    wb_led_set(led_back[i], 1);
  }

  // init friction
  mcu_init_friction("soft", cf_soft);
  mcu_init_friction("medium", cf_medium);
  mcu_init_friction("hard", cf_hard);

  while (wb_robot_step(TIME_STEP) != -1)
  {
    current_time += TIME_STEP;
    switch (competition_state)
    {
    case WELCOME:
      if (competition_state_init)
      {
        competition_state_init = false;
        wb_supervisor_set_label(1, "Welcome to the Renesas MCU Rally Virtual competition!", 0, 0, 0.1, 0x000000, 0, "Arial Black");
      }
      if (current_time / 1000.0 - competition_state_init_time > 3.0)
      {
        wb_supervisor_set_label(1, "", 0, 0, 0.1, 0x000000, 0, "Arial Black");
        competition_state = SPAWN_SAFETY_CAR;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case SPAWN_SAFETY_CAR:
      if (competition_state_init)
      {
        competition_state_init = false;
        robot_node = mcu_load_robot("MCUCarDefault.wbo");
        trans_field = wb_supervisor_node_get_proto_field(robot_node, "translation");
        safety_car_waypoints[0][0] = 0.0;
        safety_car_waypoints[0][1] = 0.0;
        mcu_leaderboard_display_current(robot_node, "");
        mcu_camera_tracking();
      }
      // wait for the start of the lap
      distance_gate = wb_distance_sensor_get_value(distance_sensor);
      if (distance_gate < DETECTION_LIMIT)
      {
        for (int i = 0; i < 5; i++)
        {
          wb_led_set(led_front[i], 0);
          wb_led_set(led_back[i], 0);
        }
        competition_state = TRACK_SAFETY_CAR;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;

    case TRACK_SAFETY_CAR:
      current_lap_time += TIME_STEP;
      // track and remember position
      const double *sc_position = wb_supervisor_field_get_sf_vec3f(trans_field);
      double current_position[2];
      current_position[0] = sc_position[0];
      current_position[1] = sc_position[2];
      double distance_scwp = sqrt(pow(safety_car_waypoints[safety_car_waypoint_nb][0] - current_position[0], 2) + pow(safety_car_waypoints[safety_car_waypoint_nb][1] - current_position[1], 2));
      if (distance_scwp > WAYPOINT_DISTANCE)
      {
        safety_car_waypoint_nb++;
        safety_car_waypoints[safety_car_waypoint_nb][0] = current_position[0];
        safety_car_waypoints[safety_car_waypoint_nb][1] = current_position[1];
      }
      distance_previous = distance_gate;
      distance_gate = wb_distance_sensor_get_value(distance_sensor);
      if (distance_gate < DETECTION_LIMIT && distance_previous >= DETECTION_LIMIT && current_lap_time > 1000)
      {
        mcu_remove_robot();
        competition_state = PAUSE;
        competition_state_init = false;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case PAUSE:
      if (competition_state_init)
      {
        mcu_leaderboard_display_leaderboard(competitor_current, team_names, robot_names, laps, times, best_laps);
        competition_state_init = false;
      }
      if (current_time / 1000.0 - competition_state_init_time > 5.0)
      {
        if (competitor_current < competitor_nb)
        {
          wb_supervisor_set_label(2, "", 0.1, 0.2, 0.05, 0x000000, 0, "Lucida Console");
          competition_state = SPAWN_COMPETITOR;
          competition_state_init = true;
          competition_state_init_time = current_time / 1000.0;
        }
      }
      break;
    case SPAWN_COMPETITOR:
      if (competition_state_init)
      {
        competition_state_init = false;
        robot_node = mcu_load_robot(competitors[competitor_current]);
        trans_field = wb_supervisor_node_get_proto_field(robot_node, "translation");
        WbFieldRef team_name_field = wb_supervisor_node_get_field(robot_node, "teamName");
        strcpy(team_names[competitor_current], wb_supervisor_field_get_sf_string(team_name_field));
        WbFieldRef robot_name_field = wb_supervisor_node_get_field(robot_node, "robotName");
        strcpy(robot_names[competitor_current], wb_supervisor_field_get_sf_string(robot_name_field));
      }
      // wait for the start of the lap
      distance_gate = wb_distance_sensor_get_value(distance_sensor);
      if (distance_gate < DETECTION_LIMIT)
      {
        for (int i = 0; i < 5; i++)
        {
          wb_led_set(led_front[i], 0);
          wb_led_set(led_back[i], 0);
        }
        competition_state = TRACK_COMPETITOR;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case TRACK_COMPETITOR:
      if (competition_state_init)
      {
        competition_state_init = false;
        competitor_waypoint_nb = 1;
        current_lap = 0;
        current_lap_time = 0;
        for (int i = 0; i < 5; i++)
        {
          laptimes[i] = 0.0;
        }
        cf_soft = CF_SOFT_MAX;
        cf_medium = CF_MEDIUM_MAX;
        cf_hard = CF_HARD_MAX;
      }
      current_lap_time += TIME_STEP;
      const double *position = wb_supervisor_field_get_sf_vec3f(trans_field);
      competitor_position[0] = position[0];
      competitor_position[1] = position[2];
      double distance = sqrt(pow(safety_car_waypoints[competitor_waypoint_nb][0] - competitor_position[0], 2) + pow(safety_car_waypoints[competitor_waypoint_nb][1] - competitor_position[1], 2));

      if (distance < WAYPOINT_DISTANCE)
      {
        competitor_waypoint_nb++;
        if (competitor_waypoint_nb > safety_car_waypoint_nb)
        {
          competitor_waypoint_nb = safety_car_waypoint_nb;
        }
        else
        {
          competitor_waypoint_time = current_time / 1000.0;
        }
        if (competitor_waypoint_nb == 2)
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.33 * safety_car_waypoint_nb)]);
        }
        else if (competitor_waypoint_nb == (int)(0.5 * safety_car_waypoint_nb))
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.67 * safety_car_waypoint_nb)]);
        }
      }

      char str[100];
      sprintf(str, "lap: %6.2f\nlap time: %6.2f", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0);
      mcu_leaderboard_display_current(robot_node, str);
      mcu_display_timings(display_front, current_lap_time, laptimes);
      mcu_display_timings(display_back, current_lap_time, laptimes);

      cf_soft = constrain(cf_soft - CF_SOFT_DELTA * TIME_STEP / 1000.0, CF_SOFT_MIN, CF_SOFT_MAX);
      cf_medium = constrain(cf_medium - CF_MEDIUM_DELTA * TIME_STEP / 1000.0, CF_MEDIUM_MIN, CF_MEDIUM_MAX);
      cf_hard = constrain(cf_hard - CF_HARD_DELTA * TIME_STEP / 1000.0, CF_HARD_MIN, CF_HARD_MAX);
      mcu_set_friction("soft", cf_soft);
      mcu_set_friction("medium", cf_medium);
      mcu_set_friction("hard", cf_hard);

      distance_previous = distance_gate;
      distance_gate = wb_distance_sensor_get_value(distance_sensor);
      if (distance_gate < DETECTION_LIMIT && distance_previous >= DETECTION_LIMIT && current_lap_time > 1000)
      {
        for (int i = 4; i >= 1; i--)
        {
          laptimes[i] = laptimes[i - 1];
        }
        laptimes[0] = current_lap_time / 1000.0;
        current_lap++;
        current_lap_time = 0;
        competitor_waypoint_nb = 1;

        if (current_lap < 5)
        {
          for (int i = 0; i < current_lap; i++)
          {
            wb_led_set(led_front[i], 1);
            wb_led_set(led_back[i], 1);
          }
        }

        if (current_lap > 4)
        {
          // leaderboard data
          double total_time = 0.0;
          double best_lap = 9999.99;
          for (int i = 0; i < 5; i++)
          {
            total_time += laptimes[i];
            if (laptimes[i] < best_lap)
              best_lap = laptimes[i];
          }
          current_lap = 5.0;
          laps[competitor_current] = current_lap;
          times[competitor_current] = total_time;
          best_laps[competitor_current] = best_lap;
          // and spawn new contestant
          mcu_remove_robot();
          competitor_current++;
          competition_state = PAUSE;
          competition_state_init = true;
          competition_state_init_time = current_time / 1000.0;
        }
      }

      // if missed waypoint
      if (current_time / 1000.0 - competitor_waypoint_time > 3.0)
      {
        // leaderboard data
        double total_time = 0.0;
        double best_lap = 9999.99;
        for (int i = 0; i < current_lap; i++)
        {
          total_time += laptimes[i];
          if (laptimes[i] < best_lap)
            best_lap = laptimes[i];
        }
        laps[competitor_current] = current_lap + (double)competitor_waypoint_nb / (double)safety_car_waypoint_nb;
        times[competitor_current] = total_time;
        best_laps[competitor_current] = best_lap;
        mcu_remove_robot();
        competitor_current++;
        competition_state = PAUSE;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case PRACTICE:
      if (competition_state_init)
      {
        // wait for the start of the lap
        distance_gate = wb_distance_sensor_get_value(distance_sensor);
        if (distance_gate < DETECTION_LIMIT)
        {
          for (int i = 0; i < 5; i++)
          {
            wb_led_set(led_front[i], 0);
            wb_led_set(led_back[i], 0);
          }
          competition_state_init = false;
          WbNodeRef root_node = wb_supervisor_node_get_root();
          WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
          robot_node = wb_supervisor_field_get_mf_node(root_children_field, -1);
        }
      }
      else
      {
        current_lap_time += TIME_STEP;
        distance_previous = distance_gate;
        distance_gate = wb_distance_sensor_get_value(distance_sensor);
        if (distance_gate < DETECTION_LIMIT && distance_previous >= DETECTION_LIMIT && current_lap_time > 1000)
        {
          for (int i = 4; i >= 1; i--)
          {
            laptimes[i] = laptimes[i - 1];
          }
          laptimes[0] = current_lap_time / 1000.0;
          current_lap++;
          current_lap_time = 0;
          if (current_lap < 5)
          {
            for (int i = 0; i < current_lap; i++)
            {
              wb_led_set(led_front[i], 1);
              wb_led_set(led_back[i], 1);
            }
          }

          if (current_lap > 4)
          {
            // leaderboard data
            double total_time = 0.0;
            double best_lap = 9999.99;
            for (int i = 0; i < 5; i++)
            {
              total_time += laptimes[i];
              if (laptimes[i] < best_lap)
                best_lap = laptimes[i];
            }
            current_lap = 5.0;
            laps[competitor_current] = current_lap;
            times[competitor_current] = total_time;
            best_laps[competitor_current] = best_lap;

            WbFieldRef team_name_field = wb_supervisor_node_get_field(robot_node, "teamName");
            strcpy(team_names[competitor_current], wb_supervisor_field_get_sf_string(team_name_field));
            WbFieldRef robot_name_field = wb_supervisor_node_get_field(robot_node, "robotName");
            strcpy(robot_names[competitor_current], wb_supervisor_field_get_sf_string(robot_name_field));

            competitor_nb = 1;
            competitor_current++;
            competition_state = PAUSE;
            competition_state_init = true;
            competition_state_init_time = current_time / 1000.0;
          }
        }
        char str[100];
        sprintf(str, "lap:      %6.2f\nlap time: %6.2f", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0);
        mcu_leaderboard_display_current(robot_node, str);
        mcu_display_timings(display_front, current_lap_time, laptimes);
        mcu_display_timings(display_back, current_lap_time, laptimes);

        // cf_soft = constrain(cf_soft - CF_SOFT_DELTA * TIME_STEP / 1000.0, CF_SOFT_MIN, CF_SOFT_MAX);
        // cf_medium = constrain(cf_medium - CF_MEDIUM_DELTA * TIME_STEP / 1000.0, CF_MEDIUM_MIN, CF_MEDIUM_MAX);
        // cf_hard = constrain(cf_hard - CF_HARD_DELTA * TIME_STEP / 1000.0, CF_HARD_MIN, CF_HARD_MAX);
        // mcu_set_friction("soft", cf_soft);
        // mcu_set_friction("medium", cf_medium);
        // mcu_set_friction("hard", cf_hard);
      }
      break;
    }
  };

  wb_robot_cleanup();

  return 0;
}
