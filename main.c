/*
 *  Lorenz Attractor Visualizer
 *
 *  Interactive 3D visualization of the Lorenz attractor
 *
 *  Key bindings:
 *  SPACE  Toggle animation on/off
 *  c/C    Toggle color mode (single/rainbow/fade)
 *  +/-    Increase/decrease animation speed
 *  r/R    Increase/decrease r parameter (rho)
 *  s/S    Increase/decrease s parameter (sigma)
 *  b/B    Increase/decrease b parameter (beta)
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 */

#include "lorenz.h"
#include "state.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

// Animation parameters
#define MIN_SPEED 1.0
#define SPEED_STEP 1.0

// Global pointer to the application state
State *appState = NULL;

/*
 *  Convenience routine to output raster text
 */
#define LEN 8192 // Maximum length of text string
void Print(const char *format, ...) {
  char buf[LEN];
  char *ch = buf;
  va_list args;
  va_start(args, format);
  vsnprintf(buf, LEN, format, args);
  va_end(args);
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(1);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char *where) {
  int err = glGetError();
  if (err)
    fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}

void reshape(int width, int height);

/*
 *  Set color based on current color mode and point index
 */
void setPointColor(int pointIndex, int totalPoints) {
  switch (appState->colorMode) {
  case 0: // Single color - cyan
    glColor3f(0.0f, 1.0f, 1.0f);
    break;
  case 1: // Rainbow mode
  {
    float hue = (float)pointIndex / totalPoints * 360.0f;
    float c = 1.0f, x = c * (1.0f - fabs(fmod(hue / 60.0f, 2.0f) - 1.0f)),
          m = 0.0f;
    float r = 0, g = 0, b = 0;
    if (hue < 60) {
      r = c;
      g = x;
    } else if (hue < 120) {
      r = x;
      g = c;
    } else if (hue < 180) {
      g = c;
      b = x;
    } else if (hue < 240) {
      g = x;
      b = c;
    } else if (hue < 300) {
      r = x;
      b = c;
    } else {
      r = c;
      b = x;
    }
    glColor3f(r + m, g + m, b + m);
  } break;
  case 2: // Fade mode - blue to red
  {
    float ratio = (float)pointIndex / totalPoints;
    glColor3f(ratio, 0.2f, 1.0f - ratio);
  } break;
  }
}

/*
 *  Update animation
 */
void updateAnimation() {
  if (!appState->animate)
    return;

  unsigned int currentTime = glutGet(GLUT_ELAPSED_TIME);
  if (appState->lastTime == 0)
    appState->lastTime = currentTime;

  double elapsed = (currentTime - appState->lastTime) / 1000.0;
  double progress = elapsed / appState->animSpeed;
  appState->currentPoints = (int)(progress * LORENZ_POINTS);

  if (appState->currentPoints >= LORENZ_POINTS) {
    appState->currentPoints = LORENZ_POINTS;
    appState->lastTime = currentTime;
  }
  glutPostRedisplay();
}

