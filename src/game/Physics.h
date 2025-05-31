#pragma once

#include <SDL2/SDL_rect.h>
#include <utility>
#include <vector>

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

namespace physics {

class Circle {
public:
  double x;
  double y;
  double r;
  Circle(double xA, double yA, double rA);
};

class Rect {
public:
  double x;
  double y;
  double w;
  double h;
  Rect(double xA, double yA, double wA, double hA);
};

enum CollisionDirection {
  NONE,
  TOP,
  BOTTOM,
  LEFT,
  RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT
};

std::pair<double, double> getPos(const Physics& physics);
void applyForce(Physics& physics, double headingDeg, double acc);
void updateHeading(Heading& heading, const int dt);
void updatePhysics(Physics& physics, const int dt);
void updatePhysics(Physics& physics,
                   const int dt,
                   std::vector<Rect>& obstacles);
} // namespace physics

} // namespace program