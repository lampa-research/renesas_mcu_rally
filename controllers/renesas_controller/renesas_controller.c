#include "renesas.h"

/* Masked value settings X:masked (disabled) O:not masked (enabled) */
#define MASK2_2 0x66 /* X O O X  X O O X            */
#define MASK2_0 0x60 /* X O O X  X X X X            */
#define MASK0_2 0x06 /* X X X X  X O O X            */
#define MASK3_3 0xe7 /* O O O X  X O O O            */
#define MASK0_3 0x07 /* X X X X  X O O O            */
#define MASK3_0 0xe0 /* O O O X  X X X X            */
#define MASK4_0 0xf0 /* O O O O  X X X X            */
#define MASK0_4 0x0f /* X X X X  O O O O            */
#define MASK4_4 0xff /* O O O O  O O O O            */

/*======================================*/
/* Prototype declarations               */
/*======================================*/
void init(void);
unsigned char sensor_inp(unsigned char mask);
int check_crossline(void);
int check_rightline(void);
int check_leftline(void);

/*======================================*/
/* Global variable declarations         */
/*======================================*/
// int pattern = 11;
// unsigned long cnt0 = 0;
// unsigned long cnt1 = 0;

int main(int argc, char **argv)
{
  wb_robot_init();    // this call is required for WeBots initialisation
  renesas_mcu_init(); // initialises the renesas MCU controller

  while (wb_robot_step(TIME_STEP) != -1)
  {
    // cnt0 += TIME_STEP;
    // cnt1 += TIME_STEP;
    // update_sensors();

    // printf("%d %x\n", pattern, sensor_inp(MASK3_3));

    // switch (pattern)
    // {

    //   /****************************************************************
    //   Pattern-related
    //     0: wait for switch input
    //     1: check if start bar is open
    //   11: normal trace
    //   12: check end of large turn to right
    //   13: check end of large turn to left
    //   21: processing at 1st cross line
    //   22: read but ignore 2nd time
    //   23: trace, crank detection after cross line
    //   31: left crank clearing processing ? wait until stable
    //   32: left crank clearing processing ? check end of turn
    //   41: right crank clearing processing ? wait until stable
    //   42: right crank clearing processing ? check end of turn
    //   51: processing at 1st right half line detection
    //   52: read but ignore 2nd line
    //   53: trace after right half line detection
    //   54: right lane change end check
    //   61: processing at 1st left half line detection
    //   62: read but ignore 2nd line
    //   63: trace after left half line detection
    //   64: left lane change end check
    //   ****************************************************************/

    // case 11:
    //   /* Normal trace */
    //   if (check_crossline())
    //   { /* Cross line check            */
    //     pattern = 21;
    //     break;
    //   }
    //   if (check_rightline())
    //   { /* Right half line detection check */
    //     pattern = 51;
    //     break;
    //   }
    //   if (check_leftline())
    //   { /* Left half line detection check */
    //     pattern = 61;
    //     break;
    //   }
    //   switch (sensor_inp(MASK3_3))
    //   {
    //   case 0x00:
    //     /* Center -> straight */
    //     handle(0);
    //     motor(20, 20);
    //     break;

    //   case 0x04:
    //     /* Slight amount left of center -> slight turn to right */
    //     handle(10);
    //     motor(20, 20);
    //     break;

    //   case 0x06:
    //     /* Small amount left of center -> small turn to right */
    //     handle(30);
    //     motor(20, 20);
    //     break;

    //   case 0x07:
    //     /* Medium amount left of center -> medium turn to right */
    //     handle(45);
    //     motor(20, 20);
    //     break;

    //   case 0x03:
    //     /* Large amount left of center -> large turn to right */
    //     handle(60);
    //     motor(10, 10);
    //     pattern = 12;
    //     break;

    //   case 0x20:
    //     /* Slight amount right of center -> slight turn to left */
    //     handle(-10);
    //     motor(20, 20);
    //     break;

    //   case 0x60:
    //     /* Small amount right of center -> small turn to left */
    //     handle(-30);
    //     motor(20, 20);
    //     break;

    //   case 0xe0:
    //     /* Medium amount right of center -> medium turn to left */
    //     handle(-45);
    //     motor(20, 20);
    //     break;

    //   case 0xc0:
    //     /* Large amount right of center -> large turn to left */
    //     handle(-60);
    //     motor(10, 10);
    //     pattern = 13;
    //     break;
    //   default:
    //     motor(10, 10);
    //     break;
    //   }
    //   break;

    // case 12:
    //   /* Check end of large turn to right */
    //   if (check_crossline())
    //   { /* Cross line check during large turn */
    //     pattern = 21;
    //     break;
    //   }
    //   if (check_rightline())
    //   { /* Right half line detection check */
    //     pattern = 51;
    //     break;
    //   }
    //   if (check_leftline())
    //   { /* Left half line detection check */
    //     pattern = 61;
    //     break;
    //   }
    //   if (sensor_inp(MASK3_3) == 0x06)
    //   {
    //     pattern = 11;
    //   }
    //   break;

    // case 13:
    //   /* Check end of large turn to left */
    //   if (check_crossline())
    //   { /* Cross line check during large turn */
    //     pattern = 21;
    //     break;
    //   }
    //   if (check_rightline())
    //   { /* Right half line detection check */
    //     pattern = 51;
    //     break;
    //   }
    //   if (check_leftline())
    //   { /* Left half line detection check */
    //     pattern = 61;
    //     break;
    //   }
    //   if (sensor_inp(MASK3_3) == 0x60)
    //   {
    //     pattern = 11;
    //   }
    //   break;

    // case 21:
    //   /* Processing at 1st cross line */
    //   handle(0);
    //   motor(0, 0);
    //   pattern = 22;
    //   cnt1 = 0;
    //   break;

    // case 22:
    //   /* Read but ignore 2nd line */
    //   if (cnt1 > 100)
    //   {
    //     pattern = 23;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 23:
    //   /* Trace, crank detection after cross line */
    //   if (sensor_inp(MASK4_4) == 0xf8)
    //   {
    //     /* Left crank determined -> to left crank clearing processing */
    //     handle(-38);
    //     motor(10, 10);
    //     pattern = 31;
    //     cnt1 = 0;
    //     break;
    //   }
    //   if (sensor_inp(MASK4_4) == 0x1f)
    //   {
    //     /* Right crank determined -> to right crank clearing processing */
    //     handle(38);
    //     motor(10, 10);
    //     pattern = 41;
    //     cnt1 = 0;
    //     break;
    //   }
    //   switch (sensor_inp(MASK3_3))
    //   {
    //   case 0x00:
    //     /* Center -> straight */
    //     handle(0);
    //     motor(40, 40);
    //     break;
    //   case 0x04:
    //   case 0x06:
    //   case 0x07:
    //   case 0x03:
    //     /* Left of center -> turn to right */
    //     handle(8);
    //     motor(20, 20);
    //     break;
    //   case 0x20:
    //   case 0x60:
    //   case 0xe0:
    //   case 0xc0:
    //     /* Right of center -> turn to left */
    //     handle(-8);
    //     motor(20, 20);
    //     break;
    //   }
    //   break;

    // case 31:
    //   /* Left crank clearing processing ? wait until stable */
    //   if (cnt1 > 200)
    //   {
    //     pattern = 32;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 32:
    //   /* Left crank clearing processing ? check end of turn */
    //   if (sensor_inp(MASK3_3) == 0x60)
    //   {
    //     pattern = 11;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 41:
    //   /* Right crank clearing processing ? wait until stable */
    //   if (cnt1 > 200)
    //   {
    //     pattern = 42;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 42:
    //   /* Right crank clearing processing ? check end of turn */
    //   if (sensor_inp(MASK3_3) == 0x06)
    //   {
    //     pattern = 11;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 51:
    //   /* Processing at 1st right half line detection */
    //   handle(0);
    //   motor(0, 0);
    //   pattern = 52;
    //   cnt1 = 0;
    //   break;

    // case 52:
    //   /* Read but ignore 2nd time */
    //   if (cnt1 > 100)
    //   {
    //     pattern = 53;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 53:
    //   /* Trace, lane change after right half line detection */
    //   if (sensor_inp(MASK4_4) == 0x00)
    //   {
    //     handle(15);
    //     motor(20, 20);
    //     pattern = 54;
    //     cnt1 = 0;
    //     break;
    //   }
    //   switch (sensor_inp(MASK3_3))
    //   {
    //   case 0x00:
    //     /* Center -> straight */
    //     handle(0);
    //     motor(20, 20);
    //     break;
    //   case 0x04:
    //   case 0x06:
    //   case 0x07:
    //   case 0x03:
    //     /* Left of center -> turn to right */
    //     handle(8);
    //     motor(20, 20);
    //     break;
    //   case 0x20:
    //   case 0x60:
    //   case 0xe0:
    //   case 0xc0:
    //     /* Right of center -> turn to left */
    //     handle(-8);
    //     motor(20, 20);
    //     break;
    //   default:
    //     break;
    //   }
    //   break;

    // case 54:
    //   /* Right lane change end check */
    //   if (sensor_inp(MASK4_4) == 0x3c)
    //   {
    //     pattern = 11;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 61:
    //   /* Processing at 1st left half line detection */
    //   handle(0);
    //   motor(0, 0);
    //   pattern = 62;
    //   cnt1 = 0;
    //   break;

    // case 62:
    //   /* Read but ignore 2nd time */
    //   if (cnt1 > 100)
    //   {
    //     pattern = 63;
    //     cnt1 = 0;
    //   }
    //   break;

    // case 63:
    //   /* Trace, lane change after left half line detection */
    //   if (sensor_inp(MASK4_4) == 0x00)
    //   {
    //     handle(-15);
    //     motor(20, 20);
    //     pattern = 64;
    //     cnt1 = 0;
    //     break;
    //   }
    //   switch (sensor_inp(MASK3_3))
    //   {
    //   case 0x00:
    //     /* Center -> straight */
    //     handle(0);
    //     motor(20, 20);
    //     break;
    //   case 0x04:
    //   case 0x06:
    //   case 0x07:
    //   case 0x03:
    //     /* Left of center -> turn to right */
    //     handle(8);
    //     motor(20, 20);
    //     break;
    //   case 0x20:
    //   case 0x60:
    //   case 0xe0:
    //   case 0xc0:
    //     /* Right of center -> turn to left */
    //     handle(-8);
    //     motor(20, 20);
    //     break;
    //   default:
    //     break;
    //   }
    //   break;

    // case 64:
    //   /* Left lane change end check */
    //   if (sensor_inp(MASK4_4) == 0x3c)
    //   {
    //     pattern = 11;
    //     cnt1 = 0;
    //   }
    //   break;

    // default:
    //   /* If neither, return to standby state */
    //   pattern = 11;
    //   break;
    // }
    update_sensors();
    unsigned short *sensor = line_sensor();
    float line, sum = 0, weighted_sum = 0;
    for (int i = 0; i < N_SENSORS; i++)
    {
      weighted_sum += sensor[i] * i;
      sum += sensor[i];
    }
    line = weighted_sum / sum - 3.5;

    motor2(60, 60, 60, 60);
    handle(300 * line);
  };

  wb_robot_cleanup(); // this call is required for WeBots cleanup

  return 0;
}

