//---------------------------------------------------------------------
// 
// demo1.cpp -- Demonstrates the use of Pitteway's algorithm for
//     drawing rotated ellipses. This version of the demo runs on the
//     Simple DirectMedia Library (SDL2) in Windows. 
//
//---------------------------------------------------------------------

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h> 
#include <assert.h>
#include "conic.h"
#include "demo.h"

// Global renderer and color values used by the DrawPixel function
SDL_Renderer *g_renderer = 0;

// DrawPixel function is used by Line and Conic functions to set the
// pixel at drawing coordinates (x,y) to the currently selected color
//
void DrawPixel(int x, int y)
{
    SDL_RenderDrawPoint(g_renderer, x, y);
}

// SetColor function is used by the functions in this module to
// specify the color to use for subsequent drawing operations
//
inline void SetColor(COLOR rgb)
{
    Uint8 r = rgb & 255;
    Uint8 g = (rgb >> 8) & 255;
    Uint8 b = (rgb >> 16) & 255;

    SDL_SetRenderDrawColor(g_renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

// Draws the minimum bounding box for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse.
void BoundingBox(int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    int xbox, ybox;
    SDL_Rect bbox;

    // Translate center of ellipse to origin
    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;
    xbox = sqrt(xp*xp + xq*xq) + 0.5;
    ybox = sqrt(yp*yp + yq*yq) + 0.5;
    bbox.x = x0-xbox;
    bbox.y = y0-ybox;
    bbox.w = 2*xbox + 1;
    bbox.h = 2*ybox + 1;
    SDL_RenderFillRect(g_renderer, &bbox);
}

// Draws the enclosing polygon for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse. The polygon
// has 8 sides: two opposing sides are vertical, two are hori-
// zontal, two are +45-degree diagonals, and two are -45-degree
// diagonals. Each side of the polygon touches the ellipse at
// exactly one point, at which the side is tangent to the ellipse.
void BoundingPgon(int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float tmp1, tmp2;
    float X, Y, Z, W, U, V;
    SDL_Point xy[9];

    // Translate center of ellipse to origin
    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;

    // Compute vertexes of bounding polygon
    X = sqrt(xp*xp + xq*xq);
    Y = sqrt(yp*yp + yq*yq);
    tmp1 = xp + yp;
    tmp2 = xq + yq;
    Z = sqrt(tmp1*tmp1+tmp2*tmp2);
    tmp1 = xp - yp;
    tmp2 = xq - yq;
    W = sqrt(tmp1*tmp1+tmp2*tmp2);

    xy[0].x = x0 + 0.5 + X;
    xy[0].y = y0 + 0.5 - X + Z;

    xy[1].x = x0 + 0.5 - Y + Z;
    xy[1].y = y0 + 0.5 + Y;

    xy[2].x = x0 + 0.5 + Y - W;
    xy[2].y = y0 + 0.5 + Y;

    xy[3].x = x0 + 0.5 - X;
    xy[3].y = y0 + 0.5 - X + W;

    xy[4].x = x0 + 0.5 - X;
    xy[4].y = y0 + 0.5 + X - Z;

    xy[5].x = x0 + 0.5 + Y - Z;
    xy[5].y = y0 + 0.5 - Y;

    xy[6].x = x0 + 0.5 - Y + W;
    xy[6].y = y0 + 0.5 - Y;

    xy[7].x = x0 + 0.5 + X;
    xy[7].y = y0 + 0.5 + X - W;

    xy[8].x = xy[0].x;  // close polyline
    xy[8].y = xy[0].y;

    // Draw eight sides of bounding polygon
    SDL_RenderDrawLines(g_renderer, xy, 9);
}

// Draws an 8-sided polygon inscribed in an ellipse specified
// by its center point (x0,y0) and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse. Each
// vertex of the polygon is a point at which the drawing
// octant changes in the ellipse-drawing algorithm. This is
// the same point at which one vertical, horizontal, or diagonal
// side of the bounding polygon (drawn by the BoundingPolygon
// function) touches (and is tangent to) the ellipse.
void InscribedPgon(int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float tmp1, tmp2;
    float X, Y, Z, W, U, V;
    SDL_Point xy[9];

    // Translate center of ellipse to origin
    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;

    // Compute vertexes of framing polygon
    X = sqrt(xp*xp + xq*xq);
    Y = sqrt(yp*yp + yq*yq);
    tmp1 = xp + yp;
    tmp2 = xq + yq;
    Z = sqrt(tmp1*tmp1+tmp2*tmp2);
    tmp1 = xp - yp;
    tmp2 = xq - yq;
    W = sqrt(tmp1*tmp1+tmp2*tmp2);
    if (X == 0 || Y == 0 || Z == 0 || W == 0)
    {
        return;  // degenerate ellipse
    }
    U = xp*(xp + yp) + xq*(xq + yq);
    V = xp*(xp - yp) + xq*(xq - yq);
    tmp1 = xp*yp + xq*yq;

    xy[0].x = x0 + 0.5 + X;
    xy[0].y = y0 + 0.5 + tmp1/X;

    xy[1].x = x0 + 0.5 + U/Z;    
    xy[1].y = y0 + 0.5 - U/Z + Z;

    xy[2].x = x0 + 0.5 + tmp1/Y;
    xy[2].y = y0 + 0.5 + Y;

    xy[3].x = x0 + 0.5 - V/W;    
    xy[3].y = y0 + 0.5 - V/W + W;

    xy[4].x = x0 + 0.5 - X;      
    xy[4].y = y0 + 0.5 - tmp1/X; 

    xy[5].x = x0 + 0.5 - U/Z;    
    xy[5].y = y0 + 0.5 + U/Z - Z;

    xy[6].x = x0 + 0.5 - tmp1/Y; 
    xy[6].y = y0 + 0.5 - Y;      

    xy[7].x = x0 + 0.5 + V/W;    
    xy[7].y = y0 + 0.5 + V/W - W;

    xy[8].x = xy[0].x;  // close polyline
    xy[8].y = xy[0].y;

    // Connect eight vertexes of inscribed polygon
    SDL_RenderDrawLines(g_renderer, xy, 9);
}

// Draws the major and minor axes for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse.
void DrawAxes(int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float A, B, C, F;
    float xprod, beta, root, slope, denom, x, y;
    SDL_Point xy[2];

    // Translate center of ellipse to origin
    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;

    // Calculate coefficients A,B,C,F for ellipse equation
    A = yp*yp + yq*yq;
    B = -2*(xp*yp + xq*yq);
    if (B == 0)
    {
        return;  // an ellipse in standard position, or a circle
    }
    C = xp*xp + xq*xq;
    xprod = xq*yp - xp*yq;
    if (xprod == 0)
    {
        return;  // area of ellipse is zero
    }
    F = -xprod*xprod;
    beta = (C - A)/B;
    root = sqrt(beta*beta + 1.0);
    slope = beta + root;

    // Each for-loop iteration draws an axis
    for (int i = 0; i < 2; i++)
    {
        denom = A + (B + C*slope)*slope;
        if (denom <= 0)
        {
            return;  // ultra-thin ellipse
        }
        x = sqrt(-F/denom);
        y = slope*x;
        xy[0].x = x0 + 0.5 + x;      
        xy[0].y = y0 + 0.5 + y;
        xy[1].x = x0 + 0.5 - x;
        xy[1].y = y0 + 0.5 - y;
        SDL_RenderDrawLines(g_renderer, xy, 2);
        slope = beta - root;
    } 
}

// Draws the updated ellipse. Also draws the parallelogram in which
// the ellipse is incribed, the bounding box for the ellipse, the
// bounding polygon for the ellipse, the polygon inscribed in the
// ellipse, and the major and minor axes of the ellipse.
void UpdateEllipse(Bounce *bounce)
{
    SDL_Rect rect;
    SDL_Point xy[5];
    int x0, y0, x1, y1, x2, y2;
    COLOR color[] = { ORANGE, GREEN, MAGENTA, YELLOW };

    // Update coordinates for animated parallelogram
    bounce->Update((XYVAL*)xy);  
    xy[4] = xy[0];       // close polyline

    // Get ellipse center point and ends of conjugate diameters
    x0 = (xy[1].x + xy[3].x)/2;  // P0 = center of ellipse
    y0 = (xy[1].y + xy[3].y)/2;
    x1 = (xy[1].x + xy[2].x)/2;  // P1 = end of 1st conjugate diameter
    y1 = (xy[1].y + xy[2].y)/2;
    x2 = (xy[2].x + xy[3].x)/2;  // P2 = end of 2nd conjugate diameter
    y2 = (xy[2].y + xy[3].y)/2;

    // Fill bounding box around ellipse
    SetColor(DARKGREEN);
    BoundingBox(x0, y0, x1, y1, x2, y2);

    // Highlight vertices of parallelogram
    for (int i = 0; i < 4; i++)
    {
        rect.x = xy[i].x-2;
        rect.y = xy[i].y-2;
        rect.w = rect.h = 5;
        SetColor(color[i]);
        SDL_RenderFillRect(g_renderer, &rect);
    }

    // Draw four sides of parallelogram
    SetColor(BLUE);
    SDL_RenderDrawLines(g_renderer, xy, 5);

    // Draw 8-sided bounding polygon around ellipse
    SetColor(RED);
    BoundingPgon(x0, y0, x1, y1, x2, y2);

    // Draw 8-sided polygon inscribed in ellipse
    SetColor(MAGENTA);
    InscribedPgon(x0, y0, x1, y1, x2, y2);

    // Draw major and minor axes of ellipse
    SetColor(YELLOW);
    DrawAxes(x0, y0, x1, y1, x2, y2);

    // Draw ellipse inscribed in parallelogram
    SetColor(WHITE);
    Ellipse(x0, y0, x1, y1, x2, y2);
}

//---------------------------------------------------------------------
//
// Main program
//
//---------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int retval = 0;

    printf("Starting SDL2 app...\n");
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SDL_Window *window = 0;

        if (SDL_CreateWindowAndRenderer(DEMO_WIDTH, DEMO_HEIGHT, 0, &window, &g_renderer) == 0)
        {
            bool quit = false;
            int redraw = -1;
            int width, height;
            SDL_Rect frame;
            Bounce *bounce = 0;

            SDL_GetWindowSize(window, &width, &height);
            bounce = new Bounce(width, height);
            frame.x = 2;
            frame.y = 2;
            frame.w = width - 4;
            frame.h = height - 4;
            while (!quit)
            {
                SDL_Event evt;

                if (redraw)
                {
                    --redraw;
                    SetColor(BLACK);
                    SDL_RenderClear(g_renderer);
                    SetColor(GRAY);
                    SDL_RenderDrawRect(g_renderer, &frame);
                    UpdateEllipse(bounce);
                    SDL_RenderPresent(g_renderer);
                }
                SDL_WaitEventTimeout(0, 17);
                while (SDL_PollEvent(&evt))
                {
                    if (evt.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if (evt.type == SDL_KEYDOWN)
                    {
                        switch (evt.key.keysym.sym)
                        {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        default:
                            redraw = 1;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            printf("--ERROR-- %s\n", SDL_GetError());
            retval = -2;
        }
        if (g_renderer)
        {
            SDL_DestroyRenderer(g_renderer);
        }
        if (window)
        {
            SDL_DestroyWindow(window);
        }
    }
    else
    {
        printf("--ERROR-- %s\n", SDL_GetError());
        retval = -1;
    }
    printf("Quitting SDL2 app...\n");
    SDL_Quit();
    return retval;
}


