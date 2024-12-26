#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

#include "cube.h"
#include "draw.h"

void *draw_thread(void *arg) {
	ArgStruct *args = arg;
  ProjectedPoint(*zb)[args->lines][args->cols] = args->zbuffer;

  for (int y = 0; y < args->lines; y++) {
    for (int x = 0; x < args->cols; x++) {
      if ((*zb)[y][x].distance != INFINITY) {
        double dist_float = (*zb)[y][x].distance / args->cube_dist;

        // UGLY
        if (dist_float < 0.7) {
          attron(A_BOLD);
          attron(COLOR_PAIR(1));
          mvprintw(y, x * args->x_mult, "00");
          attroff(COLOR_PAIR(1));
          attroff(A_BOLD);
        } else if (dist_float < 0.75) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * args->x_mult, "00");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.8) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * args->x_mult, "OO");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.85) {
          attron(COLOR_PAIR(1));
          mvprintw(y, x * args->x_mult, "oo");
          attroff(COLOR_PAIR(1));
        } else if (dist_float < 0.9) {
          attron(COLOR_PAIR(2));
          mvprintw(y, x * args->x_mult, "oo");
          attroff(COLOR_PAIR(2));
        } else if (dist_float < 1.0) {
          attron(COLOR_PAIR(2));
          mvprintw(y, x * args->x_mult, "oo");
          attroff(COLOR_PAIR(2));
        } else if (dist_float >= 1.0) {
          attron(COLOR_PAIR(3));
          mvprintw(y, x * args->x_mult, "oo");
          attroff(COLOR_PAIR(3));
        }
      }
    }
  }

	free(args->zbuffer);
	return NULL;
}
