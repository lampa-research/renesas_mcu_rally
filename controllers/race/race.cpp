#include <competition.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <thread>
#include <webots/Supervisor.hpp>

#define WAYPOINT_DISTANCE 0.25

using namespace webots;

int main(int argc, char **argv) {
  Supervisor *supervisor = new Supervisor();
  TV tv(supervisor);
  Gate gate(supervisor);
  SafetyCar safety_car;
  Competition competition(supervisor);
  competition.setState(gate.getMode());
  competition.readCompetitorsFromFile("competition_cars.txt");
  competition.readTracksFromFile("competition_tracks.txt");
  competition.initFriction("soft", CF_SOFT_MAX);
  competition.initFriction("medium", CF_MEDIUM_MAX);
  competition.initFriction("hard", CF_HARD_MAX);

  Node *robot{nullptr};
  Node *track{nullptr};
  Field *trans_field{nullptr};
  const double *position{nullptr};
  Field *weight_field{nullptr};

  int current_lap{0};
  long competitor_waypoint{0};
  double competitor_waypoint_time{0.0};
  double cf_soft = CF_SOFT_MAX;
  double cf_medium = CF_MEDIUM_MAX;
  double cf_hard = CF_HARD_MAX;
  int update_counter{0};
  double current_progress{0.0};

  srand(time(0));

  while (supervisor->step(TIME_STEP) != -1) {
    competition.incrementCurrentTime(TIME_STEP);
    switch (competition.getState()) {
      case state::WELCOME:
        if (competition.stateIsInit()) {
          // load competitor list
          for (Competitor &c : *competition.getCompetitors()) {
            robot = competition.loadRobot(c.getCar(0));
            c.setNames(robot->getField("teamName")->getSFString(), robot->getField("robotName")->getSFString(), robot->getField("country")->getSFString());
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            competition.removeRobot(robot);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
          }
          // cout << "State: Welcome" << endl;
          gate.clearDisplays();
          track = competition.loadTrack();
          switch (competition.getCurrentTrackIndex()) {
            case 0:
              competition.shuffleCompetitors();
              competition.setLapsTotal(1);
              tv.showTitle("Renesas Virtual MCU Rally\nQualifying starting soon.");
              break;
            case 1:
              competition.setLapsTotal(3);
              tv.showTitle("Renesas Virtual MCU Rally\nSprint race starting soon.");
              break;
            case 2:
              competition.setLapsTotal(5);
              tv.showTitle("Renesas Virtual MCU Rally\nFeature race starting soon.");
              break;
          }
          tv.showStartList(*competition.getCompetitors());
        } else if (competition.getStateTime() < 30.0) {
          double start_position[3]{-2.0, 5.0, 5.0};
          double end_position[3]{2.0, 5.0, 5.0};
          double start_orientation[4]{-0.677, 0.677, 0.286, 2.584};
          double end_orientation[4]{-0.677, 0.677, 0.286, 2.584};
          tv.setCameraHelicopter(start_position, start_orientation, end_position, end_orientation, 30.0, competition.getStateTime());
          tv.showTimer(30 - (int)competition.getStateTime());
        } else if (competition.getStateTime() > 30.0) {
          tv.hideTitle();
          tv.hideTimer();
          tv.hideStartList();
          competition.setState(state::SPAWN_SAFETY_CAR);
        }
        break;
      case state::SPAWN_SAFETY_CAR:
        if (competition.stateIsInit()) {
          // cout << "State: Spawn Safety Car" << endl;
          competition.setFriction("soft", CF_SOFT_MAX);
          competition.setFriction("medium", CF_MEDIUM_MAX);
          competition.setFriction("hard", CF_HARD_MAX);
          safety_car.removePoints();
          robot = competition.loadRobot("MCUCarDefault.wbo");
          trans_field = robot->getField("translation");
          tv.showTitle("Safety car");
          tv.setCameraTracking(robot);
          competition.resetBestLap();
        } else {
          if (gate.isLapStarted()) {
            competition.setState(state::TRACK_SAFETY_CAR);
          }
        }
        break;
      case state::TRACK_SAFETY_CAR:
        if (competition.stateIsInit()) {
          // cout << "State: Track Safety Car" << endl;
        }
        competition.incrementLapTime(TIME_STEP);
        position = trans_field->getSFVec3f();
        if (safety_car.getPoints().size() == 0) {
          safety_car.addPoint(position[0], position[2]);
        } else {
          double distance = sqrt(pow(position[0] - safety_car.getPoints().back().first, 2) + pow(position[2] - safety_car.getPoints().back().second, 2));
          if (distance > WAYPOINT_DISTANCE) {
            safety_car.addPoint(position[0], position[2]);
          }
          if (gate.isLapFinished() && competition.getLapTime() > 1.0) {
            // cout << "Lap time: " << competition.getLapTime() << endl;
            competition.removeRobot(robot);
            competition.setState(state::PAUSE);
          }
        }
        if (competition.getStateTime() > 5.0) {
          tv.setCameraRandom(robot);
          competition.resetStateTime();
        }

        break;
      case state::PAUSE:
        if (competition.stateIsInit()) {
          // cout << "State: Pause" << endl;
          tv.hideTitle();
          tv.hideCompetitorData();
          tv.hideCurrentTime();
          tv.hideDiff();
          competition.sortCompetitorsSoFar();
          tv.showBoard(*competition.getCompetitors(), competition.getCurrentCompetitorIndex(), competition.getCurrentTrackIndex());
          if (competition.getCurrentCompetitorIndex() == (int)competition.getCompetitors()->size()) {
            // cout << "Sorting for next..." << endl;
            competition.sortCompetitorsForNext();
          }
        } else if (gate.getMode() == state::PRACTICE) {
        } else if (competition.getStateTime() > 10.0 && competition.getCurrentTrackIndex() < 3) {
          competition.resetCurrentLap();
          if (competition.getCurrentCompetitorIndex() < (int)(competition.getCompetitors()->size())) {
            tv.hideBoard();
            competition.setState(state::SPAWN_COMPETITOR);
          } else if (competition.getCurrentTrackIndex() < 2) {
            tv.hideBoard();
            competition.removeTrack(track);
            competition.setCurrentTrackIndex(competition.getCurrentTrackIndex() + 1);
            competition.setCurrentCompetitorIndex(0);
            supervisor->simulationSetMode(supervisor->SIMULATION_MODE_PAUSE);
            competition.setState(state::WELCOME);
          }
        }
        if (competition.getStateTime() > 30.0) {
          supervisor->simulationSetMode(supervisor->SIMULATION_MODE_PAUSE);
        }
        break;
      case state::SPAWN_COMPETITOR:
        if (competition.stateIsInit()) {
          // cout << "State: Spawn Competitor" << endl;
          robot = competition.loadRobot(competition.getCurrentCompetitor()->getCar(competition.getCurrentTrackIndex()));
          trans_field = robot->getField("translation");
          competition.getCurrentCompetitor()->setNames(robot->getField("teamName")->getSFString(), robot->getField("robotName")->getSFString(), robot->getField("country")->getSFString());
          // apply weight penalties
          weight_field = robot->getField("weightPenalty");
          weight_field->setSFFloat(0.0);
          if (competition.getCurrentTrackIndex() == 1) {
            weight_field->setSFFloat(clamp(competition.getCurrentCompetitorIndex() / 10.0, 0.0, 1.0));
          } else if (competition.getCurrentTrackIndex() == 2) {
            // cout << "1 Setting weight penalty to 1.0" << endl;
            weight_field->setSFFloat(1.0);
            if (competition.getCurrentCompetitorIndex() >= competition.numberOfCompetitors() - 10) {
              // cout << "2 Setting weight penalty to " << clamp((competition.numberOfCompetitors() - competition.getCurrentCompetitorIndex() - 1) / 10.0, 0.0, 1.0) << endl;
              weight_field->setSFFloat(clamp((competition.numberOfCompetitors() - competition.getCurrentCompetitorIndex() - 1) / 10.0, 0.0, 1.0));
            }
          }
          tv.showCompetitorData(*competition.getCompetitors(), competition.getCurrentCompetitorIndex());
        } else {
          if (gate.isLapStarted()) {
            competition.setState(state::TRACK_COMPETITOR);
          }
        }
        break;
      case state::TRACK_COMPETITOR:
        if (competition.stateIsInit()) {
          // cout << "State: Track Competitor" << endl;
          competitor_waypoint = 0;
          current_lap = 0;
          for (int i = 0; i < 5; i++) {
            competition.getCurrentCompetitor()->getCurrentLaps()[i] = 0.0;
          }
          cf_soft = CF_SOFT_MAX;
          cf_medium = CF_MEDIUM_MAX;
          cf_hard = CF_HARD_MAX;
          if (competition.getCurrentCompetitorIndex() > 0) {
            tv.showLeading(*competition.getCompetitors(), competition.getCurrentCompetitorIndex());
          }
        } else {
          competition.incrementLapTime(TIME_STEP);
          position = trans_field->getSFVec3f();
          double distance = sqrt(pow(position[0] - safety_car.getPoints()[competitor_waypoint].first, 2) + pow(position[2] - safety_car.getPoints()[competitor_waypoint].second, 2));
          if (distance < WAYPOINT_DISTANCE) {
            competitor_waypoint++;
            competition.addCurrentLapPoint();
            if (competition.getCurrentCompetitorIndex() > 0) {
              tv.showDiff(competition.getTimeDiff());
            }
            if (competitor_waypoint > safety_car.nbPoints() - 1) {
              competitor_waypoint = safety_car.nbPoints() - 1;
            } else {
              competitor_waypoint_time = competition.getStateTime();
            }
            if (competitor_waypoint == 2) {
              tv.setCameraMounted(robot);
            } else if (competition.getStateTime() > 5.0) {
              tv.setCameraRandom(robot);
              competition.resetStateTime();
            }
          }

          if (++update_counter == 10) {
            update_counter = 0;
            current_progress = current_lap + (double)competitor_waypoint / (double)safety_car.nbPoints();
            gate.displayLap(competition.getLapTime(), competition.getCurrentCompetitor()->getCurrentLaps());
          }
          if (current_lap == 0) {
            tv.showCurrentTime(current_progress, competition.getLapTime(), 0.0);
          } else {
            tv.showCurrentTime(current_progress, competition.getLapTime(), competition.getCurrentCompetitor()->getCurrentLaps()[0]);
          }

          cf_soft = clamp(cf_soft - CF_SOFT_DELTA * TIME_STEP / 1000.0, CF_SOFT_MIN, CF_SOFT_MAX);
          cf_medium = clamp(cf_medium - CF_MEDIUM_DELTA * TIME_STEP / 1000.0, CF_MEDIUM_MIN, CF_MEDIUM_MAX);
          cf_hard = clamp(cf_hard - CF_HARD_DELTA * TIME_STEP / 1000.0, CF_HARD_MIN, CF_HARD_MAX);
          competition.setFriction("soft", cf_soft);
          competition.setFriction("medium", cf_medium);
          competition.setFriction("hard", cf_hard);

          if (gate.isLapFinished() && competition.getLapTime() > 1.0) {
            for (int i{4}; i >= 1; i--) {
              competition.getCurrentCompetitor()->getCurrentLaps()[i] = competition.getCurrentCompetitor()->getCurrentLaps()[i - 1];
            }
            competition.getCurrentCompetitor()->getCurrentLaps()[0] = competition.getLapTime();
            competition.checkBestLap();
            current_lap++;
            competition.setLapTime(0.0);
            competition.resetCurrentLap();
            competitor_waypoint = 0;

            if (current_lap < 5) {
              for (int i{0}; i < current_lap; i++) {
                gate.setLEDs(i);
              }
            }

            if (current_lap >= competition.getLapsTotal()) {
              double total_time = 0.0;
              double best_lap = 999.99;
              for (int i{0}; i < competition.getLapsTotal(); i++) {
                total_time += competition.getCurrentCompetitor()->getCurrentLaps()[i];
                if (competition.getCurrentCompetitor()->getCurrentLaps()[i] < best_lap) {
                  best_lap = competition.getCurrentCompetitor()->getCurrentLaps()[i];
                }
              }
              current_lap = competition.getLapsTotal();
              competition.getCurrentCompetitor()->getLaps()[competition.getCurrentTrackIndex()] = current_lap;
              competition.getCurrentCompetitor()->getBestLaps()[competition.getCurrentTrackIndex()] = best_lap;
              competition.getCurrentCompetitor()->getTimes()[competition.getCurrentTrackIndex()] = total_time;
              competition.removeRobot(robot);
              competition.incrementCurrentCompetitor();
              tv.hideLeading();
              competition.setState(state::PAUSE);
            }
          }

          // if missed waypoint
          if (competition.getStateTime() - competitor_waypoint_time > 3.0) {
            double total_time = 0.0;
            double best_lap = 999.99;
            for (int i{0}; i < (int)current_progress; i++) {
              total_time += competition.getCurrentCompetitor()->getCurrentLaps()[i];
              if (competition.getCurrentCompetitor()->getCurrentLaps()[i] < best_lap) {
                best_lap = competition.getCurrentCompetitor()->getCurrentLaps()[i];
              }
            }
            competition.getCurrentCompetitor()->getLaps()[competition.getCurrentTrackIndex()] = current_progress;
            competition.getCurrentCompetitor()->getBestLaps()[competition.getCurrentTrackIndex()] = best_lap;
            competition.getCurrentCompetitor()->getTimes()[competition.getCurrentTrackIndex()] = total_time;
            competition.removeRobot(robot);
            competition.incrementCurrentCompetitor();
            tv.hideLeading();
            competition.setState(state::PAUSE);
          }
        }
        break;
      case state::PRACTICE:
        if (competition.stateIsInit()) {
          competition.setLapsTotal(gate.getLaps());
          competition.getCompetitors()->clear();
          Competitor c("", "", "");
          robot = supervisor->getFromDef("MCUCar");
          c.setNames(robot->getField("teamName")->getSFString(), robot->getField("robotName")->getSFString(), robot->getField("country")->getSFString());
          competition.getCompetitors()->push_back(c);
        } else {
          if (gate.isLapStarted()) {
            competition.setState(state::PRACTICE_RUN);
          }
        }
        break;
      case state::PRACTICE_RUN:
        if (competition.stateIsInit()) {
        } else {
          competition.incrementLapTime(TIME_STEP);
          if (++update_counter == 10) {
            update_counter = 0;
            current_progress = current_lap;
            gate.displayLap(competition.getLapTime(), competition.getCurrentCompetitor()->getCurrentLaps());
            if (current_lap == 0) {
              tv.showCurrentTime(current_progress, competition.getLapTime(), 0.0);
            } else {
              tv.showCurrentTime(current_progress, competition.getLapTime(), competition.getCurrentCompetitor()->getCurrentLaps()[0]);
            }
          }

          cf_soft = clamp(cf_soft - CF_SOFT_DELTA * TIME_STEP / 1000.0, CF_SOFT_MIN, CF_SOFT_MAX);
          cf_medium = clamp(cf_medium - CF_MEDIUM_DELTA * TIME_STEP / 1000.0, CF_MEDIUM_MIN, CF_MEDIUM_MAX);
          cf_hard = clamp(cf_hard - CF_HARD_DELTA * TIME_STEP / 1000.0, CF_HARD_MIN, CF_HARD_MAX);
          competition.setFriction("soft", cf_soft);
          competition.setFriction("medium", cf_medium);
          competition.setFriction("hard", cf_hard);

          if (gate.isLapFinished() && competition.getLapTime() > 1.0) {
            for (int i{4}; i >= 1; i--) {
              competition.getCurrentCompetitor()->getCurrentLaps()[i] = competition.getCurrentCompetitor()->getCurrentLaps()[i - 1];
            }
            competition.getCurrentCompetitor()->getCurrentLaps()[0] = competition.getLapTime();
            competition.checkBestLap();
            current_lap++;
            competition.setLapTime(0.0);
            competition.resetCurrentLap();
            competitor_waypoint = 0;

            if (current_lap < 5) {
              for (int i{0}; i < current_lap; i++) {
                gate.setLEDs(i);
              }
            }

            if (current_lap >= competition.getLapsTotal()) {
              double total_time = 0.0;
              double best_lap = 999.99;
              for (int i{0}; i < competition.getLapsTotal(); i++) {
                total_time += competition.getCurrentCompetitor()->getCurrentLaps()[i];
                if (competition.getCurrentCompetitor()->getCurrentLaps()[i] < best_lap) {
                  best_lap = competition.getCurrentCompetitor()->getCurrentLaps()[i];
                }
              }
              current_lap = competition.getLapsTotal();
              competition.getCurrentCompetitor()->getLaps()[competition.getCurrentTrackIndex()] = current_lap;
              competition.getCurrentCompetitor()->getBestLaps()[competition.getCurrentTrackIndex()] = best_lap;
              competition.getCurrentCompetitor()->getTimes()[competition.getCurrentTrackIndex()] = total_time;
              competition.setCurrentCompetitorIndex(1);
              competition.setState(state::PAUSE);
            }
          }
        }
        break;
    }
  };

  delete supervisor;
  return 0;
}
