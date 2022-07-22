#pragma once

#include <competitor.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>
#include <webots/Supervisor.hpp>

using namespace webots;
using namespace std;

class TV {
 private:
  Supervisor *supervisor;
  random_device randomDevice;
  mt19937 generator;
  uniform_real_distribution<double> uniformPositive;
  uniform_real_distribution<double> uniformPositiveAndNegative;
  
 public:
  TV(Supervisor *supervisor);
  void showTitle(string text);
  void hideTitle();
  void showTimer(int seconds);
  void hideTimer();
  void showDiff(double diff);
  void hideDiff();
  void showCompetitorData(vector<Competitor> competitors, int current_competitor);
  void hideCompetitorData();
  void showCurrentTime(double lap, double lap_time, double previous_lap_time);
  void hideCurrentTime();
  void showBoard(vector<Competitor> competitors, int current_competitor, int current_track);
  void hideBoard();
  void showStartList(vector<Competitor> competitors);
  void hideStartList();
  void showLeading(vector<Competitor> competitors, int current_competitor);
  void hideLeading();
  void setCameraTracking(Node *robot_node);
  void setCameraMounted(Node *robot_node);
  void setCameraTV(Node *robot_node);
  void setCameraRandom(Node *robot_node);
  void setCameraHelicopter(double *start_position, double *start_orientation, double *end_position, double *end_orientation, double total_time, double time);
};
