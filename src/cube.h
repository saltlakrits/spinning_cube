#pragma once

#define CUBE_CORNERS 8
#define CUBE_SIDES 6

// 3D Point
typedef struct {
  double x;
  double y;
  double z;
} Point;

/* NOTE: here is a "cube" of points that you will actually rotate
 * during the process of drawing, a complete cube is
 * rendered from the corners
 */
typedef struct {
  int side;
  Point corners[CUBE_CORNERS];
} CubePoints;

// a point that has been projected on to a 2d plane
typedef struct {
  int x;
  int y;
  double distance;
} ProjectedPoint;

// makes a cube (the corners, really) with the given side length
CubePoints new_cube(double side_len);

// rotates around x axis by theta radians
void x_rotation(CubePoints *cp, double theta);
// rotates around y axis by theta radians
void y_rotation(CubePoints *cp, double theta);

// takes CubePoints and renders a full cube, as well as
// rasterizes it.
// cube_d is distance from camera to center of cube
// proj_d is the distance to the projection plane
// rows and cols are the max bounds of the zbuffer
void *make_zbuf(CubePoints *cp, double proj_d, double cube_d, int rows,
                int cols);
