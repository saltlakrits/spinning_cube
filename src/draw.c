#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

#include "cube.h"

void draw(void *zbuffer, int lines, int cols, double cube_dist, int x_mult) {
  ProjectedPoint(*zb)[lines][cols] = zbuffer;

  for (int y = 0; y < lines; y++) {
    for (int x = 0; x < cols; x++) {
      if ((*zb)[y][x].distance != INFINITY) {
        double dist_float = (*zb)[y][x].distance / cube_dist;

        // UGLY
        if (dist_float < 0.7) {
          attron(A_BOLD);
          attron(COLOR_PAIR(1));
          mvprintw(y, x * x_mult, "00");
          attroff(COLOR_PAIR(1));
          attroff(A_BOLD);
        } else if (dist_float < 0.75) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * x_mult, "00");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.8) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * x_mult, "OO");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.85) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * x_mult, "oo");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.9) {
          attron(COLOR_PAIR(2));
          mvprintw(y, x * x_mult, "oo");
          attroff(COLOR_PAIR(2));
        } else if (dist_float < 1.0) {
          attron(COLOR_PAIR(2));
          mvprintw(y, x * x_mult, "oo");
          attroff(COLOR_PAIR(2));
        } else if (dist_float >= 1.0) {
          attron(COLOR_PAIR(3));
          mvprintw(y, x * x_mult, "oo");
          attroff(COLOR_PAIR(3));
        }
      }
    }
  }

	free(zbuffer);
}
