# Line following

The basic functionality of a Rensas MCU Rally Car is line following. A simple line following program will consist of the following:

- Reading raw sensor data.
- Calculating the position of the line with respect to the sensor.
- Devising a control algorithm to make the robot follow the line.

Let us first look at each of these functionalities before merging them together into a working program.

## Reading raw sensor data

Line sensor data needs to be read within every step of the program, therefore, it needs to be within the `while` loop of the controller. The following two lines need to be called to access line sensor data.

```c
update();
unsigned short *sensor = line_sensor();
```

`update()` updates all sensor values (including encoders and line sensors), while `line_sensor()` API function returns an array of raw sensor data.

Line sensors are simulated as an array of infrared reflectance sensors. Each sensor measures the amount of reflected light, bouncing off of the surface. In the simulated setup, a higher number corresponds to a darker surface. The raw sensor data takes the form of `unsigned short` values since each sensor value can be between 0 and 1024. Typically, the value of sensors on a black surface will be around 500, while the value of the sensors on a white line will be around 200, but this depends on many external factors. Individual sensor values can be accessed using arrays, e.g. `sensor[0]`, `sensor[1]`, etc.

## Calculating the position of the line

Detecting the position of the line below the sensor can be done in many ways. For example, one can simply look for the sensor with the lowerst value. A more robust approach is to calculate the weighted average of the raw sensor data. Below is a calculation for 8 sensors, using the standard weighted average formula.

Note that for 8 sensors the `weighted_sum / sum` ratio will be between 0 and 7, therefore `3.5` is subtracted to make sure that the position of the line is negative if the line is on one side of the centre, and positive on the other.

Another important note is that the following code averages raw sensor data, therefore, the result will be the average position of black (not white) surface under the sensor.

```c
float line = 0, sum = 0, weighted_sum = 0;
for (int i = 0; i < 8; i++)
{
  weighted_sum += sensor[i] * i;
  sum += sensor[i];
}
line = weighted_sum / sum - 3.5;
```

## Devising a control algorithm

A very simple control algorithm would be as follows:

- move forward by applying a constant voltage to the electromotors,
- turn the handle with the servomotor to follow the line (the further away the line, the bigger the turn).

This control algorithm can be written in 2 lines of code using `motor` and `handle` function of the API.

```c
motor(40, 40, 40, 40);
handle(1000 * line);
```

## The final program

Copy-paste the following program into your controller code and test how it works.

```c
#include "renesas_api.h"

int main(int argc, char **argv)
{
  wb_robot_init();
  init();

  while (wb_robot_step(TIME_STEP) != -1)
  {
    update();
    unsigned short *sensor = line_sensor();
    float line = 0, sum = 0, weighted_sum = 0;
    for (int i = 0; i < 8; i++)
    {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
    }
    line = weighted_sum / sum - 3.5;
    motor(40, 40, 40, 40);
    handle(1000 * line);
  }
  wb_robot_cleanup();
  return 0;
}
```
