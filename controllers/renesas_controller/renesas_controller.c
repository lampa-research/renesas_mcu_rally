#include "renesas_api.h"

int main(int argc, char **argv)
{
  wb_robot_init(); // this call is required for WeBots initialisation
  init();          // initialises the renesas MCU controller

  while (wb_robot_step(TIME_STEP) != -1)
  {
    update();
    unsigned short *sensor = line_sensor();
    float line, sum = 0, weighted_sum = 0;
    for (int i = 0; i < 8; i++)
    {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
    }
    line = weighted_sum / sum - 3.5;

    motor(50, 50, 50, 50);
    handle(300 * line);
  };

  wb_robot_cleanup(); // this call is required for WeBots cleanup

  return 0;
}
