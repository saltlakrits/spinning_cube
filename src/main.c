#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <unistd.h>

#include "cube.h"

#define GRAPHICAL_X_MULTIPLIER 2
#define CANVAS_HEIGHT 40
#define CANVAS_WIDTH 40

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
  // start_color();

	// find center
	int center_y = LINES / 2;
	int center_x = COLS / 2;
	// "canvas" is 40x40
	int start_y = center_y + 2 - CANVAS_HEIGHT / 2;
	int start_x = center_x + 4 - CANVAS_WIDTH * 2;

	refresh();

	// make a cube
	CubePoints cp = new_cube(32);

	// make some constant radians value to rotate by
	const double rad = 0.01;
	int frames_drawn = 0;

	while (1) {
		// main loop
		erase();

		mvprintw(0, 0, "Frames: %d", frames_drawn);
		mvprintw(0, COLS-1-3, "%d", COLS); // 213
		mvprintw(1, COLS-1-2, "%d", LINES); // 45

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

					// i need to scale the output so the x is wider
					// i should also center it
					int nc_y = y + start_y;
					int nc_x = x + start_x;

					if (dist_float < 0.5) {
						attron(A_BOLD);
						mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "00");
						attroff(A_BOLD);
					} else if (dist_float < 0.7) {
						mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "OO");
					} else if (dist_float < 0.9) {
						mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "oo");
					} else if (dist_float < 1.0) {
						mvprintw(nc_y, nc_x * GRAPHICAL_X_MULTIPLIER, "..");
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
