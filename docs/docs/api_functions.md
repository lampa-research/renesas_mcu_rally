# API functions

## void init()

Initializes the virtual Renesas MCU controller.

## void update()

Updates sensor values, should be called in main controller while loop.

## void handle(int angle)

Servo steering operation.

- `angle`: Servo operation angle: -90 (left) to 90 (right)

## void motor(int back_right, int back_left, int front_right, int front_left)

Motor speed control. 0 is stopped, 100 is maximum voltage forward, and -100 is maximum voltage, but in reverse.

- `back_right`: Back right motor

- `back_left`: Back left motor

- `front_right`: Front right motor

- `front_left`: Front left motor

## unsigned short *line_sensor()

Returns the values of the line sensors.

- `returns`: Sensor values as an unsigned short array.

Example that prints raw sensor values:

```c
unsigned short *sensor = line_sensor();
for (int i = 0; i < 8; i++)
{
    printf("%d ", sensor[i]);
}
printf("\n");
```

## double *encoders()

Returns the motor speeds (in rad/s). An array of 4 values is returned: `{back_right, back_left, front_right, front_left}`, where:

- `back_right`: Back right motor velocity

- `back_left`: Back left motor velocity

- `front_right`: Front right motor velocity

- `front_left`: Front left motor velocity

## double *imu()

Returns the roll, pitch, and yaw angles of an inertial measurement unit, attached to the main platform. An array of 3 values is returned: `{roll, pitch, yaw}`.

## double time()

Returns the current time in seconds since the start of the controller program.

## Car parameters

The car parameters are accessible via the following functions.

```c
double get_track();
double get_wheel_base();
double get_sensor_base();
double get_gear_ratio();
const char* get_tyre_type();
double get_tyre_width();
double get_tyre_radius();
int get_number_of_sensors();
double get_distance_between_sensors();
double get_weight_penalty();
```
