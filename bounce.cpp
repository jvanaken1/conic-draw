//---------------------------------------------------------------------
// 
// bounce.cpp -- Bounce class member functions 
//
//---------------------------------------------------------------------

#include "demo.h" 

// Constructor
Bounce::Bounce(int width, int height) :
            xmin(EDGE_OFFSET), ymin(EDGE_OFFSET), 
            xmax(width-EDGE_OFFSET), ymax(height-EDGE_OFFSET)
{
    XYVAL vxy[] = { { 0, 0 }, { 5, 3 }, { 9, 7 }, { 6, 2 } };

    for (int i = 0; i < 4; ++i)
        vpt[i] = vxy[i];

    // Arrange the four starting points in a square
    pt[0].x = xmin + 5;
    pt[0].y = ymin + 5;
    pt[1].x = xmax - 5;
    pt[1].y = ymin + 5;
    pt[2].x = xmax - 5;
    pt[2].y = ymax - 5;
    pt[3].x = xmin + 5;
    pt[3].y = ymax - 5;
}

//---------------------------------------------------------------------
// Update function -- Copies the x-y coordinates for the four vertexes
// of the current parallelogram to output array xy. Then moves the
// vertexes to their updated positions in preparation for the next
// Update call. Points J = pt[0] and K = pt[2] are opposing vertexes
// of the parallelogram, as are P = pt[1] and Q = pt[3]. A parallelo-
// gram is uniquely defined by three vertexes, P, K, and Q; the fourth
// vertex, J, is determined by symmetry. Note: vpt[0] is unused.
//---------------------------------------------------------------------
void Bounce::Update(XYVAL xy[])
{
    int i, diff;

    for (i = 0; i < 4; ++i)
    {
        xy[i] = pt[i];
    }

    // Update P, K, and Q
    for (i = 1; i < 4; ++i)
    {
        pt[i].x += vpt[i].x;
        pt[i].y += vpt[i].y;
    }

    // Make sure that the x and y offsets between K and the
    // two vertexes P and Q are even so that the integer
    // coordinates of the conjugate diameters will exactly
    // divide the sides of the parallelogram
    for (i = 1; i < 4; i += 2)
    {
        diff = pt[i].x - pt[2].x;
        if (diff & 1)
        {
            pt[i].x += (diff > 0) ? 1 : -1;
        }
        diff = pt[i].y - pt[2].y;
        if (diff & 1)
        {
            pt[i].y += (diff > 0) ? 1 : -1;
        }
    }

    // Check for collisions with the boundaries of
    // the rectangular drawing window
    for (i = 1; i < 4; ++i)
    {
        if (pt[i].x < xmin || xmax <= pt[i].x)
        {
            vpt[i].x = -vpt[i].x;
            if (pt[i].x < xmin)
            {
                pt[i].x = 2*xmin - pt[i].x;
            }
            else
            {
                pt[i].x = 2*xmax - pt[i].x - 1;
            }                        
        }
        if (pt[i].y < ymin || ymax <= pt[i].y)
        {
            vpt[i].y = -vpt[i].y;
            if (pt[i].y < ymin)
            {
                pt[i].y = 2*ymin - pt[i].y;
            }
            else
            {
                pt[i].y = 2*ymax - pt[i].y - 1;
            }                        
        }
    }

    // To update J, reflect K through the center of the line
    // connecting P and Q. If the resulting J is out of bounds,
    // bounce it off the side of the bounding box, then update
    // K by reflecting J through the center of line PQ.
    pt[0].x = pt[1].x + pt[3].x - pt[2].x;
    if (pt[0].x < 0 || xmax <= pt[0].x)
    {
        vpt[2].x = -vpt[2].x;
        if (pt[0].x < xmin)
        {
            pt[0].x = 2*xmin - pt[0].x;
        }
        else
        {
            pt[0].x = 2*xmax - pt[0].x - 1;
        }
        pt[2].x = pt[1].x + pt[3].x - pt[0].x;
    }
    pt[0].y = pt[1].y + pt[3].y - pt[2].y;
    if (pt[0].y < ymin || ymax <= pt[0].y)
    {
        vpt[2].y = -vpt[2].y;
        if (pt[0].y < ymin)
        {
            pt[0].y = 2*ymin - pt[0].y;
        }
        else
        {
            pt[0].y = 2*ymax - pt[0].y - 1;
        }
        pt[2].y = pt[1].y + pt[3].y - pt[0].y;
    }
}

