# Pitteway's algorithm for drawing conic curves
  
_Author:_ Jerry R. VanAken  
_Date:_ 9/23/2019

In 1967, just a couple of years after Jack Bresenham published his well known line-drawing algorithm [1], M.L.V. Pitteway published a more general version of the algorithm [2]. Pitteway's algorithm is based on similar principles, but can draw conic curves — ellipses, parabolas, and hyperbolas.

In 1990, a second edition of the classic computer graphics textbook by James Foley and Andries van Dam [3] included a section on Pitteway's algorithm. This section provides practical implementation details and partial source code listings. However, filling in the gaps and transforming this material into a working computer program is not a task for the faint-hearted.

The goal of this GitHub project is to make the material from this textbook more accessible by providing source code listings that are complete and functional but no more complicated than necessary. 

However, if you plan to use this code, you might have good reason to consider adding more tests for special cases or possibly resorting to subpixel resolution. That's because the tracking method used by the conic-drawing algorithm can miss a turn when a curve veers off too sharply relative to the dimensions of the pixel grid. Pitteway describes this problem in his original paper [2], but concludes 

>... this behaviour must be expected when the gradient changes through several octants in one step. The algorithm can be modified to permit sharper turns...if precautions are taken to prevent looping, but a more detailed study is beyond the scope of this paper.

Such considerations are also beyond the scope of the source code provided in this GitHub project.

The effect of this limitation can be seen in the ellipse-drawing demo program, which is described below. The program sometimes draws ultra-thin ellipses incorrectly due to the sharp turns at the far ends of the ellipse. In such cases, the algorithm might fail to draw all the pixels in the ellipse, or might draw extra pixels beyond the ends of the ellipse.

## Two demo programs

This GitHub project provides the C++ source code for two programs, `demo1.cpp` and `demo2.cpp`, that demonstrate Pitteway's algorithm. Included are versions of these programs that run on Linux and Windows. Note that the source code for Pitteway's algorithm and its supporting routines is written in generic C++ and is contained in a separate file, `conic.cpp`, that contains no platform-specific code. The `Conic` function in this file is largely based on the source code listed in the Foley and van Dam book.

Users typically do not directly call the `Conic` function. Instead, they call the `Ellipse`, `EllipticSpline`, and `ParabolicSpline` functions. These functions calculate the conic equation coefficients for various curves, and then pass these coefficients to the `Conic` function, which does the actual drawing.

Demo1 is an animation of a rotated ellipse that bounces off the walls of the drawing region and is squashed, stretched, and spun around in the process. Press any key to pause the free-running animation and begin single-stepping one frame at a time.

Demo2 is an animation of a simple figure composed of 24 parabolic splines (aka quadratic Bezier curves).

**How to build the demos**

