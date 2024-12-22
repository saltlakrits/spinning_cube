// 3D Point -- should this be a vector?
typedef struct {
  double x;
  double y;
  double z;
} Point;

// Line between two points
typedef struct {
  Point points[64];
} Line;

// a plane is a collection of lines
// built from two parallel lines (that
// are drawn in the SAME DIRECTION)
typedef struct {
  Line lines[64];
} Plane;

// here is a 'cube' that you will actually rotate
// you then pass it to a render_cube function to calculate the planes
typedef struct {
  int side;
  Point corners[8];
} CubePoints;

// the full cube is only drawn once per frame, then
// discarded before rotating only the corners
typedef struct {
  int side;
  Plane planes[6];
} Cube;

typedef struct {
  int x;
  int y;
  double distance;
} ProjectedPoint;

CubePoints new_cube(double side_len);
Cube render_cube(CubePoints cp);

void x_rotation(CubePoints *cp, double theta);
void y_rotation(CubePoints *cp, double theta);

// void *make_zuffer(Cube *c, int rows, int cols);
// ProjectedPoint (*make_zuffer(Cube *c, int rows, int cols))[];
void *make_zuffer(CubePoints *cp, double proj_d, double cube_d, int rows, int cols);
