#pragma once

#define TIME_STEP 10

#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

class Competitor {
 private:
  string team_name{""};
  string robot_name{""};
  string country{""};
  string car[3]{"", "", ""};
  int points[3]{0, 0, 0};
  double laps[3]{0.0, 0.0, 0.0};
  double times[3]{999.9, 999.9, 999.9};
  double best_laps[3]{999.9, 999.9, 999.9};

  double current_laps[5]{0.0, 0.0, 0.0, 0.0, 0.0};

 public:
  Competitor(string car_quali, string car_sprint, string car_feature);
  int* getPoints();
  void setPoints(int race_type, int points_for_race);
  double* getLaps();
  double* getTimes();
  double* getBestLaps();
  string getTeamName();
  string getRobotName();
  string getCountry();
  string getResult(int current_track);
  string getPointsStages();
  string getPointsTotal();
  string getString(int current_track);
  string getNameString();
  string getCar(int current_track);
  void setNames(string team_name, string robot_name, string coutnry);
  double* getCurrentLaps();
};
