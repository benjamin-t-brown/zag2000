#include "Physics.h"
#include "State.h"
#include <cmath>

namespace program {
namespace physics {
std::pair<double, double> getPos(const Physics& physics) {
  return {physics.x, physics.y};
}

Circle::Circle(double xA, double yA, double rA) : x(xA), y(yA), r(rA) {}

Rect::Rect(double xA, double yA, double wA, double hA)
    : x(xA), y(yA), w(wA), h(hA) {}

double getAngleTowards(std::pair<double, double> p1,
                       std::pair<double, double> p2) {
  const double x1 = p1.first;
  const double y1 = p1.second;
  const double x2 = p2.first;
  const double y2 = p2.second;
  const double lenY = y2 - y1;
  const double dx = x1 - x2;
  const double dy = y1 - y2;
  const double hyp = std::sqrt(dx * dx + dy * dy);
  double ret = 0.;
  if (y2 >= y1 && x2 >= x1) {
    ret = (std::asin(lenY / hyp) * 180.) / PI + 90.;
  } else if (y2 >= y1 && x2 < x1) {
    ret = (std::asin(lenY / -hyp) * 180.) / PI - 90.;
  } else if (y2 < y1 && x2 > x1) {
    ret = (std::asin(lenY / hyp) * 180.) / PI + 90.;
  } else {
    ret = (std::asin(-lenY / hyp) * 180.) / PI - 90.;
  }
  if (ret >= 360) {
    ret = 360. - ret;
  }
  if (ret < 0) {
    ret = 360. + ret;
  }
  return ret;
}

double getAngleTowards(const Physics& p1, std::pair<double, double> p2) {
  return getAngleTowards(std::make_pair(p1.x, p1.y), p2);
}

double getAngleTowards(const Physics& p1, const Physics& p2) {
  return getAngleTowards(std::make_pair(p1.x, p2.y),
                         std::make_pair(p2.x, p2.y));
}

double degreesToRadians(double degrees) { return (degrees * PI) / 180.0; };
double radiansToDegrees(double radians) { return (radians * 180.) / PI; };

CollisionDirection collidesCircleRect(const Circle& c, const Rect& r2) {
  Rect r1(c.x - c.r, c.y - c.r, c.r * 2, c.r * 2);
  double dx = (r1.x + r1.w / 2) - (r2.x + r2.w / 2);
  double dy = (r1.y + r1.h / 2) - (r2.y + r2.h / 2);
  double width = (r1.w + r2.w) / 2;
  double height = (r1.h + r2.h) / 2;
  double crossWidth = width * dy;
  double crossHeight = height * dx;

  CollisionDirection collision = CollisionDirection::NONE;
  if (std::abs(dx) <= width && std::abs(dy) <= height) {
    if (crossWidth > crossHeight) {
      collision = (crossWidth > (-crossHeight)) ? CollisionDirection::BOTTOM
                                                : CollisionDirection::LEFT;
    } else {
      collision = (crossWidth > -(crossHeight)) ? CollisionDirection::RIGHT
                                                : CollisionDirection::TOP;
    }

    if (c.x <= r2.x && c.y <= r2.y) {
      collision = CollisionDirection::TOP_LEFT;
    } else if (c.x >= r2.x + r2.w && c.y <= r2.y) {
      collision = CollisionDirection::TOP_RIGHT;
    } else if (c.x <= r2.x && c.y >= r2.y + r2.h) {
      collision = CollisionDirection::BOTTOM_LEFT;
    } else if (c.x >= r2.x + r2.w && c.y >= r2.y + r2.h) {
      collision = CollisionDirection::BOTTOM_RIGHT;
    }
  }
  return collision;
}

double getAngleDegTowards(std::pair<double, double> point1,
                          std::pair<double, double> point2) {
  const double x1 = point1.first;
  const double y1 = point1.second;

  const double x2 = point2.first;
  const double y2 = point2.second;
  const double lenY = y2 - y1;
  const double lenX = x2 - x1;
  const double hyp = sqrt(lenX * lenX + lenY * lenY);
  double ret = 0.0;
  if (y2 >= y1 && x2 >= x1) {
    ret = (asin(lenY / hyp) * 180.0) / PI + 90.0;
  } else if (y2 >= y1 && x2 < x1) {
    ret = (asin(lenY / -hyp) * 180.0) / PI - 90.0;
  } else if (y2 < y1 && x2 > x1) {
    ret = (asin(lenY / hyp) * 180.0) / PI + 90.0;
  } else {
    ret = (asin(-lenY / hyp) * 180.0) / PI - 90.0;
  }
  if (ret >= 360.0) {
    ret = 360.0 - ret;
  }
  if (ret < 0) {
    ret = 360.0 + ret;
  }
  return ret;
}

std::pair<double, double> getVector(double angleDeg, double magnitude = 1.) {
  const double angleRad = degreesToRadians(angleDeg);
  return std::make_pair(std::sin(angleRad) * magnitude,
                        -std::cos(angleRad) * magnitude);
};

void applyDrag(Physics& physics, double dblDt) {
  const double coeff = physics.friction;

  double forceX = 0.;
  double forceY = 0.;

  // experimented with this for a while (linear/quadratic) this division by 2
  // gives an acceptable slidy-ness
  forceX = (coeff * physics.vx) / 2.;
  forceY = (coeff * physics.vy) / 2.;

  physics.vx -= (forceX / physics.mass) * dblDt;
  physics.vy -= (forceY / physics.mass) * dblDt;
}

void applyForce(Physics& physics, double headingDeg, double acc) {
  const auto [nextAx, nextAy] = getVector(headingDeg, acc);
  physics.ax += nextAx;
  physics.ay += nextAy;
}

void updateHeading(Heading& heading, const int dt) {
  double dblDt = static_cast<double>(dt);
  double mult = heading.turnDirection == HeadingTurnDirection::NONE   ? 0.
                : heading.turnDirection == HeadingTurnDirection::LEFT ? -1.
                                                                      : 1.;

  heading.angle += mult * heading.rotationRate * dblDt;
  if (heading.angle > 360.) {
    heading.angle -= 360.;
  } else if (heading.angle < 0.) {
    heading.angle += 360.;
  }
}

void updatePhysics(Physics& physics, const int dt) {
  const double dblDt = static_cast<double>(dt);

  const double vxMod = physics.ax / physics.mass;
  const double vyMod = physics.ay / physics.mass;

  if (physics.friction > 0) {
    applyDrag(physics, dblDt);
  }

  physics.vx += vxMod * dblDt;
  physics.vy += vyMod * dblDt;
  physics.x += physics.vx * dblDt;
  physics.y += physics.vy * dblDt;

  physics.ax = 0;
  physics.ay = 0;
}

void updatePhysics(Physics& physics,
                   const int dt,
                   std::vector<Rect>& obstacles) {
  updatePhysics(physics, dt);

  // copied from old version
  const double r = TILE_WIDTH_D / 6.;
  const double r2 = r * 1.25;
  Circle circle(physics.x, physics.y, r);
  for (const auto& rect : obstacles) {
    CollisionDirection collision = collidesCircleRect(circle, rect);
    if (collision != CollisionDirection::NONE) {
      if (collision == CollisionDirection::TOP) {
        physics.y = rect.y - r2;
        physics.vy = 0.;
      } else if (collision == CollisionDirection::BOTTOM) {
        physics.y = rect.y + rect.h + r2;
        physics.vy = 0.;
      } else if (collision == CollisionDirection::LEFT) {
        physics.x = rect.x - r2;
        physics.vx = 0.;
      } else if (collision == CollisionDirection::RIGHT) {
        physics.x = rect.x + rect.w + r2;
        physics.vx = 0.;
      } else if (collision == CollisionDirection::TOP_LEFT) {
        physics.y = rect.y - r2 - 1;
        physics.x = rect.x - r2 - 1;
      } else if (collision == CollisionDirection::TOP_RIGHT) {
        physics.y = rect.y - r2 - 1;
        physics.x = rect.x + rect.w + r2 + 1;
      } else if (collision == CollisionDirection::BOTTOM_LEFT) {
        physics.y = rect.y + rect.h + r2 + 1;
        physics.x = rect.x - r2 - 1;
      } else if (collision == CollisionDirection::BOTTOM_RIGHT) {
        physics.y = rect.y + rect.h + r2 + 1;
        physics.x = rect.x + rect.w + r2 + 1;
      }
    }
  }
}

} // namespace physics
} // namespace program