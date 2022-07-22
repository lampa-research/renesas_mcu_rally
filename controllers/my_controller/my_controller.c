#include <stdlib.h>

#include "renesas_api.h"

#define UPRAMP 80.0
#define FAST (40 + rand() % 20)
#define MEDIUM 20.0
#define SLOW 10.0
#define BRAKE -60.0

#define STRICT 1500.0
#define LOOSE 25.0

#define DETECTED_NB 2
#define RAMP_UP_ANGLE 0.009
#define RAMP_DOWN_ANGLE 0.018

enum states_t { FOLLOW, CORNER_IN, CORNER_OUT, RIGHT_CHANGE_DETECTED, RIGHT_TURN, LEFT_CHANGE_DETECTED, LEFT_TURN, RAMP_UP, RAMP_DOWN } state = FOLLOW;

enum detected_t { LEFT_CHANGE, RIGHT_CHANGE, CORNER, NOLINE, ANYLINE };

enum detected_t detected_current = NOLINE;
enum detected_t detected_last = NOLINE;
int detected_number = 0;

double last_time = 0.0;

int main(int argc, char **argv) {
  wb_robot_init();  // this call is required for WeBots initialisation
  init();           // initialises the renesas MCU controller

  while (wb_robot_step(TIME_STEP) != -1) {
    update();
    unsigned short *sensor = line_sensor();
    double *angles = imu();
    float line = 0, sum = 0, weighted_sum = 0;
    int double_line = 0;
    int left_change = 0;
    int right_change = 0;
    for (int i = 0; i < 8; i++) {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
      if (sensor[i] < 500) {
        double_line++;
        if (i < 4)
          left_change++;
        else
          right_change++;
      }
    }
    line = weighted_sum / sum - 3.5;
    if (double_line > 6)
      detected_current = CORNER;
    else if (left_change > 3)
      detected_current = LEFT_CHANGE;
    else if (right_change > 3)
      detected_current = RIGHT_CHANGE;
    else if (double_line == 0)
      detected_current = NOLINE;
    else
      detected_current = ANYLINE;

    if (detected_current == detected_last)
      detected_number++;
    else
      detected_number = 0;

    switch (state) {
      case FOLLOW:
        motor(FAST - fabs(100 * line), FAST - fabs(100 * line), FAST - fabs(100 * line), FAST - fabs(100 * line));
        handle(STRICT * line);
        if (angles[1] > RAMP_UP_ANGLE) {
          last_time = time();
          state = RAMP_UP;
          printf("RAMP UP\n");
        } else if (angles[1] < -RAMP_DOWN_ANGLE) {
          last_time = time();
          state = RAMP_DOWN;
          printf("RAMP DOWN\n");
        } else if (detected_current == CORNER && detected_number >= DETECTED_NB) {
          last_time = time();
          state = CORNER_IN;
          printf("CORNER IN\n");
        }

        else if (time() - last_time > 0.2 && detected_current == RIGHT_CHANGE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = RIGHT_CHANGE_DETECTED;
          printf("RIGHT_CHANGE_DETECTED\n");
        } else if (time() - last_time > 0.2 && detected_current == LEFT_CHANGE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = LEFT_CHANGE_DETECTED;
          printf("LEFT_CHANGE_DETECTED\n");
        }
        break;
      case CORNER_IN:
        if (time() - last_time < 0.1)
          motor(BRAKE, BRAKE, BRAKE, BRAKE);
        else
          motor(SLOW, SLOW, SLOW, SLOW);

        if (angles[1] > RAMP_UP_ANGLE) {
          last_time = time();
          state = RAMP_UP;
          printf("RAMP UP\n");
        } else if (angles[1] < -RAMP_DOWN_ANGLE) {
          last_time = time();
          state = RAMP_DOWN;
          printf("RAMP DOWN\n");
        }

        handle(LOOSE * line);
        if (time() - last_time > 0.3 && (detected_current == LEFT_CHANGE || detected_current == RIGHT_CHANGE) && detected_number >= DETECTED_NB) {
          last_time = time();
          state = CORNER_OUT;
          printf("CORNER OUT\n");
        }
        break;
      case CORNER_OUT:
        motor(MEDIUM, MEDIUM, MEDIUM, MEDIUM);
        handle(STRICT * line);
        if (time() - last_time > 0.70) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case RIGHT_CHANGE_DETECTED:
        motor(MEDIUM, MEDIUM, MEDIUM, MEDIUM);
        handle(LOOSE * line);
        if (detected_current == NOLINE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = RIGHT_TURN;
          printf("RIGHT_TURN\n");
        }
        if (time() - last_time > 2.5) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case RIGHT_TURN:
        motor(MEDIUM, MEDIUM, MEDIUM, MEDIUM);
        if (time() - last_time < 0.65)
          handle(-20);
        else if (time() - last_time < 0.9)
          handle(20);
        else if (detected_current == ANYLINE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case LEFT_CHANGE_DETECTED:
        motor(MEDIUM, MEDIUM, MEDIUM, MEDIUM);
        handle(LOOSE * line);
        if (detected_current == NOLINE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = LEFT_TURN;
          printf("LEFT TURN\n");
        }
        if (time() - last_time > 2.5) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case LEFT_TURN:
        motor(MEDIUM, MEDIUM, MEDIUM, MEDIUM);
        if (time() - last_time < 0.65)
          handle(20);
        else if (time() - last_time < 0.9)
          handle(-20);
        else if (detected_current == ANYLINE && detected_number >= DETECTED_NB) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case RAMP_UP:
        motor(UPRAMP, UPRAMP, UPRAMP, UPRAMP);
        handle(STRICT * line);
        if (angles[1] < RAMP_UP_ANGLE) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
      case RAMP_DOWN:
        motor(100 * angles[1], 100 * angles[1], 100 * angles[1], 100 * angles[1]);
        handle(STRICT * line);
        if (angles[1] > -RAMP_DOWN_ANGLE) {
          last_time = time();
          state = FOLLOW;
          printf("FOLLOW\n");
        }
        break;
    }

    detected_last = detected_current;
  };

  wb_robot_cleanup();  // this call is required for WeBots cleanup

  return 0;
}
