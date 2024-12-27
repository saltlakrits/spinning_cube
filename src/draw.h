#pragma once

#include <notcurses/notcurses.h>

#define BRIGHT 0x00c8ff
#define MEDIUM 0x61ddff
#define DARK 0xc8e1ff
#define BG 0xFFFFFF

typedef struct {
	void *zbuffer;
	int lines;
	int cols;
	double cube_dist;
	int x_mult;
	struct ncplane *ncp;
} ArgStruct;

void *draw_thread(void *arg);
