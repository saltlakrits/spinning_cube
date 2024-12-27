#pragma once

#include <notcurses/notcurses.h>

extern int bright;
extern int medium;
extern int dark;

typedef struct {
	void *zbuffer;
	int lines;
	int cols;
	double cube_dist;
	int x_mult;
	struct ncplane *ncp;
} ArgStruct;

void *draw_thread(void *arg);
