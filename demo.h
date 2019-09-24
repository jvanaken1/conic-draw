//---------------------------------------------------------------------
//
// demo.h -- Include file for demo programs
//
//---------------------------------------------------------------------

const float PI = 3.14159265358979323846;

// Window width and height
const int DEMO_WIDTH = 1280;
const int DEMO_HEIGHT = 960;

// Offset of frame from edges of window
const int EDGE_OFFSET = 6;

// x-y coordinate pair
struct XYVAL
{
    int x; int y;
};

typedef unsigned int COLOR;
#define RGBX(r,g,b)  (COLOR)(((r)&255)|(((g)&255)<<8)|(((b)&255)<<16))  

const COLOR BLACK     = RGBX(  0,   0,   0);
const COLOR BLUE      = RGBX(  0,   0, 255);
const COLOR DARKBLUE  = RGBX(  0,   0, 175);
const COLOR DARKGRAY  = RGBX(128, 128, 128);
const COLOR DARKGREEN = RGBX(  0,  96,   0);
const COLOR GREEN     = RGBX(  0, 255,   0);
const COLOR GRAY      = RGBX(128, 128, 128);
const COLOR MAGENTA   = RGBX(255,   0, 255);
const COLOR ORANGE    = RGBX(255, 192,   0);
const COLOR RED       = RGBX(255,   0,   0);
const COLOR WHITE     = RGBX(255, 255, 255);
const COLOR YELLOW    = RGBX(255, 255,   0);

// The Bounce class generates an animated parallelogram that bounces
// off the sides of a rectangular drawing area. The pt array specifies
// the four vertexes of the parallelogram, and the vpt array specifies
// the velocities at which the vertexes are moving. The Update function
// uses the velocities to calculate the next position of the vertexes.
// All calculations use integer arithmetic.
//
class Bounce
{
public:
    XYVAL pt[4];      // vertexes (in order) J, P, K, Q
    XYVAL vpt[4];     // velocities of points -, P, K, Q
    int xmin, ymin;   // top-left corner of animation boundary
    int xmax, ymax;   // bottom-right corner of animation boundary

    Bounce(int width, int height);
    ~Bounce()
    {
    }
    void Update(XYVAL xy[]);
};
