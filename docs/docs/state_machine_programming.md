# State machine programming

Line following is just one of the functionalities that the Renesas MCU Rally robot needs in order to complete a track as there are three other types of obstacles:

- 90 degree turns (marked by two white stripes across the track),
- left and right lane changes (marked by white stripes across the corresponding half of the track), and
- elevation changes (unmarked).

A structured way of solving these additional problems is to introduce _state machine_ programming. The robot's behaviour can then be described as a set of states and transitions between them. For example, the robot starts in a *FOLLOW_LINE* state, in which it follows the line, but then switches to a *MAKE_90_DEGREE_TURN* state when it detects the white stripes that mark the turn.

## Simple state machines in C

A rudimentary way of programming state machines in C is to use an `enum` as a data structure and `switch` statements. The following code introduces an `enum` variable `state`, and can be extended to an arbitrary number of named states.

```c
enum states_t
{
  FOLLOW_LINE,
  MAKE_90_DEGREE_TURN
} state = FOLLOW_LINE;
```

Then, in the `while` loop of the main program, the robot's behaviour in each of the states and the state transitions need to be specified.

```c
switch (state)
{
    case FOLLOW_LINE:
        // write the line following code
        // write the white stripe detection code
        // if white stripes detected, state = MAKE_90_DEGREE_TURN
    break;
    case MAKE_90_DEGREE_TURN:
        // follow line, but very slowly (for example)
        // detect when the turn was made
        // if turn made, state = FOLLOW_LINE
    break;
}
```

## Example program

The following example program uses the state machine programming principle and represents a good basis for controller development. The program works with the default MCUCar robot with 8 line sensors. Feel free to use it as a starting point. It should, however, be studied, modified, and improved.

```c
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
} state = FOLLOW;
double last_time = 0.0;
int left_change_pending = 0;
int right_change_pending = 0;

int main(int argc, char **argv)
{
  wb_robot_init();
  init();

  while (wb_robot_step(TIME_STEP) != -1)
  {
    update();
    unsigned short *sensor = line_sensor();
    float line = 0, sum = 0, weighted_sum = 0;
    bool double_line = 0;
    bool left_change = 0;
    bool right_change = 0;
    for (int i = 0; i < 8; i++)
    {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
      if (sensor[i] < 400) // white under sensor, used for detecting white stripe markings
      {
        double_line++;
        if (i < 4) // count white under sensor on the left side
        {
          left_change++;
        }
        else // and the right side
        {
          right_change++;
        }
      }
    }
    line = weighted_sum / sum - 3.5;

    switch (state)
    {
    case FOLLOW:
      motor(40, 40, 40, 40);
      handle(1000 * line);
      if (double_line > 5)
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
      motor(10, 10, 10, 10);
      handle(1000 * line);
      if (time() - last_time > 0.2 && (left_change > 3 || right_change > 3))
      {
        last_time = time();
        state = CORNER_OUT;
      }
      break;
    case CORNER_OUT:
      motor(10, 10, 10, 10);
      handle(1000 * line);
      if (time() - last_time > 0.75)
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
      if (time() - last_time > 2.5)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case RIGHT_TURN:
      motor(20, 20, 20, 20);
      if (time() - last_time < 0.45)
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
      if (time() - last_time > 2.5)
      {
        last_time = time();
        state = FOLLOW;
      }
      break;
    case LEFT_TURN:
      motor(20, 20, 20, 20);
      if (time() - last_time < 0.45)
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

  wb_robot_cleanup();

  return 0;
}

```
