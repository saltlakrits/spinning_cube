#define CUBE_CORNERS 8
#define CUBE_SIDES 6

// 3D Point
typedef struct {
  double x;
  double y;
  double z;
} Point;

// here is a "cube" of points that you will actually rotate
// during the process of drawing, a complete cube is
// rendered from the corners
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

CubePoints new_cube(double side_len);

void x_rotation(CubePoints *cp, double theta);
void y_rotation(CubePoints *cp, double theta);

void *make_zbuffer(CubePoints *cp, double proj_d, double cube_d, int rows,
                   int cols);
