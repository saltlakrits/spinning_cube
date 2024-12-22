#include "cube.h"
#include <math.h>
#include <stdlib.h>
// #include <string.h>
#include <ncurses.h>

static Point new_point(double x, double y, double z) {
  Point n_p;
  n_p.x = x;
  n_p.y = y;
  n_p.z = z;

  return n_p;
}

static Line new_line(Point start, Point end) {

  int n = 64;
  Line new_line;

  for (int i = 1; i < n; i++) {
    double t = ((double)i) / (n - 1);

    new_line.points[i] = new_point(start.x + t * (end.x - start.x),
                                   start.y + t * (end.y - start.y),
                                   start.z + t * (end.z - start.z));
  }

  return new_line;
}

static Plane new_plane(Line first, Line second) {
  // note that first and second must be lines
  // perpendicular to each other, and their points
  // should be parallel to each other.
  Plane new_plane;

  int n = 64;
  for (int i = 0; i < n; i++) {
    // for each point in the starting line we draw another line
    // to the corresponding point in the ending line, to create a plane
    new_plane.lines[i] = new_line(first.points[i], second.points[i]);
  }

  return new_plane;
}

// note that the names of the points here will be meaningless
// once you start rotating them, they only make sense during
// the initialization

// const int half_side = side_len / 2;
// side_len ignored rn, 8 hardcoded
CubePoints new_cube(double side_len) {

  const double half_side = side_len / 2;

  // west is x-, east is x+
  // bottom is y-, top is y+
  // south is z+, north is z-
  Point top_north_west = new_point(-half_side, half_side, (-half_side));
  Point top_north_east = new_point(half_side, half_side, (-half_side));

  Point top_south_west = new_point(-half_side, half_side, half_side);
  Point top_south_east = new_point(half_side, half_side, half_side);

  Point bottom_north_west = new_point(-half_side, -half_side, (-half_side));
  Point bottom_north_east = new_point(half_side, -half_side, (-half_side));

  Point bottom_south_west = new_point(-half_side, -half_side, half_side);
  Point bottom_south_east = new_point(half_side, -half_side, half_side);

  CubePoints new_cp = {.side = side_len,
                       .corners = {top_north_west, top_north_east,
                                   top_south_west, top_south_east,
                                   bottom_north_west, bottom_north_east,
                                   bottom_south_west, bottom_south_east}};

  return new_cp;
}

Cube render_cube(CubePoints cp) {

  // calculate lines between corners
  Line top_north = new_line(cp.corners[0], cp.corners[1]);
  Line top_south = new_line(cp.corners[2], cp.corners[3]);

  Plane top = new_plane(top_north, top_south);

  Line bottom_north = new_line(cp.corners[4], cp.corners[5]);
  Line bottom_south = new_line(cp.corners[6], cp.corners[7]);

  Plane bottom = new_plane(bottom_north, bottom_south);
  Plane north = new_plane(top_north, bottom_north);
  Plane south = new_plane(top_south, bottom_south);

  Line top_west = new_line(cp.corners[0], cp.corners[2]);
  Line bottom_west = new_line(cp.corners[4], cp.corners[6]);

  Plane west = new_plane(top_west, bottom_west);

  Line top_east = new_line(cp.corners[1], cp.corners[3]);
  Line bottom_east = new_line(cp.corners[5], cp.corners[7]);

  Plane east = new_plane(top_east, bottom_east);

  Cube new_cube = {.side = cp.side,
                   .planes = {top, bottom, north, east, south, west}};

  return new_cube;
}

// rotate CubePoints
// maybe pass an enum for CLOCKWISE, COUNTERCLOCKWISE
// maybe another enum for X, Y, Z to determine which axis to rotate along
// should be able to combine rotates

