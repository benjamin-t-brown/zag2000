#include "Physics.h"
#include <cmath>

namespace program {

inline double degreesToRadians(double degrees) {
  return (degrees * PI) / 180.0;
};
inline double radiansToDegrees(double radians) {
  return (radians * 180.) / PI;
};

inline std::pair<double, double> getVector(double angleDeg,
                                           double magnitude = 1.) {
  const double angleRad = degreesToRadians(angleDeg);
  return std::make_pair(std::sin(angleRad) * magnitude,
                        -std::cos(angleRad) * magnitude);
};

inline void applyDrag(Physics& physics, double dblDt) {
  const double coeff = physics.friction;

  double forceX = 0.;
  double forceY = 0.;

  // experimented with this for a while (linear/quadratic) this division by 2
  // gives an acceptable slidy-ness
  forceX = (coeff * physics.vx) / 2;
  forceY = (coeff * physics.vy) / 2;

  physics.vx -= (forceX / physics.mass) * dblDt;
  physics.vy -= (forceY / physics.mass) * dblDt;
}

inline void applyForce(Physics& physics, double heading, double acc) {
  const auto [nextAx, nextAy] = getVector(heading, acc);
  physics.ax = nextAx;
  physics.ay = nextAy;
}

inline void updateHeading(Heading& heading, const int dt) {
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

inline void updatePhysics(Physics& physics, const int dt) {
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

  // // wrap physics to screen (aka wormhole)
  // if (physics.x > WINDOW_WIDTH) {
  //   physics.x = 0;
  // } else if (physics.x < 0) {
  //   physics.x = WINDOW_WIDTH;
  // }

  // // 22 is the ui height
  // if (physics.y > WINDOW_HEIGHT) {
  //   physics.y = 22;
  // } else if (physics.y < 22) {
  //   physics.y = WINDOW_HEIGHT;
  // }

  physics.ax = 0;
  physics.ay = 0;
}

} // namespace program