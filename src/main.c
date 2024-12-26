#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "cube.h"

// how much wider to draw the screen
#define GRAPHICAL_X_MULTIPLIER 2
#define LIGHT_MODE 1

// make some constant radians value to rotate by,
// 2.0 * M_PI / 600.0 -> a full rotation takes 10 sec
#define ROTATION_RADIANS (2.0 * M_PI / 600.0)

#define MIN(A, B) ((A < B) ? A : B)

// just for convenience, converting "normal" colors
// to colors usable in ncurses
int nc_col(double v) { return (int)(v / 256 * 1000); }

// rotation that varies
void rotate_c(CubePoints *cp, double *rotation_rad) {
  *rotation_rad += (2.0 * M_PI) / 3000;
  *rotation_rad = fmod(*rotation_rad, (2.0 * M_PI));

  x_rotation(cp, cos(*rotation_rad) * ROTATION_RADIANS);
  y_rotation(cp, sin(*rotation_rad) * ROTATION_RADIANS);
}

void qcol(int num, double r, double b, double g) {
	init_color(num, nc_col(r), nc_col(b), nc_col(g));
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

	#if LIGHT_MODE == 1
  // Bright
	qcol(1, 0, 256, 128);
  // init_color(1, nc_col(0), nc_col(256), nc_col(128));
  // Darkened
	qcol(2, 0, 256 * 0.66, 128 * 0.66);
  // init_color(2, nc_col(0), nc_col(256.0 * 0.66), nc_col(128 * 0.66));
  // Very dark
	qcol(3, 0, 256 * 0.33, 128 * 0.33);
  // init_color(3, nc_col(0), nc_col(256.0 * 0.33), nc_col(128 * 0.33));

  init_pair(1, 1, COLOR_BLACK);
  init_pair(2, 2, COLOR_BLACK);
  init_pair(3, 3, COLOR_BLACK);

	#else
	// Bright
	qcol(1, 0, 200, 256);
	// Darkened
	qcol(2, 0, 179, 232);
	// Very Dark
	qcol(3, 0, 161, 209);

	init_pair(1, 1, COLOR_WHITE);
  init_pair(2, 2, COLOR_WHITE);
  init_pair(3, 3, COLOR_WHITE);

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
  CubePoints cp = new_cube(cube_side_len * 1.5);

  // two choices, either we change rotation after a full rotation,
  // or we vary the amount of rotation depending on a function.
  double rotation_rad = 0;

  while (1) {
    // main loop
    erase();

    // mvprintw(0, 0, "Frames: %d", frames_drawn);
    // mvprintw(0, cols - 1 - 3, "%d", cols);  // 213
    // mvprintw(1, cols - 1 - 2, "%d", LINES); // 45

    // render the cube
    // Cube c = render_cube(cp);

    // get a zbuffer -- remember to free this
    // proj_d = distance from viewer to projection plane
    // cube_d = distance to move the cube before projecting
		double cube_d = cube_side_len * 3;
		double proj_d = cube_side_len;
    ProjectedPoint(*zb)[lines][cols] = make_zbuffer(&cp, proj_d, cube_d, lines, cols);

    for (int y = 0; y < lines; y++) {
      for (int x = 0; x < cols; x++) {
        // draw each cell
        if ((*zb)[y][x].distance != INFINITY) {
					// something to judge distance by
					// "real distance"/distance from camera to cube center
          double dist_float = (*zb)[y][x].distance / cube_d;

          // UGLY
          if (dist_float < 0.7) {
            attron(A_BOLD);
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "00");
            attroff(COLOR_PAIR(1));
            attroff(A_BOLD);
          } else if (dist_float < 0.75) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "00");
            attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.8) {
            attron(COLOR_PAIR(1));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "OO");
            attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.85) {
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
          } else if (dist_float >= 1.0) {
            attron(COLOR_PAIR(3));
            mvprintw(y, x * GRAPHICAL_X_MULTIPLIER, "..");
            attroff(COLOR_PAIR(3));
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
    // NOTE: getch() calls refresh()!
    ch = getch();
    if (ch == 'q') {
      break;
    }

    usleep(16 * 1000); // 60 fps, can change
  }

  // ncurses quit, restore term
  curs_set(1);
  endwin();

  return 0;
}
