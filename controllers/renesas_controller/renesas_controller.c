#include "renesas_api.h"

enum states_t
{
  FOLLOW,
  CORNER_IN,
  CORNER_OUT,
  RIGHT_CHANGE_DETECTED,
  RIGHT_TURN,
  LEFT_CHANGE_DETECTED,
  LEFT_TURN
} states;
int state = FOLLOW;
double last_time = 0.0;
int left_change_pending = 0;
int right_change_pending = 0;

int main(int argc, char **argv)
{
  wb_robot_init(); // this call is required for WeBots initialisation
  init();          // initialises the renesas MCU controller

  while (wb_robot_step(TIME_STEP) != -1)
  {
    update();
    printf("%d ", state);
    unsigned short *sensor = line_sensor();
    float line = 0, sum = 0, weighted_sum = 0;
    bool double_line = 0;
    bool left_change = 0;
    bool right_change = 0;
    for (int i = 0; i < 8; i++)
    {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
      if (sensor[i] < 400)
      {
        double_line++;
        if (i < 4)
        {
          left_change++;
        }
        else
        {
          right_change++;
        }
      }
      printf("%d ", sensor[i]);
    }
    printf("\n");
    line = weighted_sum / sum - 3.5;

    switch (state)
    {
    case FOLLOW:
      motor(40, 40, 40, 40);
      handle(1000 * line);
      if (double_line > 6)
      {
        last_time = time();
        state = CORNER_IN;
      }
      else if (time() - last_time > 0.2 && right_change > 3)
      {
        last_time = time();
        state = RIGHT_CHANGE_DETECTED;
      }
      else if (time() - last_time > 0.2 && left_change > 3)
      {
        last_time = time();
        state = LEFT_CHANGE_DETECTED;
      }
      break;
    case CORNER_IN:
      motor(20, 20, 20, 20);
      handle(1000 * line);
      if (time() - last_time > 0.2 && (left_change > 3 || right_change > 3))
      {
        last_time = time();
        state = CORNER_OUT;
      }
      break;
    case CORNER_OUT:
      motor(20, 20, 20, 20);
      handle(1000 * line);
      if (time() - last_time > 0.5)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case RIGHT_CHANGE_DETECTED:
      motor(20, 20, 20, 20);
      handle(100 * line);
      if (double_line == 0)
      {
        last_time = time();
        state = RIGHT_TURN;
      }
      if (time() - last_time > 1.5)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case RIGHT_TURN:
      motor(20, 20, 20, 20);
      if (time() - last_time < 0.4)
        handle(-25);
      else
        handle(5);
      if (double_line > 2)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case LEFT_CHANGE_DETECTED:
      motor(20, 20, 20, 20);
      handle(100 * line);
      if (double_line == 0)
      {
        last_time = time();
        state = LEFT_TURN;
      }
      if (time() - last_time > 1.5)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case LEFT_TURN:
      motor(20, 20, 20, 20);
      if (time() - last_time < 0.4)
        handle(25);
      else
        handle(-5);
      if (double_line > 2)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    }
  };

  wb_robot_cleanup(); // this call is required for WeBots cleanup

  return 0;
}
