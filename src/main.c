#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "cube.h"

#define GRAPHICAL_X_MULTIPLIER 2
#define CANVAS_HEIGHT 40
#define CANVAS_WIDTH 40

// make some constant radians value to rotate by
#define ROTATION_RADIANS (2.0 * M_PI / 600.0)

int nc_col(double v) { return (int)(v / 256 * 1000); }

void rotate_c(CubePoints *cp, double *rotation_rad) {
	*rotation_rad += (2.0 * M_PI) / 3000;
	*rotation_rad = fmod(*rotation_rad, (2.0 * M_PI));

	x_rotation(cp, cos(*rotation_rad) * ROTATION_RADIANS);
	y_rotation(cp, sin(*rotation_rad) * ROTATION_RADIANS);
}

int main() {

  // ncurses init, kinda just boilerplate
  // not everything is strictly needed i don't think
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
  // if color this is needed
  start_color();

  // Bright
  init_color(1, nc_col(0), nc_col(256), nc_col(128));
  // Darkened
  init_color(2, nc_col(0), nc_col(256.0 * 0.9), nc_col(128 * 0.9));

  init_pair(1, 1, COLOR_BLACK);
  init_pair(2, 2, COLOR_BLACK);

  // find center, offset a little to try and center it dirtily
  double center_y = LINES / 1.9;
  double center_x = COLS / 1.9;
  // "canvas" is 40x40
  int start_y = center_y + 0 - CANVAS_HEIGHT / 2.0;
  int start_x = center_x + 0 - CANVAS_WIDTH * 2.0;

  refresh();

  // make a cube
  CubePoints cp = new_cube(32);

	// two choices, either we change rotation after a full rotation,
	// or we vary the amount of rotation depending on a function.
	double rotation_rad = 0;

  while (1) {
    // main loop
    erase();

    // mvprintw(0, 0, "Frames: %d", frames_drawn);
    // mvprintw(0, COLS - 1 - 3, "%d", COLS);  // 213
    // mvprintw(1, COLS - 1 - 2, "%d", LINES); // 45

    // render the cube
    Cube c = render_cube(cp);

    // get a zbuffer -- remember to free this
    ProjectedPoint(*zb)[40][40] = make_zuffer(&c, 40, 40);

    double max_distance = -INFINITY;
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        // draw each cell
        if ((*zb)[y][x].distance != INFINITY) {
          double dist = (*zb)[y][x].distance;
          if (dist > max_distance) {
            // find furthest object that actually exists
            max_distance = dist;
          }
        }
      }
    }

    // point_distance / max_distance will always be 0 < d < 1
    // so we can draw different chars at different fractions of 1

    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        // draw each cell
        if ((*zb)[y][x].distance != INFINITY) {
          double dist_float = (*zb)[y][x].distance / max_distance;

          // i need to scale the output so the x is wider
          // i should also center it
          int nc_y = y + start_y;
          int nc_x = x + start_x;

          if (dist_float < 0.5) {
            attron(A_BOLD);
						attron(COLOR_PAIR(1));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "00");
						attroff(COLOR_PAIR(1));
            attroff(A_BOLD);
          } else if (dist_float < 0.6) {
						attron(COLOR_PAIR(1));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "00");
						attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.7) {
						attron(COLOR_PAIR(1));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "OO");
						attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.8) {
						attron(COLOR_PAIR(1));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "oo");
						attroff(COLOR_PAIR(1));
          } else if (dist_float < 0.9) {
						attron(COLOR_PAIR(2));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "oo");
						attroff(COLOR_PAIR(2));
          } else if (dist_float < 1.0) {
						attron(COLOR_PAIR(2));
            mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "..");
						attroff(COLOR_PAIR(2));
          }
        }
      }
    }
    free(zb);

    // i will loop through different rotations later, nyi
    // but something like counterclockwise x rotation ->
    // counterclockwise x+y rotation -> y_rotation -> etc
    // not too hard to implement as long as you make sure
    // to switch after full rotations. perhaps that's not
    // even necessary. maybe just make it rotate sin(x) in the x
    // and cos(x) in the y, thereby making it rotate different amounts
    // in each axis?

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
