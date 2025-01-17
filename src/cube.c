#include "cube.h"
#include <math.h>
#include <stdlib.h>

static struct Point new_point(double x, double y, double z) {
  struct Point n_p;
  n_p.x = x;
  n_p.y = y;
  n_p.z = z;

  return n_p;
}

static struct Point *new_line(struct Point *line, int side, struct Point start,
                              struct Point end) {

  for (int i = 0; i < side; i++) {
    double t = ((double)i) / (side - 1);

    line[i] = new_point(start.x + t * (end.x - start.x),
                        start.y + t * (end.y - start.y),
                        start.z + t * (end.z - start.z));
  }

  return line;
}

struct CubePoints new_cube(double side_len) {

  const double half_side = side_len / 2;

  /* NOTE: that the names of the points here will be meaningless
   * once you start rotating them, they are only for making
   * sense out of "building" the cube
   */
  // west is x-, east is x+
  // bottom is y-, top is y+
  // south is z+, north is z-
  struct Point top_north_west = new_point(-half_side, half_side, (-half_side));
  struct Point top_north_east = new_point(half_side, half_side, (-half_side));

  struct Point top_south_west = new_point(-half_side, half_side, half_side);
  struct Point top_south_east = new_point(half_side, half_side, half_side);

  struct Point bottom_north_west =
      new_point(-half_side, -half_side, (-half_side));
  struct Point bottom_north_east =
      new_point(half_side, -half_side, (-half_side));

  struct Point bottom_south_west = new_point(-half_side, -half_side, half_side);
  struct Point bottom_south_east = new_point(half_side, -half_side, half_side);

  struct CubePoints new_cp = {
      .side = side_len,
      .corners = {top_north_west, top_north_east, top_south_west,
                  top_south_east, bottom_north_west, bottom_north_east,
                  bottom_south_west, bottom_south_east}};

  return new_cp;
}

static void *add_planes(void *cube, int plane_num, int side_len,
                        struct Point p1, struct Point p2, struct Point p3,
                        struct Point p4) {
  struct Point *first_line = malloc(side_len * sizeof *first_line);
  struct Point *second_line = malloc(side_len * sizeof *second_line);

  first_line = new_line(first_line, side_len, p1, p2);
  second_line = new_line(second_line, side_len, p3, p4);

  // need to cast the void ptr to index it (locally)
  struct Point(*casted_cube)[CUBE_SIDES][side_len][side_len] = cube;

  // for each point in the two above lines,
  // we want to draw a perpendicular line between
  // each pair of parallel points in the lines
  for (int line = 0; line < side_len; line++) {
    struct Point start = first_line[line];
    struct Point end = second_line[line];

    for (int point = 0; point < side_len; point++) {
      double t = ((double)point) / (side_len - 1);

      (*casted_cube)[plane_num][line][point] = new_point(
          start.x + t * (end.x - start.x), start.y + t * (end.y - start.y),
          start.z + t * (end.z - start.z));
    }
  }

  free(first_line);
  free(second_line);
  return cube;
}

static void *render_cube(struct CubePoints cp, int side_len) {

  // cp.corners easier access in for loop
  int crn[CUBE_SIDES][4] = {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 1, 4, 5},
                            {2, 3, 6, 7}, {0, 2, 4, 6}, {1, 3, 5, 7}};

  // let's try VLA type again
  struct Point(*cube)[CUBE_SIDES][side_len][side_len] = malloc(sizeof *cube);
  for (int i = 0; i < CUBE_SIDES; i++) {
    cube = add_planes(cube, i, side_len, cp.corners[crn[i][0]],
                      cp.corners[crn[i][1]], cp.corners[crn[i][2]],
                      cp.corners[crn[i][3]]);
  }

  return cube;
}
// macro to ensure that when we call this function, the result is
// casted appropriately
#define render_cube(CP, SIDE)                                                  \
  ((struct Point(*)[CUBE_SIDES][SIDE][SIDE])render_cube(CP, SIDE))

