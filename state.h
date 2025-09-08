#ifndef STATE_H
#define STATE_H

#define LORENZ_POINTS 50000

// Simple point struct
typedef struct {
  double x;
  double y;
  double z;
} Point3D;

// Encapsulates every stateful variable for the app :)
typedef struct {
  // Lorenz parameters
  double s;
  double b;
  double r;

  // View state
  int th;     // Azimuth of view angle
  int ph;     // Elevation of view angle
  double dim; // Dimension of orthogonal box
  double asp; // Aspect ratio

  // Animation and visualization controls
  int animate;           // Animation toggle
  int colorMode;         // 0=single, 1=rainbow, 2=fade
  double animSpeed;      // Animation speed in seconds
  int currentPoints;     // Number of points to draw in animation
  unsigned int lastTime; // Last animation update time

  // The calculated points for the attractor
  Point3D points[LORENZ_POINTS];
} State;

#endif // STATE_H
