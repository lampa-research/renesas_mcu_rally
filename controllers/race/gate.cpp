#include <gate.h>

Gate::Gate(Supervisor *supervisor) : supervisor{supervisor} {
  self = supervisor->getSelf();
  distance_sensor = supervisor->getDistanceSensor("distance_sensor");
  distance_sensor->enable(TIME_STEP);

  display_front = supervisor->getDisplay("display_front");
  display_front->setFont("Lucida Sans Unicode", 20, true);
  display_back = supervisor->getDisplay("display_back");
  display_back->setFont("Lucida Sans Unicode", 20, true);

  for (int i = 0; i < 5; i++) {
    led_front[i] = supervisor->getLED("led_front_" + to_string(i));
    led_back[i] = supervisor->getLED("led_back_" + to_string(i));
  }
}

state Gate::getMode() {
  Field *mode_field = self->getField("mode");
  string mode = mode_field->getSFString();
  if (mode == "practice") {
    return state::PRACTICE;
  } else {
    return state::WELCOME;
  }
}

int Gate::getLaps() {
  Field *laps_field = self->getField("laps");
  return clamp(laps_field->getSFInt32(), 1, 5);
}

bool Gate::isLapStarted() {
  distance = distance_sensor->getValue();
  if (distance < DETECTION_LIMIT) {
    for (int i{0}; i < 5; i++) {
      led_front[i]->set(1);
      led_back[i]->set(1);
    }
    return true;
  }
  return false;
}

bool Gate::isLapFinished() {
  distance_previous = distance;
  distance = distance_sensor->getValue();
  if (distance < DETECTION_LIMIT && distance_previous >= DETECTION_LIMIT) {
    // cout << "Robot detected!" << endl;
    return true;
  }
  return false;
}

void Gate::clearDisplay(Display *display) {
  display->setColor(0xE5E5E5);
  display->fillRectangle(0, 0, display->getWidth(), display->getHeight());
  display->setColor(0x0000FF);
}

void Gate::showDisplay(Display *display, double current_lap_time, double *laptimes) {
  clearDisplay(display);
  display->drawText("Renesas MCU Rally", 2, 2);
  stringstream ss;
  ss << fixed << setprecision(2) << current_lap_time;
  display->drawText(ss.str(), 2, 27);
  ss.clear();
  ss.str(string());
  ss << fixed << setprecision(2) << laptimes[0];
  display->drawText(ss.str(), 2, 52);
  ss.clear();
  ss.str(string());
  ss << fixed << setprecision(2) << laptimes[1];
  display->drawText(ss.str(), 2, 77);
  ss.clear();
  ss.str(string());
  ss << fixed << setprecision(2) << laptimes[2];
  display->drawText(ss.str(), 2, 102);
}

void Gate::displayLap(double current_lap_time, double *laptimes) {
  showDisplay(display_front, current_lap_time, laptimes);
  showDisplay(display_back, current_lap_time, laptimes);
}

void Gate::clearDisplays() {
  clearDisplay(display_front);
  clearDisplay(display_back);
}

void Gate::setLEDs(int lap) {
  for (int i{0}; i < 5; i++) {
    led_front[i]->set(0);
    led_back[i]->set(0);
  }
  for (int i{0}; i < lap; i++) {
    led_front[i]->set(1);
    led_back[i]->set(1);
  }
}
