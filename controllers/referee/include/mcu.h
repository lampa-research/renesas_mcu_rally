#ifndef MCU_H_
#define MCU_H_

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <webots/supervisor.h>
#include <webots/display.h>

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define TIME_STEP 10
#define DETECTION_LIMIT 970
#define WAYPOINT_DISTANCE 0.25

#define CF_SOFT_MAX 0.55
#define CF_SOFT_MIN 0.15
#define CF_SOFT_DELTA 0.005 // per second

#define CF_MEDIUM_MAX 0.45
#define CF_MEDIUM_MIN 0.20
#define CF_MEDIUM_DELTA 0.003

#define CF_HARD_MAX 0.35
#define CF_HARD_MIN 0.25
#define CF_HARD_DELTA 0.001

struct board_data_t
{
    char team_name[50];
    char robot_name[50];
    double laps;
    double total_time;
    double best_lap;
};

WbNodeRef mcu_load_robot(char *robot_file);
void mcu_remove_robot();
int mcu_competitor_nb();
void mcu_competitor_list(char list[50][50]);
void mcu_display_init(WbDeviceTag display);
void mcu_display_clear(WbDeviceTag display);
void mcu_display_timings(WbDeviceTag display, long current_lap_time, double *laptimes);
void mcu_init_friction(char *name, double value);
void mcu_set_friction(char *name, double value);
void mcu_camera_mounted();
void mcu_camera_tracking();
void mcu_camera_pan_tilt(double waypoint[2]);
void mcu_leaderboard_display_current(WbNodeRef robot_node, char *data);
void mcu_leaderboard_display_leaderboard(int competitor_current, char team_names[50][50], char robot_names[50][50], double laps[50], double times[50], double best_laps[50]);
int compare(const void *a, const void *b);

#endif
