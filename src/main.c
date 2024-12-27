// you can tell this is well-written by the way
// it uses a million libraries for something so simple
#include <math.h>
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cube.h"
#include "draw.h"

// how much wider to draw the screen
// than it is tall
#define GRAPHICAL_X_MULTIPLIER 2
// target framerate, will not exceed this
// but can drop below (and the whole animation
// will slow down).
#define FRAMERATE 60.0
#define LIGHT_MODE 0

// if DEBUG is defined, collect and print out
// data about average FPS and such
#define DEBUG

// make some constant radians value to rotate by,
// 2.0 * M_PI / 600.0 -> a full rotation takes 10 sec
#define ROTATION_RADIANS (2.0 * M_PI / (FRAMERATE * 20))

#define MIN(A, B) ((A < B) ? A : B)

// TODO: illumination? zzz

// rotation that varies
void rotate_c(CubePoints *cp, double *rotation_rad) {
  *rotation_rad += (2.0 * M_PI) / (FRAMERATE * 100);
  *rotation_rad = fmod(*rotation_rad, (2.0 * M_PI));

  x_rotation(cp, cos(*rotation_rad) * ROTATION_RADIANS);
  y_rotation(cp, sin(*rotation_rad) * ROTATION_RADIANS);
}

int main() {

#ifndef LIGHT_MODE
#define LIGHT_MODE 0
#endif

#if LIGHT_MODE == 0
	// green (for dark mode) colors:
  // bright = 0x00ff80;
  // medium = 0x00a854;
  // dark = 0x00542a;

	// blue (for dark mode) colors:
  bright = 0x00c8ff;
  medium = 0x00a6d4;
  dark = 0x006480;

  int bg = NCCHANNEL_INITIALIZER(0, 0, 0);
  int fg = NCCHANNEL_INITIALIZER(255, 255, 255);

#else
  bright = 0x00c8ff;
  medium = 0x61ddff;
  dark = 0xc8e1ff;

  int bg = NCCHANNEL_INITIALIZER(255, 255, 255);
  int fg = NCCHANNEL_INITIALIZER(0, 0, 0);

#endif

  struct notcurses *nc = notcurses_init(NULL, NULL);
  struct ncplane *ncp = notcurses_stdplane(nc);
  struct ncinput *nci;

  // combine fg, bg to make channels value, set it as base
  // for the plane (basically just set a background color)
  int channels = ncchannels_combine(fg, bg);
  ncplane_set_base(ncp, " ", 0, channels);
  ncplane_erase(ncp);

  unsigned int LINES, COLS;
  notcurses_stddim_yx(nc, &LINES, &COLS);

  // available drawing area; x is divided by 2 becaue
  // we will stretch the drawing by 2 horizontally to
  // make the dimensions appear even, so we make the
  // actual drawing area half as big
  int lines = LINES;
  int cols = COLS / GRAPHICAL_X_MULTIPLIER;

  // make a cube
  double cube_side_len = MIN(lines, cols) * 1.5;
  // proj_d = distance from viewer to projection plane
  // cube_d = distance to move the cube before projecting
  double cube_d = cube_side_len * 2;
  double proj_d = cube_side_len / 1.5;
  CubePoints cp = new_cube(cube_side_len);

  // we use rotation_rad as a radians value to increase
  // each frame and in turn use to determine the rotation
  // by y and x axis
  double rotation_rad = 0;

#ifdef DEBUG
  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);

  long frames_drawn = 0;
#endif

  // for calculating how long a frame takes -> calc'ing the sleep time
  struct timespec draw_start, draw_end;
  double draw_time, sleep_time;

  // id for drawing thread, in an attempt to increase performance.
  // that may very well not really be the bottleneck though
  pthread_t draw_thread_id;

  // get a zbuffer -- passed to and freed in draw function
  double(*zb)[lines][cols] = make_zbuf(&cp, proj_d, cube_d, lines, cols);

  while (1) {
    // main loop

    clock_gettime(CLOCK_MONOTONIC, &draw_start);

#ifdef DEBUG
    frames_drawn++;
#endif
    ncplane_erase(ncp);

    // NOTE: to spawn a thread the function must be of signature
    // void *fun(void *arg) {
    // 		return NULL;
    // }
    // we copy the zbuffer so we can pass it along and start making a new one
    void *zb_cpy = malloc(sizeof *zb);
    zb_cpy = memcpy(zb_cpy, zb, sizeof *zb);
    free(zb);

    ArgStruct thread_arg = {.zbuffer = zb_cpy,
                            .lines = lines,
                            .cols = cols,
                            .cube_dist = cube_d,
                            .x_mult = GRAPHICAL_X_MULTIPLIER,
                            .ncp = ncp};

    pthread_create(&draw_thread_id, NULL, draw_thread, &thread_arg);

    rotate_c(&cp, &rotation_rad);
    zb = make_zbuf(&cp, proj_d, cube_d, lines, cols);

    // enable quitting
    int ch;
    ch = notcurses_get_nblock(nc, nci);
    if (ch == 'q') {
      break;
    }

    notcurses_render(nc);

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

    pthread_join(draw_thread_id, NULL);
    // NOTE: usleep sleeps for usec (microseconds, millions of a second)
    usleep(sleep_time * 1e3);
  }

  // notcurses quit, restore term
  notcurses_stop(nc);

#ifdef DEBUG
  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsed = (end.tv_sec - start.tv_sec);
  elapsed += (end.tv_nsec - start.tv_nsec) / 1.0e9;

  printf("\n\nFrames drawn: %ld\nTotal time: %lf\nAverage FPS: %lf\n\n",
         frames_drawn, elapsed, (double)frames_drawn / elapsed);
#endif

  return 0;
}
