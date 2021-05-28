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
WbNodeRef track_node = NULL;
WbFieldRef trans_field = NULL;
int current_track = 0;

// timing
double distance_gate = 1000.0;
double distance_previous = 1000.0;
long current_time = 0;
double laptimes[5] = {0, 0, 0, 0, 0};
int current_lap = 0;
long current_lap_time = 0;
int update_counter = 0;

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
int competitor_current = 0;
int laps_total = 5;
char tracks[3][50];

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

  // get number of laps
  WbFieldRef laps_field = wb_supervisor_node_get_field(self_node, "laps");
  laps_total = wb_supervisor_field_get_sf_int32(laps_field);
  laps_total = constrain(laps_total, 1, 5);

  // get competitors and tracks from file
  competitor_nb = mcu_competitor_nb();
  mcu_track_list(tracks);

  // initialize board data
  struct board_data_t *board_data = malloc(20 * sizeof(struct board_data_t));
  for (int i = 0; i < competitor_nb; i++)
  {
    strcpy(board_data[i].team_name, "");
    strcpy(board_data[i].robot_name, "");
  }
  mcu_competitor_list(board_data);

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
        track_node = mcu_load_track(tracks[current_track]);
        switch (current_track)
        {
        case 0:
          laps_total = 1;
          wb_supervisor_set_label(0, "Renesas Virtual MCU Rally\nQualifying starting soon.", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
          wb_supervisor_set_label(1, "Renesas Virtual MCU Rally\nQualifying starting soon.", 0, 0, 0.1, 0xffffff, 0, "Arial Black");
          break;
        case 1:
          wb_supervisor_set_label(0, "Renesas Virtual MCU Rally\nSprint race starting soon.", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
          wb_supervisor_set_label(1, "Renesas Virtual MCU Rally\nSprint race starting soon.", 0, 0, 0.1, 0xffffff, 0, "Arial Black");
          laps_total = 3;
          break;
        case 2:
          wb_supervisor_set_label(0, "Renesas Virtual MCU Rally\nFeature race starting soon.", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
          wb_supervisor_set_label(1, "Renesas Virtual MCU Rally\nFeature race starting soon.", 0, 0, 0.1, 0xffffff, 0, "Arial Black");
          laps_total = 5;
          break;
        }
        competition_state_init = false;
      }
      if (current_time / 1000.0 - competition_state_init_time > 3.0)
      {
        wb_supervisor_set_label(0, "", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
        wb_supervisor_set_label(1, "", 0, 0, 0.1, 0xffffff, 0, "Arial Black");
        competition_state = SPAWN_SAFETY_CAR;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case SPAWN_SAFETY_CAR:
      if (competition_state_init)
      {
        competition_state_init = false;
        // init friction
        mcu_init_friction("soft", cf_soft);
        mcu_init_friction("medium", cf_medium);
        mcu_init_friction("hard", cf_hard);
        robot_node = mcu_load_robot("MCUCarDefault.wbo");
        trans_field = wb_supervisor_node_get_field(robot_node, "translation");
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
        safety_car_waypoint_nb = 0;
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
        mcu_remove_robot(robot_node);
        competition_state = PAUSE;
        competition_state_init = false;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case PAUSE:
      if (competition_state_init)
      {
        // sort
        switch (current_track)
        {
        case 0:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_qualy);
          break;
        case 1:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_sprint);
          break;
        case 2:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_feature);
          break;
        }
        // score points
        int scoring[54] = {25, 18, 15, 12, 10, 8, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < competitor_nb; i++)
        {
          board_data[i].points[current_track] = scoring[i + 4 - 2 * current_track];
        }

        mcu_leaderboard_display_leaderboard(board_data, competitor_current, current_track);

        // sort for next
        switch (current_track)
        {
        case 0:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_reverse);
          break;
        case 1:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_qualy);
          break;
        case 2:
          qsort(board_data, competitor_current, sizeof board_data[0], compare_points);
          break;
        }

        competition_state_init = false;
      }
      if (current_time / 1000.0 - competition_state_init_time > 5.0)
      {
        if (competitor_current < competitor_nb)
        {
          wb_supervisor_set_label(4, "", 0.021, 0.101, 0.06, 0x000000, 0, "Lucida Console");
          wb_supervisor_set_label(5, "", 0.02, 0.1, 0.06, 0xffffff, 0, "Lucida Console");
          competition_state = SPAWN_COMPETITOR;
          competition_state_init = true;
          competition_state_init_time = current_time / 1000.0;
        }
        // round finished
        else if (current_track < 2)
        {
          wb_supervisor_set_label(4, "", 0.021, 0.101, 0.06, 0x000000, 0, "Lucida Console");
          wb_supervisor_set_label(5, "", 0.02, 0.1, 0.06, 0xffffff, 0, "Lucida Console");
          mcu_remove_track(track_node);
          current_track++;
          // reload competitor cars, controllers
          competitor_current = 0;
          competition_state = WELCOME;
          competition_state_init = true;
          competition_state_init_time = current_time / 1000.0;
        }
      }
      break;
    case SPAWN_COMPETITOR:
      if (competition_state_init)
      {
        competition_state_init = false;
        robot_node = mcu_load_robot(board_data[competitor_current].car[current_track]);
        trans_field = wb_supervisor_node_get_field(robot_node, "translation");
        WbFieldRef team_name_field = wb_supervisor_node_get_field(robot_node, "teamName");
        strcpy(board_data[competitor_current].team_name, wb_supervisor_field_get_sf_string(team_name_field));
        WbFieldRef robot_name_field = wb_supervisor_node_get_field(robot_node, "robotName");
        strcpy(board_data[competitor_current].robot_name, wb_supervisor_field_get_sf_string(robot_name_field));
        WbFieldRef weight_field = wb_supervisor_node_get_field(robot_node, "weightPenalty");

        // apply weight penalties
        if (current_track != 0 && competitor_current != 0)
        {
          double weight_penalty = competitor_current / 10.0;
          if (weight_penalty > 1.0)
            weight_penalty = 1.0;
          wb_supervisor_field_set_sf_float(weight_field, weight_penalty);
        }
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
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.1 * safety_car_waypoint_nb)]);
        }
        else if (competitor_waypoint_nb == (int)(0.2 * safety_car_waypoint_nb))
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.3 * safety_car_waypoint_nb)]);
        }
        else if (competitor_waypoint_nb == (int)(0.4 * safety_car_waypoint_nb))
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.5 * safety_car_waypoint_nb)]);
        }
        else if (competitor_waypoint_nb == (int)(0.6 * safety_car_waypoint_nb))
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.6 * safety_car_waypoint_nb)]);
        }
        else if (competitor_waypoint_nb == (int)(0.8 * safety_car_waypoint_nb))
        {
          mcu_camera_pan_tilt(safety_car_waypoints[(int)(0.9 * safety_car_waypoint_nb)]);
        }
      }

      if (++update_counter == 10)
      {
        update_counter = 0;
        char str[100];
        if (current_lap == 0)
        {
          sprintf(str, "lap: %6.1f\nlap time: %6.1f\n", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0);
        }
        else
        {
          sprintf(str, "lap: %6.1f\nlap time: %6.1f\nprevious: %6.2f", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0, laptimes[0]);
        }
        mcu_leaderboard_display_current(robot_node, str);
        mcu_display_timings(display_front, current_lap_time, laptimes);
        mcu_display_timings(display_back, current_lap_time, laptimes);
      }

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

        if (current_lap >= laps_total)
        {
          // leaderboard data
          double total_time = 0.0;
          double best_lap = 9999.99;
          for (int i = 0; i < laps_total; i++)
          {
            total_time += laptimes[i];
            if (laptimes[i] < best_lap)
              best_lap = laptimes[i];
          }
          current_lap = laps_total;
          board_data[competitor_current].laps[current_track] = current_lap;
          board_data[competitor_current].times[current_track] = total_time;
          board_data[competitor_current].best_laps[current_track] = best_lap;
          // and spawn new contestant
          mcu_remove_robot(robot_node);
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
        board_data[competitor_current].laps[current_track] = current_lap + (double)competitor_waypoint_nb / (double)safety_car_waypoint_nb;
        board_data[competitor_current].times[current_track] = total_time;
        board_data[competitor_current].best_laps[current_track] = best_lap;
        mcu_remove_robot(robot_node);
        competitor_current++;
        competition_state = PAUSE;
        competition_state_init = true;
        competition_state_init_time = current_time / 1000.0;
      }
      break;
    case PRACTICE:
      if (competition_state_init)
      {
        competitor_nb = 1;
        current_track = 2;
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
          robot_node = wb_supervisor_node_get_from_def("MCUCar");
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

          if (current_lap >= laps_total)
          {
            // leaderboard data
            double total_time = 0.0;
            double best_lap = 9999.99;
            for (int i = 0; i < laps_total; i++)
            {
              total_time += laptimes[i];
              if (laptimes[i] < best_lap)
                best_lap = laptimes[i];
            }
            current_lap = laps_total;
            board_data[competitor_current].laps[current_track] = current_lap;
            board_data[competitor_current].times[current_track] = total_time;
            board_data[competitor_current].best_laps[current_track] = best_lap;

            WbFieldRef team_name_field = wb_supervisor_node_get_field(robot_node, "teamName");
            strcpy(board_data[competitor_current].team_name, wb_supervisor_field_get_sf_string(team_name_field));
            WbFieldRef robot_name_field = wb_supervisor_node_get_field(robot_node, "robotName");
            strcpy(board_data[competitor_current].robot_name, wb_supervisor_field_get_sf_string(robot_name_field));

            competitor_nb = 1;
            competitor_current++;
            competition_state = PAUSE;
            competition_state_init = true;
            competition_state_init_time = current_time / 1000.0;
          }
        }

        if (++update_counter == 10)
        {
          update_counter = 0;
          char str[100];
          if (current_lap == 0)
          {
            sprintf(str, "lap: %6.1f\nlap time: %6.1f\n", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0);
          }
          else
          {
            sprintf(str, "lap: %6.1f\nlap time: %6.1f\nprevious: %6.2f", current_lap + (double)competitor_waypoint_nb / ((double)safety_car_waypoint_nb + 1.0), current_lap_time / 1000.0, laptimes[0]);
          }
          mcu_leaderboard_display_current(robot_node, str);
          mcu_display_timings(display_front, current_lap_time, laptimes);
          mcu_display_timings(display_back, current_lap_time, laptimes);
        }

        cf_soft = constrain(cf_soft - CF_SOFT_DELTA * TIME_STEP / 1000.0, CF_SOFT_MIN, CF_SOFT_MAX);
        cf_medium = constrain(cf_medium - CF_MEDIUM_DELTA * TIME_STEP / 1000.0, CF_MEDIUM_MIN, CF_MEDIUM_MAX);
        cf_hard = constrain(cf_hard - CF_HARD_DELTA * TIME_STEP / 1000.0, CF_HARD_MIN, CF_HARD_MAX);
        mcu_set_friction("soft", cf_soft);
        mcu_set_friction("medium", cf_medium);
        mcu_set_friction("hard", cf_hard);
      }
      break;
    }
  };

  wb_robot_cleanup();

  return 0;
}
