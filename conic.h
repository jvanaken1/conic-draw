//-----------------------------------------------------------
//
// conic.h -- Include file for conic-drawing functions
//
//-----------------------------------------------------------

// Implemented by demo program
extern void DrawPixel(int x, int y);

// Implemented in conic.cpp
extern void Line(int xs, int ys, int xe, int ye);
extern int GetOctant(int dfdx, int dfdy);
extern void Conic(int xs, int ys, int xe, int ye,
                  int A, int B, int C, int D, int E, int F);
extern void DrawEllipse(int x0, int y0, int x1, int y1, int x2, int y2);
extern void DrawEllipticalArc(int xs, int ys, int xc, int yc, int xe, int ye);
extern void DrawParabolicArc(int xs, int ys, int xc, int yc, int xe, int ye);








