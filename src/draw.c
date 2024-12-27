#include <math.h>
#include <stdlib.h>

#include "cube.h"
#include "draw.h"

int bright, medium, dark;

void *draw_thread(void *arg) {
  ArgStruct *args = arg;
  double(*zb)[args->lines][args->cols] = args->zbuffer;

  for (int y = 0; y < args->lines; y++) {
    for (int x = 0; x < args->cols; x++) {
      if ((*zb)[y][x] != INFINITY) {
        double dist_float = (*zb)[y][x] / args->cube_dist;

        if (dist_float < 0.7) {
        	// was bold and bright 00
        	ncplane_set_fg_rgb(args->ncp, bright);
					ncplane_on_styles(args->ncp, NCSTYLE_BOLD);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "00");
					ncplane_off_styles(args->ncp, NCSTYLE_BOLD);
        } else if (dist_float < 0.75) {
        	// bright 00
        	ncplane_set_fg_rgb(args->ncp, bright);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "00");
        } else if (dist_float < 0.8) {
        	// bright OO
        	ncplane_set_fg_rgb(args->ncp, bright);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "OO");
        } else if (dist_float < 0.85) {
        	// bright oo
        	ncplane_set_fg_rgb(args->ncp, bright);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        } else if (dist_float < 0.9) {
        	// darkened oo
        	ncplane_set_fg_rgb(args->ncp, medium);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        } else if (dist_float < 1.0) {
        	// also darkened oo apparently
        	ncplane_set_fg_rgb(args->ncp, medium);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        } else if (dist_float >= 1.0) {
        	// very darkened oo
        	ncplane_set_fg_rgb(args->ncp, dark);
        	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        }
      }
    }
  }

  free(args->zbuffer);
  return NULL;
}