/***********************************************************************/
/* Sensor state detection                                              */
/* Arguments:       masked values                                      */
/* Return values:   sensor value                                       */
/***********************************************************************/
int sensor_calibration[] = {400, 400, 400, 400, 400, 400, 400, 400};

unsigned char sensor_inp(unsigned char mask)
{
  unsigned short *sensors = line_sensor();
  unsigned char sensor = 0;

  for (int i = 0; i < N_SENSORS; i++)
  {
    sensor += ((sensors[i] < sensor_calibration[i] ? 1 : 0) << i);
  }

  sensor &= mask;

  return sensor;
}

/***********************************************************************/
/* Cross line detection processing                                     */
/* Return values: 0: no cross line, 1: cross line                      */
/***********************************************************************/
int check_crossline(void)
{
  unsigned char b;
  int ret;

  ret = 0;
  b = sensor_inp(MASK3_3);
  if (b == 0xe7)
  {
    ret = 1;
  }
  return ret;
}

/***********************************************************************/
/* Right half line detection processing                                */
/* Return values: 0: not detected, 1: detected                         */
/***********************************************************************/
int check_rightline(void)
{
  unsigned char b;
  int ret;

  ret = 0;
  b = sensor_inp(MASK4_4);
  if (b == 0x1f)
  {
    ret = 1;
  }
  return ret;
}

/***********************************************************************/
/* Left half line detection processing                                 */
/* Return values: 0: not detected, 1: detected                         */
/***********************************************************************/
int check_leftline(void)
{
  unsigned char b;
  int ret;

  ret = 0;
  b = sensor_inp(MASK4_4);
  if (b == 0xf8)
  {
    ret = 1;
  }
  return ret;
}