#include <competition.h>

Competition::Competition(Supervisor *supervisor) : supervisor{supervisor} {}

Competition::~Competition() { delete supervisor; }

void Competition::readCompetitorsFromFile(string file_name) {
  ifstream file(file_name);
  int num_competitors;
  file >> num_competitors;
  for (int i{0}; i < num_competitors; ++i) {
    string car_quali, car_sprint, car_feature;
    file >> car_quali >> car_sprint >> car_feature;
    competitors.push_back(Competitor(car_quali, car_sprint, car_feature));
    // cout << "Competitor " << i << ": " << car_quali << " " << car_sprint << " " << car_feature << endl;
  }
  file.close();
}

bool compare_qualifying(Competitor i, Competitor j) {
  if (i.getLaps()[0] == j.getLaps()[0]) {
    if (i.getTimes()[0] == j.getTimes()[0]) {
      return i.getBestLaps()[0] < j.getBestLaps()[0];
    } else {
      return i.getTimes()[0] < j.getTimes()[0];
    }
  } else {
    return i.getLaps()[0] > j.getLaps()[0];
  }
}

bool compare_sprint(Competitor i, Competitor j) {
  if (i.getLaps()[1] == j.getLaps()[1]) {
    if (i.getTimes()[1] == j.getTimes()[1]) {
      return i.getBestLaps()[1] < j.getBestLaps()[1];
    } else {
      return i.getTimes()[1] < j.getTimes()[1];
    }
  } else {
    return i.getLaps()[1] > j.getLaps()[1];
  }
}

bool compare_feature(Competitor i, Competitor j) {
  if (i.getLaps()[2] == j.getLaps()[2]) {
    if (i.getTimes()[2] == j.getTimes()[2]) {
      return i.getBestLaps()[2] < j.getBestLaps()[2];
    } else {
      return i.getTimes()[2] < j.getTimes()[2];
    }
  } else {
    return i.getLaps()[2] > j.getLaps()[2];
  }
}

bool compare_reverse(Competitor i, Competitor j) {
  if (i.getLaps()[0] == j.getLaps()[0]) {
    if (i.getTimes()[0] == j.getTimes()[0]) {
      return i.getBestLaps()[0] > j.getBestLaps()[0];
    } else {
      return i.getTimes()[0] > j.getTimes()[0];
    }
  } else {
    return i.getLaps()[0] < j.getLaps()[0];
  }
}

bool compare_points(Competitor i, Competitor j) {
  if (i.getPoints()[0] + i.getPoints()[1] + i.getPoints()[2] == j.getPoints()[0] + j.getPoints()[1] + j.getPoints()[2]) {
    if (i.getPoints()[2] == j.getPoints()[2]) {
      if (i.getPoints()[1] == j.getPoints()[1]) {
        return i.getPoints()[0] < j.getPoints()[0];
      } else {
        return i.getPoints()[1] < j.getPoints()[1];
      }
    } else {
      return i.getPoints()[2] < j.getPoints()[2];
    }
  } else {
    return i.getPoints()[0] + i.getPoints()[1] + i.getPoints()[2] < j.getPoints()[0] + j.getPoints()[1] + j.getPoints()[2];
  }
}

void Competition::sortCompetitorsSoFar() {
  // cout << "Sorting competitors so far..." << endl;
  // for (auto &c : competitors) {
  //   cout << c.getString(current_track) << endl;
  // }
  // cout << endl;
  switch (current_track) {
    case 0:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_qualifying);
      break;
    case 1:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_sprint);
      break;
    case 2:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_feature);
      break;
  }

  // score points
  int scoring[54] = {25, 18, 15, 12, 10, 8, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int i{0}; i < (int)(competitors.size()); i++) {
    competitors[i].setPoints(current_track, scoring[i + 4 - 2 * current_track]);
  }
}

void Competition::sortCompetitorsForNext() {
  switch (current_track) {
    case 0:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_reverse);
      break;
    case 1:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_reverse);
      break;
    case 2:
      partial_sort(competitors.begin(), competitors.begin() + current_competitor, competitors.end(), compare_points);
      break;
    case 3:
      sort(competitors.begin(), competitors.end(), compare_points);
      break;
  }
}

vector<Competitor> *Competition::getCompetitors() { return &competitors; }

int Competition::getCurrentCompetitorIndex() { return current_competitor; }

void Competition::setCurrentCompetitorIndex(int index) { current_competitor = index; }

