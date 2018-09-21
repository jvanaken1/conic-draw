//-----------------------------------------------------------
//
// conic.cpp -- Contains functions that use Bresenham and
//     Pitteway algorithms for drawing conic curves
//
// This source code for the conic-drawing functions in this
// module is adapted from:
//     Foley, J., A. van Dam, S. Feiner, and J. Hughes,
//     Computer Graphics: Principles and Practice, 2nd ed.,
//     Addison-Wesley, 1990, 945-961. 
//
//-----------------------------------------------------------

#include <math.h>
#include "conic.h"

// Bresenham's line-drawing algorithm. Draws a straight
// line from starting point (xs,ys) to end point (xe,ye).
//
void Line(int xs, int ys, int xe, int ye)
{
    int x, y, d, a, b, diagInc, squareInc;
    int dxDiag, dyDiag, dxSquare, dySquare;

    x = xs;
    y = ys;
    a = xe - xs;
    b = ye - ys;
    if (a < 0)
    {
        a = -a;
        dxDiag = -1;
    }
    else
        dxDiag = 1;

    if (b < 0)
    {
        b = -b;
        dyDiag = -1;
    }
    else
        dyDiag = 1;

    if (a < b)
    {
        int swap = a; a = b; b = swap;
        dxSquare = 0;
        dySquare = dyDiag;
    }
    else
    {
        dxSquare = dxDiag;
        dySquare = 0;
    }
    d = 2*b - a;
    squareInc = 2*b;
    diagInc = 2*(b - a);
    for (int i = 0; i <= a; i++)
    {
        DrawPixel(x, y);
        if (d < 0)
        {
            x += dxSquare;
            y += dySquare;
            d += squareInc;
        }
        else
        {
            x += dxDiag;
            y += dyDiag;
            d += diagInc;
        }
    }
}

// Returns the octant number of a point of interest on a conic curve.
// Arguments dfdx and dfdy specify the x and y components of the
// gradient at this point. The return value is an octant number in
// the range 1 to 8.
//
int GetOctant(int dfdx, int dfdy)
{
    int oct = 0;

    if (dfdx < 0)
    {
        oct = 7;
        dfdx = -dfdx;
    }
    if (dfdy > 0)
    {
        oct ^= 3;
        dfdy = -dfdy;
    }
    if (dfdx > -dfdy)
    {
        oct ^= 1;
    }
    return ++oct;
}

