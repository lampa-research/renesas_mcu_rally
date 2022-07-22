#pragma once

#include <competitor.h>
#include <gate.h>
#include <safety_car.h>
#include <state.h>
#include <tv.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#define CF_SOFT_MAX 0.70
#define CF_SOFT_MIN 0.15
#define CF_SOFT_DELTA 0.0035  // per second

#define CF_MEDIUM_MAX 0.60
#define CF_MEDIUM_MIN 0.25
#define CF_MEDIUM_DELTA 0.0035

#define CF_HARD_MAX 0.40
#define CF_HARD_MIN 0.30
#define CF_HARD_DELTA 0.0035

using namespace webots;
using namespace std;

class Competition {
 private:
  Supervisor *supervisor;
  vector<Competitor> competitors;
  int current_competitor{0};
  array<string, 3> tracks;
  Field *field_soft;
  Field *field_medium;
  Field *field_hard;
  double current_time{0.0};
  state current_state{state::WELCOME};
  double current_state_time{0.0};
  bool current_state_init{true};
  Node *track_node;
  int current_track{0};
  int laps_total{1};
  Node *robot_node;
  Field *trans_field;

  double current_lap_time{0.0};
  vector<double> best_lap_vector;
  double best_lap_time{999.9};
  vector<double> current_lap_vector;
  double time_diff{0.0};

 public:
  Competition(Supervisor *supervisor);
  ~Competition();
  void readCompetitorsFromFile(string file_name);
  void sortCompetitorsSoFar();
  void sortCompetitorsForNext();
  vector<Competitor> *getCompetitors();
  int getCurrentCompetitorIndex();
  void setCurrentCompetitorIndex(int index);
  Competitor *getCurrentCompetitor();
  void incrementCurrentCompetitor();
  int numberOfCompetitors();
  void shuffleCompetitors();

  void readTracksFromFile(string file_name);
  Node *loadTrack();
  void removeTrack(Node *track);
  void setCurrentTrackIndex(int track);
  int getCurrentTrackIndex();

  void initFriction(string name, double value);
  void setFriction(string name, double value);

  void incrementCurrentTime(long time);
  double getStateTime();
  void resetStateTime();
  state getState();
  void setState(state state);
  bool stateIsInit();

  void setLapsTotal(int laps);
  int getLapsTotal();

  Node *loadRobot(string name);
  void removeRobot(Node *robot);

  void incrementLapTime(long time);
  double getLapTime();
  void setLapTime(double time);

  void addCurrentLapPoint();
  double getTimeDiff();
  void checkBestLap();
  void resetCurrentLap();
  void resetBestLap();
};
