#pragma once

#include <notcurses/notcurses.h>

// for setting colors in main and using
// in draw.c
extern int bright;
extern int medium;
extern int dark;

// struct to pass to draw thread,
// thread functions can only take one
// argument and it has to be a void ptr
struct DrawArgs {
  void *zbuffer;
  int lines;
  int cols;
  double cube_dist;
  int x_mult;
  struct ncplane *ncp;
};

void *draw_thread(void *arg);