// Pitteway's algorithm for drawing a conic curve. This function
// draws an arc of a conic curve given the arc's starting coordinates
// (xs,ys), ending coordinates (xe,ye), and coefficients A-F of the
// implicit conic equation
//          f(x,y) = Ax^2 + Bxy + Cy^2 + Dx + Ey + F = 0
// The algorithm assumes that the caller translates the origin to
// the starting coordinates to calculate the coefficient values A-F
// that are passed to this function. To draw a full ellipse instead
// of an arc, set xe = xs and ye = ys.
//
void Conic(int xs, int ys, int xe, int ye,
           int A, int B, int C, int D, int E, int F)
{
    int x, y, swap, octant, octantCount, pixelCount;
    int dxsquare, dysquare, dxdiag, dydiag;
    int d, u, v, k1, k2, k3, dSdx, dSdy;

    // Determine whether to draw all 8 octants or just an arc
    octant = GetOctant(D, E);    // starting octant number
    if (xs != xe || ys != ye)       
    {                           
        // Draw just an arc
        x = xe - xs;             // origin at (xs,ys)
        y = ye - ys;
        dSdx = 2*A*x + B*y + D;  // gradient at end point
        dSdy = B*x + 2*C*y + E;
        octantCount = GetOctant(dSdx,dSdy) - octant;
        if (octantCount < 0)
            octantCount += 8;
    }
    else
        octantCount = 8;  // draw full ellipse (8 octants)

    // Adjust parameters for starting octant
    dxdiag = dydiag = 1;
    dxsquare = dysquare = 0;
    if ((octant + 1) & 4)
    {
        D = -D;  // octants 3, 4, 5 and 6
        dxdiag = -1;
    }
    if ((octant - 1) & 4)
    {
        E = -E;  // octants 5, 6, 7 and 8
        dydiag = -1;
    }
    if ((octant - 1) & 2)   // octants 3, 4, 7 and 8
        B = -B;

    if (octant & 2)
    {
        dysquare = dydiag;  // octants 2, 3, 6 and 7
        swap = A;   A = C;   C = swap;
        swap = D;   D = E;   E = swap;
    }
    else
        dxsquare = dxdiag;

    // Convert to fixed-point values with 2 bits of fraction
    A *= 4;   B *= 4;   C *= 4;
    D *= 4;   E *= 4;   F *= 4;

    // Initialize drawing control parameters
    d  =  A + B/2 + C/4 + D + E/2 + F;
    u  =  A + B/2 + D;
    v  =  A + B/2 + D + E;
    k1 =  2*A;
    k2 =  2*A + B;
    k3 =  2*A + 2*B + 2*C;
    if (!(octant & 1))
    {      
        // Octant is even, so reverse signs
        d  = -d;   k1 = -k1;
        u  = -u;   k2 = -k2;
        v  = -v;   k3 = -k3;
    }

    // Each iteration of for-loop draws one octant of conic curve
    x = xs;
    y = ys;
    pixelCount = -1;
    for (;;)
    {
        // If final octant, count number of pixels to end of arc
        if (!octantCount)
        {
            if (octant & 2)  // terminate in octant 2, 3, 6 or 7
                pixelCount = 1 + abs(ye - y);
            else             // terminate in octant 1, 4, 5 or 8
                pixelCount = 1 + abs(xe - x);
        }

        // Track curve through current drawing octant
        while ((u > 0 || octant & 1) && (v < 0 || ~octant & 1))
        {
            DrawPixel(x, y);
            if (--pixelCount == 0)
                return;  // we drew all pixels in final octant

            if (d < 0)
            {
                x += dxsquare;  // square step
                y += dysquare;
                u += k1;
                v += k2;
                d += u;
            }
            else
            {
                x += dxdiag;  // diagonal step
                y += dydiag;
                u += k2;
                v += k3;
                d += v;
            }
        }

        // Cross boundary into next drawing octant
        if (--octantCount < 0)
        {
            // Failed to draw all pixels in final octant
            Line(x, y, xe, ye);  // band-aid okay here?
            return;
        } 

        // Adjust drawing parameters for new octant
        if (++octant & 1)
        {    
            // Cross square octant boundary
            d  = -d - u + v - k1 + k2;
            v  = -2*u + v - k1 + k2;
            u  = -u - k1 + k2;
            k3 = -4*k1 + 4*k2 - k3;
            k2 = -2*k1 + k2;
            k1 = -k1;
            swap = dxdiag;  dxdiag = -dydiag;  dydiag = swap;
        }
        else
        {           
            // Cross diagonal octant boundary
            d  = -d + u - v/2 + k2/2 - 3*k3/8;
            u  =  u - v + k2/2 - k3/2;
            v  = -v + k2 - k3/2;
            k1 = -k1 + 2*k2 - k3;
            k2 =  k2 - k3;
            k3 = -k3;
            swap = dxsquare;  dxsquare = -dysquare;  dysquare = swap;
        }
    }
}

// Draws the outline of a full ellipse using an incremental curve-
// tracking algorithm. The ellipse can be arbitrarily oriented. It
// is specified in terms of its center point P0, and the end points 
// P1 and P2 of a pair of conjugate diameters of the ellipse. These
// diameters can be at arbitrary angles with respect to each other.
// Described in terms of the parallelogram in which the ellipse is 
// inscribed, the ellipse touches the parallelogram at points P1 and 
// P2, which are located at the midpoints of two adjacent sides of 
// the parallelogram. The function's six arguments are the x and y
// coordinates (x0,y0) at P0, (x1,y1) at P1, and (x2,y2) at P2.
//
void DrawEllipse(int x0, int y0, int x1, int y1, int x2, int y2)
{
    int xp, yp, xq, yq, xprod, xe, ye;
    int A, B, C, D, E, F;

    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;
    A =  yp*yp + yq*yq;
    B = -2*(xp*yp + xq*yq);
    C =  xp*xp + xq*xq;
    xprod = xp*yq - xq*yp;
    if (xprod == 0)
    {
        // Draw degenerate ellipse as a straight line
        int x = sqrt(float(C)) + 0.5;
        int y = sqrt(float(A)) + 0.5;

        if (((xp-xq)^(yp-yq) | (xp+xq)^(yp+yq)) < 0)
            y = -y;  // x and y have opposite signs

        Line(x0+x, y0+y, x0-x, y0-y);
        return;
    }
    if (xprod < 0)
    {
        int swap = x1; x1 = x2; x2 = swap;
        swap = y1; y1 = y2; y2 = swap;
        swap = xp; xp = xq; xq = swap;
        swap = yp; yp = yq; yq = swap;
        xprod = -xprod;
    }
    D =  2*yq*xprod;
    E = -2*xq*xprod;
    F =  0;
    Conic(x1, y1, x1, y1, A, B, C, D, E, F);
}

