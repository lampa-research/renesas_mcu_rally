#include <safety_car.h>

vector<pair<double, double>> SafetyCar::getPoints() { return safety_car_points; }

void SafetyCar::addPoint(double x, double y) { safety_car_points.push_back(make_pair(x, y)); }

long SafetyCar::nbPoints() { return safety_car_points.size(); }

void SafetyCar::removePoints() { safety_car_points.clear(); }