#pragma once

#include <state.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <webots/Display.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/LED.hpp>
#include <webots/Supervisor.hpp>

#define TIME_STEP 10
#define DETECTION_LIMIT 970

using namespace webots;
using namespace std;

class Gate {
 private:
  double distance{0};
  double distance_previous{0};
  Supervisor *supervisor;
  Node *self;
  DistanceSensor *distance_sensor;
  Display *display_front;
  Display *display_back;
  LED *led_front[5];
  LED *led_back[5];
  void clearDisplay(Display *display);
  void showDisplay(Display *display, double current_lap_time, double *laptimes);

 public:
  Gate(Supervisor *supervisor);
  state getMode();
  int getLaps();
  bool isLapStarted();
  bool isLapFinished();
  void displayLap(double current_lap_time, double *laptimes);
  void clearDisplays();
  void setLEDs(int lap);
};