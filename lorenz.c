#include "lorenz.h"
#include <stddef.h>

void computeLorenzPoints(State *state) {
  if (!state) return;

  double x = 1.0;
  double y = 1.0;
  double z = 1.0;
  double dt = 0.001;

  for (int i = 0; i < LORENZ_POINTS; i++) {
    double dx = state->s * (y - x);
    double dy = x * (state->r - z) - y;
    double dz = x * y - state->b * z;
    x += dt * dx;
    y += dt * dy;
    z += dt * dz;

    state->points[i].x = x;
    state->points[i].y = y;
    state->points[i].z = z;
  }
}