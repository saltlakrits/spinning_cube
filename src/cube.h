#pragma once

#define CUBE_CORNERS 8
#define CUBE_SIDES 6

// 3D Point
struct Point {
  double x;
  double y;
  double z;
};

/* NOTE: here is a "cube" of points that you will actually rotate
 * during the process of drawing, a complete cube is
 * rendered from the corners
 */
struct CubePoints {
  int side;
  struct Point corners[CUBE_CORNERS];
};

// a point that has been projected on to a 2d plane
struct ProjectedPoint {
  int x;
  int y;
  double distance;
};

// makes a cube (the corners, really) with the given side length
struct CubePoints new_cube(double side_len);

// rotates around x axis by theta radians
void x_rotation(struct CubePoints *cp, double theta);
// rotates around y axis by theta radians
void y_rotation(struct CubePoints *cp, double theta);

// combined rotation (but not combined using proper math)
void rotate_c(struct CubePoints *cp, double *rotation_rad,
              const double framerate, const double rotation_constant);

// takes CubePoints and renders a full cube, as well as
// rasterizes it.
// cube_d is distance from camera to center of cube
// proj_d is the distance to the projection plane
// rows and cols are the max bounds of the zbuffer
void *make_zbuf(struct CubePoints *cp, double proj_d, double cube_d, int rows,
                int cols);
