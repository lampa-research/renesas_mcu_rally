#include <tv.h>
#define HYPATIA_IMPLEMENTATION
#include <hypatia.h>

TV::TV(Supervisor *supervisor) : supervisor{supervisor} {
  generator = mt19937(randomDevice());
  uniformPositive = uniform_real_distribution<double>{0.0, 1.0};
  uniformPositiveAndNegative = uniform_real_distribution<double>{-1.0, 1.0};
}

void TV::showTitle(string text) {
  supervisor->setLabel(0, text, 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(1, text, 0.000, 0.000, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::hideTitle() {
  supervisor->setLabel(0, "", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(1, "", 0.000, 0.000, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::showTimer(int seconds) {
  stringstream ss;
  ss << setfill('0') << setw(2) << seconds / 60 << ":" << setw(2) << seconds % 60;
  supervisor->setLabel(2, ss.str(), 0.462, 0.002, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(3, ss.str(), 0.460, 0.000, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::hideTimer() {
  supervisor->setLabel(2, "", 0.462, 0.002, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(3, "", 0.460, 0.000, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::showDiff(double diff) {
  stringstream ss;
  ss << fixed << showpos << setw(5) << setfill('0') << setprecision(2) << diff;
  supervisor->setLabel(4, ss.str(), 0.462, 0.002, 0.1, 0x000000, 0, "Arial Black");
  int color = 0xffffff;
  diff = std::clamp(diff, -1.0, 1.0);
  if (diff > 0) {
    color = 0xff0000;
    color += ((int)((1.0 - diff) * 0xff) << 8) + (int)((1.0 - diff) * 0xff);
  } else if (diff < 0) {
    color = 0x00ff00;
    color += ((int)((1.0 + diff) * 0xff) << 16) + (int)((1.0 + diff) * 0xff);
  }
  supervisor->setLabel(5, ss.str(), 0.460, 0.000, 0.1, color, 0, "Arial Black");
}

void TV::hideDiff() {
  supervisor->setLabel(4, "", 0.462, 0.002, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(5, "", 0.460, 0.000, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::showCompetitorData(vector<Competitor> competitors, int current_competitor) {
  stringstream ss;
  ss << "Current: " << competitors[current_competitor].getNameString();
  if (current_competitor < (int)competitors.size() - 1) {
    ss << "Next: " << competitors[current_competitor + 1].getNameString();
  }
  supervisor->setLabel(6, ss.str(), 0.002, 0.002, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(7, ss.str(), 0.000, 0.000, 0.08, 0xffffff, 0, "Arial Black");
}

void TV::hideCompetitorData() {
  supervisor->setLabel(6, "", 0.002, 0.002, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(7, "", 0.000, 0.000, 0.08, 0xffffff, 0, "Arial Black");
}

void TV::showCurrentTime(double lap, double lap_time, double previous_lap_time) {
  stringstream ss;
  ss << setprecision(2) << fixed << "Lap: " << lap << "\nCurrent: " << lap_time << "\nPrevious: " << previous_lap_time;
  supervisor->setLabel(8, ss.str(), 0.002, 0.852, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(9, ss.str(), 0.000, 0.850, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::hideCurrentTime() {
  supervisor->setLabel(8, "", 0.002, 0.852, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(9, "", 0.000, 0.850, 0.1, 0xffffff, 0, "Arial Black");
}

void TV::showBoard(vector<Competitor> competitors, int current_competitor, int current_track) {
  supervisor->setLabel(10, "Standings", 0.412, 0.122, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(11, "Standings", 0.410, 0.120, 0.1, 0x00ff00, 0, "Arial Black");

  stringstream ss;
  ss << "Pos." << endl;
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << setw(4) << i + 1 << endl;
  }
  supervisor->setLabel(12, ss.str(), 0.052, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(13, ss.str(), 0.050, 0.200, 0.08, 0x00ff00, 0, "Arial Black");

  ss.str(string());
  ss << "Team" << endl;
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getTeamName() << endl;
  }
  supervisor->setLabel(14, ss.str(), 0.102, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(15, ss.str(), 0.100, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  ss << "Robot" << endl;
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getRobotName() << endl;
  }
  supervisor->setLabel(16, ss.str(), 0.252, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(17, ss.str(), 0.250, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  ss << "Country" << endl;
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getCountry() << endl;
  }
  supervisor->setLabel(18, ss.str(), 0.402, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(19, ss.str(), 0.400, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  ss << "Laps: Time (Best)" << endl;
  for (int i = 0; i < current_competitor; i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getResult(current_track);
  }
  supervisor->setLabel(20, ss.str(), 0.522, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(21, ss.str(), 0.520, 0.200, 0.08, 0x00ff00, 0, "Arial Black");

  ss.str(string());
  ss << "Q/S/F" << endl;
  for (int i = 0; i < current_competitor; i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getPointsStages();
  }
  supervisor->setLabel(22, ss.str(), 0.742, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(23, ss.str(), 0.740, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  ss << "Total" << endl;
  for (int i = 0; i < current_competitor; i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getPointsTotal();
  }
  supervisor->setLabel(24, ss.str(), 0.852, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(25, ss.str(), 0.850, 0.200, 0.08, 0x00ff00, 0, "Arial Black");
}

void TV::hideBoard() {
  supervisor->setLabel(10, "", 0.412, 0.122, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(11, "", 0.410, 0.120, 0.1, 0x00ff00, 0, "Arial Black");
  supervisor->setLabel(12, "", 0.052, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(13, "", 0.050, 0.200, 0.08, 0x00ff00, 0, "Arial Black");
  supervisor->setLabel(14, "", 0.102, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(15, "", 0.100, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(16, "", 0.252, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(17, "", 0.250, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(18, "", 0.402, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(19, "", 0.400, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(20, "", 0.502, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(21, "", 0.500, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(22, "", 0.702, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(23, "", 0.700, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(24, "", 0.852, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(25, "", 0.850, 0.200, 0.08, 0x00ff00, 0, "Arial Black");
}

void TV::showStartList(vector<Competitor> competitors) {
  supervisor->setLabel(26, "Starting order", 0.412, 0.122, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(27, "Starting order", 0.410, 0.120, 0.1, 0x00ff00, 0, "Arial Black");
  stringstream ss;
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << setw(4) << i + 1 << endl;
  }
  supervisor->setLabel(28, ss.str(), 0.252, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(29, ss.str(), 0.250, 0.200, 0.08, 0x00ff00, 0, "Arial Black");

  ss.str(string());
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getTeamName() << endl;
  }
  supervisor->setLabel(30, ss.str(), 0.302, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(31, ss.str(), 0.300, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getRobotName() << endl;
  }
  supervisor->setLabel(32, ss.str(), 0.502, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(33, ss.str(), 0.500, 0.200, 0.08, 0xffffff, 0, "Arial Black");

  ss.str(string());
  for (int i = 0; i < (int)competitors.size(); i++) {
    Competitor competitor = competitors[i];
    ss << competitor.getCountry() << endl;
  }
  supervisor->setLabel(34, ss.str(), 0.702, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(35, ss.str(), 0.700, 0.200, 0.08, 0xffffff, 0, "Arial Black");
}

void TV::hideStartList() {
  supervisor->setLabel(26, "", 0.412, 0.122, 0.1, 0x000000, 0, "Arial Black");
  supervisor->setLabel(27, "", 0.410, 0.120, 0.1, 0x00ff00, 0, "Arial Black");
  supervisor->setLabel(28, "", 0.252, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(29, "", 0.250, 0.200, 0.08, 0x00ff00, 0, "Arial Black");
  supervisor->setLabel(30, "", 0.302, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(31, "", 0.300, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(32, "", 0.502, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(33, "", 0.500, 0.200, 0.08, 0xffffff, 0, "Arial Black");
  supervisor->setLabel(34, "", 0.702, 0.202, 0.08, 0x000000, 0, "Arial Black");
  supervisor->setLabel(35, "", 0.700, 0.200, 0.08, 0xffffff, 0, "Arial Black");
}

void TV::showLeading(vector<Competitor> competitors, int current_competitor) {
  stringstream ss;
  ss << "Leaderboard\n";
  for (int i = 0; i < current_competitor; i++) {
    Competitor competitor = competitors[i];
    ss << setw(2) << i + 1 << " " << competitor.getNameString();
  }
  supervisor->setLabel(33, ss.str(), 0.752, 0.002, 0.06, 0x000000, 0, "Arial Black");
  supervisor->setLabel(34, ss.str(), 0.750, 0.000, 0.06, 0xffffff, 0, "Arial Black");
}

void TV::hideLeading() {
  supervisor->setLabel(33, "", 0.752, 0.002, 0.06, 0x000000, 0, "Arial Black");
  supervisor->setLabel(34, "", 0.750, 0.000, 0.06, 0xffffff, 0, "Arial Black");
}

void TV::setCameraTracking(Node *robot_node) {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  Node *view_point = root_children->getMFNode(1);
  Field *follow_field = view_point->getField("follow");
  follow_field->setSFString("robot");
  Field *follow_type_field = view_point->getField("followType");
  follow_type_field->setSFString("Tracking Shot");

  Field *robot_orientation_field = robot_node->getField("rotation");
  Field *robot_position_field = robot_node->getField("translation");
  const double *robot_orientation = robot_orientation_field->getSFRotation();
  const double *robot_position = robot_position_field->getSFVec3f();

  struct quaternion robot_quaternion;
  quaternion_set_from_axis_anglef3(&robot_quaternion, robot_orientation[0], robot_orientation[1], robot_orientation[2], robot_orientation[3]);
  struct vector3 position_vector {
    0.85, 1.8, 1.67
  };
  position_vector = *vector3_rotate_by_quaternion(&position_vector, &robot_quaternion);
  struct quaternion view_quaternion;
  quaternion_set_from_axis_anglef3(&view_quaternion, -0.557, 0.702, 0.444, 2.96);
  robot_quaternion = *quaternion_multiply(&robot_quaternion, &view_quaternion);
  struct vector3 view_vector;
  double view_angle;
  quaternion_get_axis_anglev3(&robot_quaternion, &view_vector, &view_angle);

  Field *orientation_field = view_point->getField("orientation");
  double orientation[4] = {-1, 0, 0, 1};
  orientation[0] = view_vector.x;
  orientation[1] = view_vector.y;
  orientation[2] = view_vector.z;
  orientation[3] = view_angle;
  orientation_field->setSFRotation(orientation);
  Field *position_field = view_point->getField("position");
  double position[3] = {robot_position[0] + position_vector.x, robot_position[1] + position_vector.y, robot_position[2] + position_vector.z};
  position_field->setSFVec3f(position);
}

void TV::setCameraMounted(Node *robot_node) {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  Node *view_point = root_children->getMFNode(1);
  Field *follow_field = view_point->getField("follow");
  follow_field->setSFString("robot");
  Field *follow_type_field = view_point->getField("followType");
  follow_type_field->setSFString("Mounted Shot");

  Field *robot_orientation_field = robot_node->getField("rotation");
  Field *robot_position_field = robot_node->getField("translation");
  const double *robot_orientation = robot_orientation_field->getSFRotation();
  const double *robot_position = robot_position_field->getSFVec3f();

  struct quaternion robot_quaternion;
  quaternion_set_from_axis_anglef3(&robot_quaternion, robot_orientation[0], robot_orientation[1], robot_orientation[2], robot_orientation[3]);
  struct vector3 position_vector {
    -1.0, 0.3, 0.0
  };
  position_vector = *vector3_rotate_by_quaternion(&position_vector, &robot_quaternion);

  struct quaternion view_quaternion;
  quaternion_set_from_axis_anglef3(&view_quaternion, -0.985, 0.135, -0.103, 1.5807);
  robot_quaternion = *quaternion_multiply(&robot_quaternion, &view_quaternion);
  struct vector3 view_vector;
  double view_angle;
  quaternion_get_axis_anglev3(&robot_quaternion, &view_vector, &view_angle);

  // position_vector = *vector3_rotate_by_quaternion(&position_vector, &robot_quaternion);

  Field *orientation_field = view_point->getField("orientation");
  double orientation[4] = {0, -1, 0, 1.5708};
  orientation[0] = view_vector.x;
  orientation[1] = view_vector.y;
  orientation[2] = view_vector.z;
  orientation[3] = view_angle;
  orientation_field->setSFRotation(orientation);
  Field *position_field = view_point->getField("position");
  double position[3] = {robot_position[0] + position_vector.x, robot_position[1] + position_vector.y, robot_position[2] + position_vector.z};
  position_field->setSFVec3f(position);
}

void TV::setCameraTV(Node *robot_node) {
  double x = robot_node->getField("translation")->getSFVec3f()[0];
  double z = robot_node->getField("translation")->getSFVec3f()[2];
  // get robot rotation
  const double *robot_orientation = robot_node->getField("rotation")->getSFRotation();
  // convert to quaternion
  struct quaternion robot_quaternion;
  quaternion_set_from_axis_anglef3(&robot_quaternion, robot_orientation[0], robot_orientation[1], robot_orientation[2], robot_orientation[3]);
  struct vector3 forward_vector {
    2.0, 0.0, 0.0
  };
  forward_vector = *vector3_rotate_by_quaternion(&forward_vector, &robot_quaternion);
  x += forward_vector.x;
  z += forward_vector.z;

  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  Node *view_point = root_children->getMFNode(1);
  Field *follow_field = view_point->getField("follow");
  follow_field->setSFString("robot");
  Field *follow_type_field = view_point->getField("followType");
  follow_type_field->setSFString("Pan and Tilt Shot");
  Field *position_field = view_point->getField("position");
  double position[3] = {x + 2.0 * uniformPositiveAndNegative(generator), 2.0 + 1.0 * uniformPositiveAndNegative(generator), z + 2.0 * uniformPositiveAndNegative(generator)};
  position_field->setSFVec3f(position);
}

void TV::setCameraRandom(Node *robot_node) {
  double probability = uniformPositive(generator);
  if (probability < 0.20) {
    setCameraMounted(robot_node);
  } else {
    setCameraTV(robot_node);
  }
}

void TV::setCameraHelicopter(double *start_position, double *start_orientation, double *end_position, double *end_orientation, double total_time, double time) {
  Node *root = supervisor->getRoot();
  Field *root_children = root->getField("children");
  Node *view_point = root_children->getMFNode(1);
  Field *follow_field = view_point->getField("follow");
  follow_field->setSFString("");
  Field *follow_type_field = view_point->getField("followType");
  follow_type_field->setSFString("Tracking Shot");
  Field *orientation_field = view_point->getField("orientation");
  double orientation[4] = {0.0, 0.0, 0.0, 0.0};
  for (int i{0}; i < 4; i++) {
    orientation[i] = start_orientation[i] + (end_orientation[i] - start_orientation[i]) * time / total_time;
  }
  orientation_field->setSFRotation(orientation);
  Field *position_field = view_point->getField("position");
  double position[3] = {0.0, 0.0, 0.0};
  position[0] = start_position[0] + (end_position[0] - start_position[0]) * time / total_time;
  position[1] = start_position[1] + (end_position[1] - start_position[1]) * time / total_time;
  position[2] = start_position[2] + (end_position[2] - start_position[2]) * time / total_time;
  position_field->setSFVec3f(position);
}