The two demo programs can be built to run in Linux or Windows. This project includes the following three subdirectories:
* `linux-sdl` -- Contains the platform-specific code to run the demos on [SDL2](https://wiki.libsdl.org/FrontPage) (Simple DirectMedia Library 2.0) in Linux
* `windows-sdl` -- Contains the platform-specific code to run the demos on SDL2 in Windows
* `windows-gdi` -- Contains the platform-specific code to run the demos on the Win32 API in Windows

Each of these subdirectories contains a README.md file that gives detailed instructions on how to build the demos for the designated platform.

The main directory of this project contains the following source code files, which are used by all three versions of the two demos:

* `conic.cpp` -- Contains an implementation of Pitteway's algorithm, plus several helper functions

* `conic.h` -- The include file for the functions in `conic.cpp`

* `bounce.cpp` -- Contains the implementation of the Bounce class, which is used to animate the two demos

* `demo.h` -- The include file for the demo code in the `demo1.cpp` and `demo2.cpp` files

None of these four files contains platform-dependent code.

**Demo1 description**

In Demo1, an animated ellipse is squashed, stretched, and bounced off the sides of the window. The white ellipse shown in each frame is drawn by Pitteway's algorithm. The ellipse is surrounded by these additional figures:
1. The green filled rectangle in the background is the bounding box for the ellipse. The ellipse should touch each of the four sides of this box but never extend beyond the box. (A very thin ellipse will sometimes fail the bounding box test and either extend beyond the box or fail to touch the sides. This failure is due to the known limitation of the conic-drawing algorithm that was discussed earlier.)
2. The blue polyline is the parallelogram in which the ellipse is inscribed. The ellipse touches the parallelogram at the midpoint of each side of the parallelogram, where the ellipse is tangent to that side.
3. The red polyline is an 8-sided bounding polygon for the ellipse. This polygon has two vertical sides, two horizontal sides, and four sides that are +/- 45-degree diagonal lines. Each side touches the ellipse at a point at which the ellipse is tangent to that side.
4. The magenta polyline is an 8-sided polygon that is inscribed within the ellipse. The vertexes of _this_ polygon are the points on the ellipse at which the (red) bounding polygon touches and is tangent to the ellipse. The spaces between these two polygons neatly frame the 8 octants traversed by Pitteway's algorithm as it draws the ellipse. (These octants are described in the Foley and van Dam book.)
5. The perpendicular yellow lines passing through the center of the ellipse are the major and minor axes of the ellipse.

The purpose in showing these additional figures is to point out that they can all be constructed from the same three points that are used to specify the ellipse. The ellipse is specified in terms of its center coordinates, _P0_ = (_x0_,_y0_), and the end points, _P1_ = (_x1_,_y1_) and _P2_ = (_x2_,_y2_), of two [conjugate diameters](https://en.wikipedia.org/wiki/Conjugate_diameters) of the ellipse. In terms of the bounding parallelogram, the conjugate diameter end points are simply the midpoints of two adjacent sides of the (blue) parallelogram. Thus, specifying the conjugate diameter end points is equivalent to specifying the parallelogram in which the ellipse is inscribed.

Specifying an ellipse in terms of a bounding parallelogram (or conjugate diameters) seems much simpler and more natural than some of the more cumbersome methods that might be used — for example, specifying an angle of rotation, plus the lengths of the major and minor axes, and so on.

For more information about the Demo1 program, see the ellipse.pdf file in the main directory of this project.

**Demo2 description**

In Demo2, the figure drawn in white is an asterisk glyph (aka "splat") composed of 24 parabolic splines (aka quadratic Bezier curves). Each spline is drawn by Pitteway's algorithm. The knots and control points for the splines are highlighted in blue. The spline skeleton is also shown.

The demo uses barycentric coordinates to map the spline knots and control points to the interior of a parallelogram, whose four sides are drawn in green. As the animated parallelogram is squashed and stretched, the positions of these points change accordingly.

The splines in this demo are drawn by the `ParabolicSpline` function, but you can change the code to call the `EllipticSpline` function instead. The result will be a noticeable change in curvature of the splines.

If you're interested in learning more about conic splines, and possibly writing your own spline-drawing functions, two good sources are Theo Pavlidis' 1983 paper in ACM TOG [4] and Vaughn Pratt's SIGGRAPH 1985 paper [5].

## Implementation detail

A noteworthy feature of the Bresenham and Pitteway drawing algorithms is that they rely solely on simple integer arithmetic operations. The same is nearly always true for the functions in `conic.cpp` that support the conic-drawing algorithm.

However, I have resorted to using floating-point square root operations to handle a couple of relatively rare special cases in the `Ellipse` and `EllipticSpline` support functions. In the event that all three points passed to either function are collinear, the `sqrt` function from `math.h` is used to determine the length of the line to draw.

## References

[1] Bresenham, J.E., "Algorithm for Computer Control of a Digital Plotter," _IBM Systems Journal_, 4(1), 1965, 25-30.

[2] Pitteway, M.L.V., "Algorithm for Drawing Ellipses or Hyperbolae with a Digital Plotter," _Computer Journal_, 10(3), November 1967, 282-289.

[3] Foley, J., A. van Dam, S. Feiner, and J. Hughes, _Computer Graphics: Principles and Practice_, 2nd ed., Addison-Wesley, 1990, 945-961.

[4] Pavlidis, T., "Curve Fitting with Conic Splines," _ACM Trans. on Graphics_, 2(1), January 1983, 1-31.

[5] Pratt, V., "Techniques for Conic Splines," _SIGGRAPH 85_, 151-159.

.

.

.

.

.























