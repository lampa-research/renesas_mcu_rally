#pragma once

#include <vector>

using namespace std;

class SafetyCar {
 private:
  vector<pair<double, double>> safety_car_points;

 public:
  vector<pair<double, double>> getPoints();
  void addPoint(double x, double y);
  long nbPoints();
  void removePoints();
};
