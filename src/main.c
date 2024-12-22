#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <unistd.h>

#include "cube.h"

//#define make_zuffer(C, ROWS, COLS) ((ProjectedPoint(*)[ROWS][COLS])make_zuffer(C, ROWS, COLS))

typedef struct {
	int x;
} SomeStruct;

int main() {

  // ncurses init, kinda just boilerplate
	// not everything is strictly needed
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
	// if color this is needed
  // start_color();

	refresh();

	// make a cube
	CubePoints cp = new_cube(32);

	// make some constant radians value to rotate by
	const double rad = 0.01;
	int frames_drawn = 0;

	while (1) {
		// main loop
		erase();

		mvprintw(0, COLS-1-3, "%d", COLS); // 213
		mvprintw(1, COLS-1-2, "%d", LINES); // 45
		mvprintw(2, COLS-1-10, "%d", frames_drawn);

		// render the cube
		Cube c = render_cube(cp);

		// get a zbuffer -- remember to free this
		ProjectedPoint (*zb)[40][40] = make_zuffer(&c, 40, 40);

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
					// print some char
					// mvprintw(y, x + 50, "%c", '@');
					if (dist_float < 0.5) {
						attron(A_BOLD);
						mvprintw(y, x, "O");
						attroff(A_BOLD);
					} else if (dist_float < 0.7) {
						mvprintw(y, x, "O");
					// } else if (dist_float < 0.5) {
					//	mvprintw(y, x, "o");
					} else if (dist_float < 0.9) {
						mvprintw(y, x, "o");
					} else if (dist_float < 1.0) {
						mvprintw(y, x, ".");
					}
				}
			}
		}
		free(zb);

		x_rotation(&cp, rad);
		y_rotation(&cp, rad); // fix this before uncommenting
		frames_drawn += 1;

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