Competitor *Competition::getCurrentCompetitor() { return &(competitors[current_competitor]); }

void Competition::incrementCurrentCompetitor() { current_competitor++; }

int Competition::numberOfCompetitors() { return competitors.size(); }

void Competition::shuffleCompetitors() {
  // cout << "Shuffling competitors..." << endl;
  // for (int i{0}; i < (int)(competitors.size()); i++) {
  //   cout << competitors[i].getString(current_track) << endl;
  // }
  // cout << endl;
  random_shuffle(competitors.begin(), competitors.end());
  // cout << "Shuffled competitors..." << endl;
  // for (int i{0}; i < (int)(competitors.size()); i++) {
  //   cout << competitors[i].getString(current_track) << endl;
  // }
  // cout << endl;
}

void Competition::readTracksFromFile(string file_name) {
  ifstream file(file_name);
  for (int i{0}; i < 3; ++i) {
    string track_name;
    file >> track_name;
    tracks[i] = track_name;
    // cout << "Track " << i << ": " << track_name << endl;
  }
}

void Competition::initFriction(string name, double value) {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  Node *world = root_children->getMFNode(0);
  Field *contact_properties = world->getField("contactProperties");
  int num_properties = contact_properties->getCount();
  for (int i{0}; i < num_properties; ++i) {
    Node *contact_property = contact_properties->getMFNode(i);
    Field *contact_property_name = contact_property->getField("material2");
    if (contact_property_name->getSFString() == name) {
      Field *contact_property_friction = contact_property->getField("coulombFriction");
      contact_property_friction->setMFFloat(0, value);
      if (name == "soft") {
        field_soft = contact_property_friction;
      } else if (name == "medium") {
        field_medium = contact_property_friction;
      } else if (name == "hard") {
        field_hard = contact_property_friction;
      }
    }
  }
}

void Competition::setFriction(string name, double value) {
  if (name == "soft") {
    field_soft->setMFFloat(0, value);
  } else if (name == "medium") {
    field_medium->setMFFloat(0, value);
  } else if (name == "hard") {
    field_hard->setMFFloat(0, value);
  }
}

void Competition::incrementCurrentTime(long time) { current_time += time / 1000.0; }

double Competition::getStateTime() { return current_time - current_state_time; }

void Competition::resetStateTime() { current_state_time = current_time; }

state Competition::getState() { return current_state; }

void Competition::setState(state state) {
  current_state = state;
  current_state_time = current_time;
  current_state_init = true;
  current_lap_time = 0.0;
}

bool Competition::stateIsInit() {
  if (current_state_init) {
    current_state_init = false;
    return true;
  }
  return false;
}

Node *Competition::loadTrack() {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  // cout << "Loading track " << tracks[current_track] << endl;
  root_children->importMFNode(-1, string("saved_nodes/") + tracks[current_track]);
  // cout << "Loaded track " << tracks[current_track] << endl;
  return root_children->getMFNode(-1);
}

void Competition::removeTrack(Node *track) { track->remove(); }

void Competition::setCurrentTrackIndex(int track) { current_track = track; }

int Competition::getCurrentTrackIndex() { return current_track; }

void Competition::setLapsTotal(int laps) { laps_total = laps; }

int Competition::getLapsTotal() { return laps_total; }

Node *Competition::loadRobot(string name) {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  root_children->importMFNode(-1, string("saved_nodes/") + name);
  return root_children->getMFNode(-1);
}

void Competition::removeRobot(Node *robot) { robot->remove(); }

void Competition::incrementLapTime(long time) { current_lap_time += time / 1000.0; }

double Competition::getLapTime() { return current_lap_time; }

void Competition::setLapTime(double time) { current_lap_time = time; }

void Competition::addCurrentLapPoint() {
  current_lap_vector.push_back(current_lap_time);
  if (best_lap_vector.size() < current_lap_vector.size()) {
    best_lap_vector.push_back(current_lap_time);
  }
}

double Competition::getTimeDiff() {
  time_diff = current_lap_vector[current_lap_vector.size() - 1] - best_lap_vector[current_lap_vector.size() - 1];
  return time_diff;
}

void Competition::checkBestLap() {
  if (current_lap_time < best_lap_time) {
    best_lap_time = current_lap_time;
    best_lap_vector = current_lap_vector;
  }
  resetCurrentLap();
}

void Competition::resetCurrentLap() { current_lap_vector.clear(); }

void Competition::resetBestLap() {
  best_lap_time = 999.9;
  best_lap_vector.clear();
}
