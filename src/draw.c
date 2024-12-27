#include <math.h>
#include <stdlib.h>

#include "cube.h"
#include "draw.h"

static void set_gradient_color(struct ncplane *ncp, int r, int g, int b, double dist) {
	int r_mod, g_mod, b_mod;

	r_mod = r * (2 * dist);
	g_mod = g * (2 * dist);
	b_mod = b * (2 * dist);

	ncplane_set_fg_rgb8_clipped(ncp, r_mod, g_mod, b_mod);
}

void *draw_thread(void *arg) {
	ArgStruct *args = arg;
  ProjectedPoint(*zb)[args->lines][args->cols] = args->zbuffer;

	double side_len = args->cube_dist / 3;
	double min_dist = args->cube_dist + sqrt(3) / 2 * side_len;
	double max_dist = args->cube_dist - sqrt(3) / 2 * side_len;

  for (int y = 0; y < args->lines; y++) {
    for (int x = 0; x < args->cols; x++) {
      if ((*zb)[y][x].distance != INFINITY) {
        double dist_float = (*zb)[y][x].distance / args->cube_dist;

				// int base = 0x006480;
				ncplane_set_bg_rgb8(args->ncp, 255, 255, 255);
				set_gradient_color(args->ncp, 0, 100, 128, dist_float);
				ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        // UGLY
        // if (dist_float < 0.7) {
				// 	// was bold and bright 00
				// 	ncplane_set_fg_rgb(args->ncp, BRIGHT);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "00");
        // } else if (dist_float < 0.75) {
				// 	// bright 00
				// 	ncplane_set_fg_rgb(args->ncp, BRIGHT);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "00");
        // } else if (dist_float < 0.8) {
				// 	// bright OO
				// 	ncplane_set_fg_rgb(args->ncp, BRIGHT);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "OO");
        // } else if (dist_float < 0.85) {
				// 	// bright oo
				// 	ncplane_set_fg_rgb(args->ncp, BRIGHT);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        // } else if (dist_float < 0.9) {
				// 	// darkened oo
				// 	ncplane_set_fg_rgb(args->ncp, MEDIUM);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        // } else if (dist_float < 1.0) {
				// 	// also darkened oo apparently
				// 	ncplane_set_fg_rgb(args->ncp, MEDIUM);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        // } else if (dist_float >= 1.0) {
				// 	// very darkened oo
				// 	ncplane_set_fg_rgb(args->ncp, DARK);
				// 	ncplane_putstr_yx(args->ncp, y, x * args->x_mult, "oo");
        // }
      }
    }
  }

	free(args->zbuffer);
	return NULL;
}