// Draws a spline curve consisting of a quarter of an ellipse (that is,
// PI/2 radians of elliptical arc). The spline is specified in terms of
// its start point Ps = (xs,ys), end point Pe = (xe,ye), and control
// point Pc = (xc,yc). The arc is contained within the triangle
// formed by these three points. The curve is tangent at Ps to side
// Ps.Pc of the triangle, and is tangent at Pe to side Pe.Pc.
//
void DrawEllipticalArc(int xs, int ys, int xc, int yc, int xe, int ye)
{
    int xp, yp, xq, yq, xprod;
    int A, B, C, D, E, F;

    xp = xc - xe;
    yp = yc - ye;
    xq = xc - xs;
    yq = yc - ys;
    A =  yp*yp + yq*yq;
    B = -2*(xp*yp + xq*yq);
    C =  xp*xp + xq*xq;
    xprod = xp*yq - xq*yp;
    if (xprod == 0)
    {
        // Draw degenerate conic arc as two straight lines
        int dx = sqrt(float(C)) + 0.5;
        int dy = sqrt(float(A)) + 0.5;
        int x = xs + xe - xc;
        int y = ys + ye - yc;

        x += (xc < x) ? -dx : dx;
        y += (yc < y) ? -dy : dy;
        Line(xs, ys, x, y);
        Line(x, y, xe, ye);
        return;
    }
    if (xprod < 0)
    {
        int swap = xs; xs = xe; xe = swap;
        swap = ys; ys = ye; ye = swap;
        swap = xp; xp = xq; xq = swap;
        swap = yp; yp = yq; yq = swap;
        xprod = -xprod;
    }
    D =  2*yq*xprod;
    E = -2*xq*xprod;
    F =  0;
    Conic(xs, ys, xe, ye, A, B, C, D, E, F);
}
 
// Draws a parabolic spline, which is an arc of a parabola. The arc is 
// specified in terms of its start point Ps = (xs,ys), end point 
// Pe = (xe,ye), and control point Pc = (xc,yc). The arc is contained
// within the triangle formed by points Ps, Pc, and Pe. The arc is
// tangent at Ps to side Ps.Pc of the triangle, and is tangent at Pe to
// Pe.Pc. The shape of the curve is the same as that of a second-degree
// Bezier curve with the same three control points.
void DrawParabolicArc(int xs, int ys, int xc, int yc, int xe, int ye)
{
    int xq, yq, xr, yr, xprod;
    int A, B, C, D, E, F;

    xq = xe - xs;
    yq = ye - ys;
    xr = xc - xs;
    yr = yc - ys;
    xprod = xr*yq - xq*yr;
    if (xprod == 0)
    {
        // Draw degenerate conic arc as two lines
        int x = (xs + 2*xc + xe)/4;
        int y = (ys + 2*yc + ye)/4;
        Line(xs, ys, x, y);
        Line(x, y, xe, ye);
        return;
    }

    if (xprod < 0)
    {
        int swap = xs; xs = xe; xe = swap;
        swap = ys; ys = ye; ye = swap;
        xq = -xq;
        yq = -yq;
        xr += xq;
        yr += yq;
        xprod = -xprod;
    }
    A =  4*(yr - yq)*yr + yq*yq;
    B =  4*((xq - xr)*yr - (yr - yq)*xr) - 2*xq*yq;
    C =  4*(xr - xq)*xr + xq*xq;
    D =  4*yr*xprod;
    E = -4*xr*xprod;
    F =  0;
    Conic(xs, ys, xe, ye, A, B, C, D, E, F);
}