/*
 *  Display the scene
 */
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();
  glRotated(appState->ph, 1, 0, 0);
  glRotated(appState->th, 0, 1, 0);

  if (LORENZ_POINTS > 0) {
    glLineWidth(1.5f);
    int pointsToDraw =
        appState->animate ? appState->currentPoints : LORENZ_POINTS;
    if (pointsToDraw > 0) {
      if (appState->colorMode == 0) {
        setPointColor(0, LORENZ_POINTS);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < pointsToDraw; i++)
          glVertex3d(appState->points[i].x, appState->points[i].y,
                     appState->points[i].z);
        glEnd();
      } else {
        glBegin(GL_LINES);
        for (int i = 0; i < pointsToDraw - 1; i++) {
          setPointColor(i, LORENZ_POINTS);
          glVertex3d(appState->points[i].x, appState->points[i].y,
                     appState->points[i].z);
          glVertex3d(appState->points[i + 1].x, appState->points[i + 1].y,
                     appState->points[i + 1].z);
        }
        glEnd();
      }
    }
  }

  glColor3f(0.8f, 0.8f, 0.8f);
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  glVertex3d(-30, 0, 0);
  glVertex3d(20, 0, 0);
  glVertex3d(0, -20, 0);
  glVertex3d(0, 20, 0);
  glVertex3d(0, 0, -10);
  glVertex3d(0, 0, 40);
  glEnd();

  glColor3f(1, 1, 1);
  glRasterPos3d(22, 0, 0);
  Print("X");
  glRasterPos3d(0, 22, 0);
  Print("Y");
  glRasterPos3d(0, 0, 42);
  Print("Z");

  glColor3f(1, 1, 1);
  glWindowPos2i(5, 5);
  Print("Lorenz Attractor - View: %d,%d", appState->th, appState->ph);
  glWindowPos2i(5, 25);
  Print("Animation: %s | Speed: %.1fs | Color: %s",
        appState->animate ? "ON" : "OFF", appState->animSpeed,
        appState->colorMode == 0   ? "Single"
        : appState->colorMode == 1 ? "Rainbow"
                                   : "Fade");
  if (appState->animate) {
    glWindowPos2i(5, 45);
    Print("Progress: %d/%d points", appState->currentPoints, LORENZ_POINTS);
  }
  glWindowPos2i(5, 65);
  Print("Params: s=%.1f b=%.2f r=%.1f", appState->s, appState->b, appState->r);
  glWindowPos2i(5, 85);
  Print("Controls: s/S,b/B,r/R=params, SPACE=anim, c=cycle color, +/-=speed, "
        "z/Z=zoom, arrows=rotate, 0=reset view");

  updateAnimation();
  ErrCheck("display");
  glFlush();
  glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y) {
  switch (ch) {
  case 27: // ESC
    exit(0);
    break;
  case '0': // Reset view
    appState->th = 0;
    appState->ph = 15;
    break;
  case ' ': // Toggle animation
    appState->animate = !appState->animate;
    if (appState->animate) {
      appState->lastTime = glutGet(GLUT_ELAPSED_TIME);
      appState->currentPoints = 0;
    }
    break;
  case 'c':
    appState->colorMode = (appState->colorMode + 1) % 3;
    break;
  case 'C': // Cycle color mode
    appState->colorMode = (appState->colorMode - 1) % 3;
    break;
  case '+':
  case '=': // Increase speed
    appState->animSpeed -= SPEED_STEP;
    if (appState->animSpeed < MIN_SPEED)
      appState->animSpeed = MIN_SPEED;
    break;
  case '-':
  case '_': // Decrease speed
    appState->animSpeed += SPEED_STEP;
    break;
  // Lorenz parameter controls
  case 's':
    appState->s += 0.5;
    computeLorenzPoints(appState);
    break;
  case 'S':
    appState->s -= 0.5;
    computeLorenzPoints(appState);
    break;
  case 'b':
    appState->b += 0.1;
    computeLorenzPoints(appState);
    break;
  case 'B':
    appState->b -= 0.1;
    computeLorenzPoints(appState);
    break;
  case 'r':
    appState->r += 1.0;
    computeLorenzPoints(appState);
    break;
  case 'R':
    appState->r -= 1.0;
    computeLorenzPoints(appState);
    break;
  case 'z':
    appState->dim -= 2.0;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    break;
  case 'Z':
    appState->dim += 2.0;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    break;
  }
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y) {
  if (key == GLUT_KEY_RIGHT)
    appState->th += 5;
  else if (key == GLUT_KEY_LEFT)
    appState->th -= 5;
  else if (key == GLUT_KEY_UP)
    appState->ph += 5;
  else if (key == GLUT_KEY_DOWN)
    appState->ph -= 5;
  appState->th %= 360;
  appState->ph %= 360;
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  appState->asp = (height > 0) ? (double)width / height : 1;
  glOrtho(-appState->asp * appState->dim, +appState->asp * appState->dim,
          -appState->dim, +appState->dim, -100, +100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*
 *  Idle callback for smooth animation
 */
void idle() { glutPostRedisplay(); }

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[]) {
  // Initialize state using a designated initializer list
  State state = {
      .s = 10.0,
      .b = 2.6666,
      .r = 28.0,
      .th = 0,
      .ph = 15,
      .dim = 60.0,
      .asp = 1.0,
      .animate = 1,
      .colorMode = 2,
      .animSpeed = 20.0,
      .currentPoints = 0,
      .lastTime = 0,
  };
  appState = &state;
  computeLorenzPoints(appState); // compute initial lorenz and update state

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Lorenz Assignment: Mason Bott");
#ifdef USEGLEW
  if (glewInit() != GLEW_OK)
    Fatal("Error initializing GLEW\n");
#endif

  // Set up OpenGL
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Setup GLUT callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);

  // Initialize animation timing
  appState->lastTime = glutGet(GLUT_ELAPSED_TIME);

  //  Pass control to GLUT so it can interact with the user
  glutMainLoop();
  return 0;
}