// basically just copied these matrices from the web
void x_rotation(CubePoints *cp, double theta) {
  /*
   * transformation matrices:
   *
   * rotate a point around x axis:
   * [1 0 0]
   * [0 cos(theta) -sin(theta)]
   * [0 sin(theta) cos(theta)]
   *
   * should be multiplied by
   * [x]
   * [y]
   * [z]
   *
   * which looks like this:
   * (x * 1 + y * 0 + z  *0) = x'
   * (x * 0 + y * cos(theta) + z * -sin(theta)) = y'
   * (x * 0 + y * sin(theta) + z * cos(theta)) = z'
   *
   * furthermore, to do combined rotations, you would first
   * multiply the matrices with each other (e.g. multiply
   * x rotation matrix with y rotation matrix, then multiply
   * that with a vector to get new coords)
   *
   * in code, i think you can just perform one
   * rotation before the next instead of multiplying matrices,
   * i am not sure if there is a practical difference in result
   * or efficiency.
   */

  // might need to use doubles for x, y, z before casting final result
  for (int i = 0; i < 8; i++) {
    double x = cp->corners[i].x;
    double y = cp->corners[i].y;
    double z = cp->corners[i].z;

    // x = (x * 1 + x * 0 + x * 0); // does nothing so skippable
    double new_y = (y * cos(theta) - z * sin(theta));
    double new_z = (y * sin(theta) + z * cos(theta));
    cp->corners[i] = new_point(x, new_y, new_z);
  }
}

void y_rotation(CubePoints *cp, double theta) {
  /*
	 * rotation around y axis:
	 * [ cos(θ)  0  sin(θ) ]
	 * [ 0       1   0     ]
	 * [ -sin(θ) 0  cos(θ) ]
	 * [x]
	 * [y]
	 * [z]
   */

  for (int i = 0; i < 8; i++) {
    double x = cp->corners[i].x;
    double y = cp->corners[i].y;
    double z = cp->corners[i].z;

    double new_x = (x * cos(theta) + z * sin(theta));
    double new_z = (x * (-sin(theta)) + z * cos(theta));
    cp->corners[i] = new_point(new_x, y, new_z);
  }
}

static ProjectedPoint to_2d(Point p, double proj_d, double cube_d) {

  // camera is at origin, (0, 0, 0)

  // projection plane is at some distance from viewer.
  // this is constant for the calculation,
  // but arbitrarily chosen as far as i can tell.
	// well, not arbitrarily, because the choosing how
	// close to the camera the projection plane is
	// as well as how far you move the object is key
	// to making it appear as you want

  // throw the point way back
  double pushed_z = p.z - cube_d;

  // now we find new x and y
  // y' = y * z' / z
  // obviously same for x'
  double new_x = p.x * (-proj_d / pushed_z);
  double new_y = p.y * (-proj_d / pushed_z);

  // we also need the distance from camera to point
  double d = sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(pushed_z, 2));

  ProjectedPoint new_point = {.x = new_x, .y = new_y, .distance = d};
  return new_point;
}

// not a real zbuffer, just how i imagine it, if the
// performance is wildly bad i can look up how to
// do this properly
void *make_zuffer(CubePoints *cp, double proj_d, double cube_d, int rows,
                  int cols) {
  // Cubes have 6 planes
  // planes have n lines
  // lines have n points

  Cube c = render_cube(*cp);

  // VLA pointer (variably modified type)
  ProjectedPoint(*zuffer)[rows][cols] = (malloc(sizeof *zuffer));

  // initialize for comparison
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      ProjectedPoint init_prp = {.distance = INFINITY};
      (*zuffer)[y][x] = init_prp;
    }
  }

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 64; j++) {
      for (int k = 0; k < 64; k++) {

        // this gigantic for loop feels wrong, but i can't
        // think of a shortcut...

        ProjectedPoint pr_p =
            to_2d(c.planes[i].lines[j].points[k], proj_d, cube_d);
        int x_index = (cols / 2) + pr_p.x;
        int y_index = (rows / 2) + pr_p.y;
        if (x_index < 0 || x_index > cols || y_index < 0 || y_index > rows) {
          // if pixel doesn't fit, discard it
          continue;
        }
        if ((*zuffer)[y_index][x_index].distance > pr_p.distance) {
          (*zuffer)[y_index][x_index] = pr_p;
        }
      }
    }
  }
  // returns a alloc'd 2d array, need to free all of the subarrays and the big
  // array in main loop after printing
  return zuffer;
}
// macro to ensure that when we call this function, the result is
// casted appropriately
#define make_zuffer(C, ROWS, COLS)                                             \
  ((ProjectedPoint(*)[ROWS][COLS])make_zuffer(C, ROWS, COLS))
