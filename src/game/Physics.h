#pragma once

namespace program {
constexpr double PI = 3.14159265358979323846;

struct Physics {
  double x = 0.;
  double y = 0.;
  double vx = 0.;
  double vy = 0.;
  double ax = 0.;
  double ay = 0.;
  double mass = 1.;
  double radius = 32.;
  double friction = 0.0;
};

enum HeadingTurnDirection {
  NONE,
  LEFT,
  RIGHT,
};

struct Heading {
  double angle = 0.;
  double rotationRate = 0.25;
  HeadingTurnDirection turnDirection = NONE;
};
} // namespace Program