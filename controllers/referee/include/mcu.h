#ifndef MCU_H_
#define MCU_H_

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <webots/supervisor.h>
#include <webots/display.h>

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define TIME_STEP 10
#define DETECTION_LIMIT 970
#define WAYPOINT_DISTANCE 0.25

#define CF_SOFT_MAX 0.70
#define CF_SOFT_MIN 0.15
#define CF_SOFT_DELTA 0.0035 // per second

#define CF_MEDIUM_MAX 0.60
#define CF_MEDIUM_MIN 0.25
#define CF_MEDIUM_DELTA 0.0035

#define CF_HARD_MAX 0.40
#define CF_HARD_MIN 0.30
#define CF_HARD_DELTA 0.0035

typedef intptr_t ssize_t;

struct board_data_t
{
    char team_name[30];
    char robot_name[30];
    char car[3][30];

    double laps[3];
    double times[3];
    double best_laps[3];

    int points[3];
};

WbNodeRef mcu_load_robot(char *robot_file);
void mcu_remove_robot(WbNodeRef robot_node);
void mcu_set_weight(WbNodeRef robot_node, double weight);
WbNodeRef mcu_load_track(char *track_file);
void mcu_remove_track(WbNodeRef track_node);
int mcu_competitor_nb();
void mcu_competitor_list(struct board_data_t *board_data);
void mcu_track_list(char list[3][50]);
void mcu_display_init(WbDeviceTag display);
void mcu_display_clear(WbDeviceTag display);
void mcu_display_timings(WbDeviceTag display, long current_lap_time, double *laptimes);
void mcu_init_friction(char *name, double value);
void mcu_set_friction(char *name, double value);
void mcu_camera_mounted();
void mcu_camera_tracking();
void mcu_camera_pan_tilt(double waypoint[2]);
void mcu_leaderboard_display_current(WbNodeRef robot_node, char *data);
void mcu_leaderboard_display_leaderboard(struct board_data_t *sorted, int competitor_current, int current_track);
int compare_qualy(const void *a, const void *b);
int compare_reverse(const void *a, const void *b);
int compare_sprint(const void *a, const void *b);
int compare_feature(const void *a, const void *b);
int compare_points(const void *a, const void *b);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#endif
