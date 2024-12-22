// 3D Point -- should this be a vector?
typedef struct {
  double x;
  double y;
  double z;
} Point;

// Vector? Useless?
typedef struct {
  Point pos;
  int len;
} Vector;

// Line between two points
typedef struct {
  // array of all points
  // 8 now, but could be dynamic later
  Point points[64];
} Line;

// a plane is a collection of lines
// to create a plane you take two PARALLEL lines,
// and then draw lines between each corresponding point
// in each line (so line1[0] to line2[0], line1[5] to line2[5], etc)

// could probably be generalized better, but i am just
// trying to make a cube and also idrk this math, so KISS
typedef struct {
  // keep in mind each corner is, given line length of 8:
  // lines[0][0], lines[0][7], lines[7][0], lines[7][7]
  Line lines[64];
} Plane;

// now to make a cube, we need 2 hori lines up top and a plane between
// 2 hori lines down below and a plane between
// verti planes between the upper and lower hori line pairs
// and finally left/right hand side planes

// now to make a cube
// we make a horizontal, flat plane
// clone it y coords below
// make a vertical plane between each corresponding corner
// of top plane and bottom plane

// i.e. a cube is a collection of 6 planes
// generalizing it, we want to calculate it from only a starting coord
// (which is the center of the entire cube)
// and one side length
// we can refer to the planes as
// top, bottom, north, east, south, west

// you will be able to loop through all points in the cube
// by doing
// ---
// for (int i = 0; i < 6; i++)
// 	for (int j = 0; j < Cube.side; j++)
// 		for (int k = 0; k < Cube.side; k++)
// 			Cube.planes[i].lines[j].points[k]
// ---
// which seems horribly inefficient.
// the other option is to make 8 points, calc all the planes,
// only rotate the points and remake the entire cube.
typedef struct {
  // center point (0, 0, 0) is assumed
  // Point center;
  int side;
  Plane planes[6];
} Cube;

// here is a 'cube' that you will actually rotate
// you then pass it to a render_cube function to calculate the planes
typedef struct {
  int side;
  Point corners[8];
} CubePoints;

typedef struct {
  int x;
  int y;
  double distance;
} ProjectedPoint;
// would you draw each "pixel" by finding what is closest,
// or would you draw each point? drawing pixels seems more efficient but
// a lot more complicated...

// loop through each point in the cube
// calculate angle from camera to point -> choose screen coordinate
// if coordinate already occupied:
// calculate distance to camera; if new dist is smaller, replace in buffer
// if angle already exists, but this one is closer, replace in buffer with
// this point -- something like this?

// center point is assumed to be (0, 0, 0)
// camera will be at (0, 0, side * 2) or such (assuming +z is towards camera)
CubePoints new_cube(double side_len);
Cube render_cube(CubePoints cp);

void x_rotation(CubePoints *cp, double theta);
void y_rotation(CubePoints *cp, double theta);

// void *make_zuffer(Cube *c, int rows, int cols);
// ProjectedPoint (*make_zuffer(Cube *c, int rows, int cols))[];
void *make_zuffer(CubePoints *cp, int rows, int cols);