// basically just copied these matrices from the web
void x_rotation(struct CubePoints *cp, double theta) {
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

  for (int i = 0; i < CUBE_CORNERS; i++) {
    double x = cp->corners[i].x;
    double y = cp->corners[i].y;
    double z = cp->corners[i].z;

    // x = (x * 1 + x * 0 + x * 0); // does nothing so skippable
    double new_y = (y * cos(theta) - z * sin(theta));
    double new_z = (y * sin(theta) + z * cos(theta));
    cp->corners[i] = new_point(x, new_y, new_z);
  }
}

void y_rotation(struct CubePoints *cp, double theta) {
  /*
   * rotation around y axis:
   * [ cos(θ)  0  sin(θ) ]
   * [ 0       1   0     ]
   * [ -sin(θ) 0  cos(θ) ]
   * [x]
   * [y]
   * [z]
   */

  for (int i = 0; i < CUBE_CORNERS; i++) {
    double x = cp->corners[i].x;
    double y = cp->corners[i].y;
    double z = cp->corners[i].z;

    double new_x = (x * cos(theta) + z * sin(theta));
    double new_z = (x * (-sin(theta)) + z * cos(theta));
    cp->corners[i] = new_point(new_x, y, new_z);
  }
}

void rotate_c(struct CubePoints *cp, double *rotation_rad,
              const double framerate, const double rotation_constant) {
  *rotation_rad += (2.0 * M_PI) / (framerate * 100);
  *rotation_rad = fmod(*rotation_rad, (2.0 * M_PI));

  x_rotation(cp, cos(*rotation_rad) * rotation_constant);
  y_rotation(cp, sin(*rotation_rad) * rotation_constant);
}

static struct ProjectedPoint to_2d(struct Point p, double proj_d,
                                   double cube_d) {

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

  struct ProjectedPoint new_point = {.x = new_x, .y = new_y, .distance = d};
  return new_point;
}

void *make_zbuf(struct CubePoints *cp, double proj_d, double cube_d, int rows,
                int cols) {

  // render cube, cast the void ptr
  struct Point(*c)[CUBE_SIDES][cp->side][cp->side] = render_cube(*cp, cp->side);

  // VLA pointer (variably modified type)
  // super convenient! VLAs without actually using VLAs!
  // this MUST be supported in C23, so while we're not there yet,
  // might as well make use of it.
  // (but the syntax for it is atrocious. oh well!)
  double(*zbuffer)[rows][cols] = malloc(sizeof *zbuffer);

  // initialize zbuffer with inf values
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      (*zbuffer)[y][x] = INFINITY;
    }
  }

  int x_index, y_index;
  // ugly nested for loop, but well... it's 3 dimensions.
  // all the memory in the cube should be adjacent to each other
  // as far as i understand, so all the indirection should be fine
  for (int side = 0; side < CUBE_SIDES; side++) {
    for (int line = 0; line < cp->side; line++) {
      for (int point = 0; point < cp->side; point++) {
        struct ProjectedPoint pr_p =
            to_2d((*c)[side][line][point], proj_d, cube_d);
        // offset by half a screen width and height
        x_index = (cols / 2) + pr_p.x;
        y_index = (rows / 2) + pr_p.y;

        // if it doesn't fit, discard
        if (x_index < 0 || x_index > cols || y_index < 0 || y_index > rows) {
          continue;
        }

        // if it is closer to camera than whether is already in the buffer,
        // replace in buffer
        if ((*zbuffer)[y_index][x_index] > pr_p.distance) {
          (*zbuffer)[y_index][x_index] = pr_p.distance;
        }
      }
    }
  }

  free(c);
  return zbuffer;
}
// macro to ensure that when we call this function, the result is
// casted appropriately
#define make_zbuf(C, ROWS, COLS)                                               \
  ((double(*)[ROWS][COLS])make_zbuf(C, ROWS, COLS))
