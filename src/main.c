#include <bits/time.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cube.h"

// how much wider to draw the screen
#define GRAPHICAL_X_MULTIPLIER 2
#define FRAMERATE 60.0
#define LIGHT_MODE 0

#define DEBUG

// make some constant radians value to rotate by,
// 2.0 * M_PI / 600.0 -> a full rotation takes 10 sec
#define ROTATION_RADIANS (2.0 * M_PI / (FRAMERATE * 20))

#define MIN(A, B) ((A < B) ? A : B)

// just for convenience, converting "normal" colors
// to colors usable in ncurses
int nc_col(double v) { return (int)(v / 256 * 1000); }

// rotation that varies
void rotate_c(CubePoints *cp, double *rotation_rad) {
  *rotation_rad += (2.0 * M_PI) / (FRAMERATE * 100);
  *rotation_rad = fmod(*rotation_rad, (2.0 * M_PI));

  x_rotation(cp, cos(*rotation_rad) * ROTATION_RADIANS);
  y_rotation(cp, sin(*rotation_rad) * ROTATION_RADIANS);
}

void qcol(int num, double r, double g, double b) {
  init_color(num, nc_col(r), nc_col(g), nc_col(b));
}

void dist_color(double dist, double r, double g, double b) {
  // should redefine color 1 based on dist; in practice this
  // should make things that are closer (dist < 1) brighter,
  // and things that are further away (dist >= 1) darker

  qcol(1, r / (2 * dist), g / (2 * dist), b / (2 * dist));
  // qcol(1, r / dist, g / dist, b / dist);
}

int main() {

  // ncurses init, not everything is strictly needed
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
  // if color this is needed
  start_color();

#ifndef LIGHT_MODE
#define LIGHT_MODE 0
#endif

#if LIGHT_MODE == 0
  // colors
  const double r = 0;
  const double g = 1000;
  const double b = 500;

  init_color(1, r, g, b);
  init_pair(1, 1, COLOR_BLACK);

#else
  const double r = 0;
  const double g = 200 / 256 * 1000;
  const double b = 1000;

  init_color(1, r, g, b);
  init_pair(1, 1, COLOR_WHITE);

  // white background
  init_pair(4, COLOR_WHITE, COLOR_WHITE);
  bkgd(COLOR_PAIR(4));
#endif

  refresh();

  // available drawing area; x is divided by 2 becaue
  // we will stretch the drawing by 2 horizontally to
  // make the dimensions appear even
  int lines = LINES;
  int cols = COLS / GRAPHICAL_X_MULTIPLIER;

  // make a cube
  int cube_side_len = MIN(lines, cols);
  // proj_d = distance from viewer to projection plane
  // cube_d = distance to move the cube before projecting
  double cube_d = cube_side_len * 3;
  double proj_d = cube_side_len;
  CubePoints cp = new_cube(cube_side_len * 1.5);

  // two choices, either we change rotation after a full rotation,
  // or we vary the amount of rotation depending on a function.
  double rotation_rad = 0;

#ifdef DEBUG
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);

  long frames_drawn = 0;
#endif

	// for calculating an even framerate
  struct timespec draw_start, draw_end;
  double draw_time, sleep_time;

  while (1) {
    // main loop

    clock_gettime(CLOCK_MONOTONIC, &draw_start);

#ifdef DEBUG
    frames_drawn++;
#endif
    erase();

    // mvprintw(0, 0, "Frames: %d", frames_drawn);
    // mvprintw(0, cols - 1 - 3, "%d", cols);  // 213
    // mvprintw(1, cols - 1 - 2, "%d", LINES); // 45

    // get a zbuffer -- remember to free this
    ProjectedPoint(*zb)[lines][cols] =
        make_zbuffer(&cp, proj_d, cube_d, lines, cols);

    for (int y = 0; y < lines; y++) {
      for (int x = 0; x < cols; x++) {
        // draw each cell
        // if the distance to a point is INFINITY, it means we didn't project a
        // coord to that cell
        if ((*zb)[y][x].distance != INFINITY) {
          // something to judge distance by
          // "real distance"/distance from camera to cube center
          // 0~2 ish
          double dist_float = (*zb)[y][x].distance / cube_d;

					// UGLY
          if (dist_float < 0.5) {
            attron(A_BOLD);
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "00");
            attroff(COLOR_PAIR(1));
            attroff(A_BOLD);
          } else if (dist_float < 0.6) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "00");
            attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.7) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "OO");
            attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.8) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "oo");
            attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.9) {
            attron(COLOR_PAIR(2));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "oo");
            attroff(COLOR_PAIR(2));
          } else if (dist_float < 1.0) {
            attron(COLOR_PAIR(2));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "..");
            attroff(COLOR_PAIR(2));
          }
        }
      }
    }

    free(zb);

    // x_rotation(&cp, ROTATION_RADIANS);
    // y_rotation(&cp, ROTATION_RADIANS);
    rotate_c(&cp, &rotation_rad);

    // enable quitting
    int ch;
    // NOTE: getch() calls refresh()! So don't refresh manually!
    ch = getch();
    if (ch == 'q') {
      break;
    }

    clock_gettime(CLOCK_MONOTONIC, &draw_end);
    // calculate draw_time: time in ms since last draw
    draw_time = (draw_end.tv_sec - draw_start.tv_sec) * 1.0e3 +
                (draw_end.tv_nsec - draw_start.tv_nsec) * 1.0e-6;

    // for e.g. 60 fps, we want to sleep (with 0 zero draw time) 1/60
    // = 16.666... ms between each frame

    sleep_time = (1 / FRAMERATE) * 1.0e3 - draw_time;
		// if negative sleep_time, we just don't sleep at all and start drawing
		// next frame -- we can't keep up with the target framerate
    sleep_time = (sleep_time >= 0) ? sleep_time : 0;

    // NOTE: usleep sleeps for usec (microseconds, millions of a second)
    usleep(sleep_time * 1e3);
  }

  // ncurses quit, restore term
  curs_set(1);
  endwin();

#ifdef DEBUG
  clock_gettime(CLOCK_MONOTONIC, &end);
  // nsec_diff is the elapsed time in nanoseconds
  double elapsed = (end.tv_sec - start.tv_sec);
  elapsed += (end.tv_nsec - start.tv_nsec) / 1.0e9;

  printf("\n\nFrames drawn: %ld\nTotal time: %lf\nAverage FPS: %lf\n\n",
         frames_drawn, elapsed, (double)frames_drawn / elapsed);
#endif

  return 0;
}
