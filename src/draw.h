typedef struct {
	void *zbuffer;
	int lines;
	int cols;
	double cube_dist;
	int x_mult;
} ArgStruct;

void *draw_thread(void *arg);
