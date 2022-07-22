#include <competitor.h>

Competitor::Competitor(string car_quali, string car_sprint, string car_feature) {
  car[0] = car_quali;
  car[1] = car_sprint;
  car[2] = car_feature;
}

int* Competitor::getPoints() { return points; }

void Competitor::setPoints(int race_type, int points_for_race) { points[race_type] = points_for_race; }

double* Competitor::getLaps() { return laps; }

double* Competitor::getTimes() { return times; }

double* Competitor::getBestLaps() { return best_laps; }

string Competitor::getTeamName() { return team_name; }

string Competitor::getRobotName() { return robot_name; }

string Competitor::getCountry() { return country; }

string Competitor::getResult(int current_track) {
  stringstream ss;
  ss << std::fixed << std::setprecision(2) << right << setw(3) << laps[current_track] << ": " << setw(5) << times[current_track] << " (" << setw(5) << best_laps[current_track] << ")" << endl;
  return ss.str();
}

string Competitor::getPointsStages() {
  stringstream ss;
  ss << std::fixed << right << std::setprecision(0) << setw(2) << points[0] << "/" << points[1] << "/" << points[2] << endl;
  return ss.str();
}

string Competitor::getPointsTotal() {
  stringstream ss;
  ss << std::fixed << right << std::setprecision(0) << setw(2) << points[0] + points[1] + points[2] << endl;
  return ss.str();
}

string Competitor::getString(int current_track) {
  stringstream ss;
  ss << std::fixed << std::setprecision(2) << left << setw(13) << team_name << " " << setw(13) << robot_name << " " << right << setw(4) << country << " " << setw(5) << laps[current_track] << " " << std::setprecision(2) << setw(11) << times[current_track] << " " << setw(9) << best_laps[current_track] << " " << std::setprecision(0) << setw(3) << points[0] << " " << setw(3) << points[1] << " " << setw(3) << points[2] << " " << setw(6) << points[0] + points[1] + points[2] << endl;
  return ss.str();
}

string Competitor::getNameString() {
  stringstream ss;
  ss << team_name << ": " << robot_name << " (" << country << ")\n";
  return ss.str();
}

string Competitor::getCar(int current_track) { return car[current_track]; }

void Competitor::setNames(string team_name, string robot_name, string coutnry) {
  this->team_name = team_name;
  this->robot_name = robot_name;
  this->country = coutnry;
}

double* Competitor::getCurrentLaps() { return current_laps; }