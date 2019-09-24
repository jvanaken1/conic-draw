//---------------------------------------------------------------------
// 
// demo2.cpp -- Demonstrates the use of Pitteway's algorithm for
//     drawing conic splines. This version of the demo runs on the
//     Simple DirectMedia Library (SDL2) in Windows. 
//
//---------------------------------------------------------------------

#include <SDL.h>
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

// Barycentric coordinates
struct BARYCENT
{
    float u;
    float v;
    float w;
};

// Converts x-y coordinate pair (xin,yin) to barycentric coordinates
// (u,v,w) given the three vertexes -- (xu,yu), (xv,yv), and (xw,yw)
// -- of the reference triangle.
void xyToBary(float xin, float yin, BARYCENT *uvwout, 
              float xu, float yu, float xv, float yv, float xw, float yw)
{
    BARYCENT tmp;
    tmp.u = (xv - xin)*(yw - yin) - (xw - xin)*(yv - yin);
    tmp.v = (xw - xin)*(yu - yin) - (xu - xin)*(yw - yin);
    tmp.w = (xu - xin)*(yv - yin) - (xv - xin)*(yu - yin);
    float norm = tmp.u + tmp.v + tmp.w;
    tmp.u /= norm;
    tmp.v /= norm;
    tmp.w /= norm;
    *uvwout = tmp;
}

// Converts barycentric coordinates uvwIn = (u,v,w) to x-y coordinates
// xyOut given the three vertexes -- xy[0], xy[1], and xy[2] -- of the
// reference triangle.
void baryToXy(SDL_Point *xyOut, const BARYCENT *uvwIn, SDL_Point xy[])
{
    xyOut->x = uvwIn->u*xy[0].x + uvwIn->v*xy[1].x + uvwIn->w*xy[2].x;
    xyOut->y = uvwIn->u*xy[0].y + uvwIn->v*xy[1].y + uvwIn->w*xy[2].y;
}

// Contains conic spline control points for a "splat" glyph (an
// asterisk). The control points are stored as barycentric
// coordinates relative to three triangle vertexes. This triangle
// is half of the square that contains the splat figure. When the
// square is squashed and stretched by being affine-transformed
// into a parallelogram, the splat is affine-transformed as well.
class Splat
{
    BARYCENT uvwSplat[49];  // spline control points for "splat" figure

public:
    Splat();
    ~Splat()
    {
    }
    void Update(Bounce *bounce);
};

// Initializes conic spline control points, and stores these
// points as barycentric coordinates defined relative to triangle
// vertexes u = (-1,1), v = (1,1), w = (1,-1). 
Splat::Splat()
{
    float a, xmax[14], ymax[14], x[49], y[49];
    int i, j;

    for (i = 0, a = PI/12; i < 12; i++, a += PI/6)
    {
        xmax[i] = sin(a);
        ymax[i] = cos(a);
    }
    for (i = 0; i < 12; i += 2)
    {
        float dx = (xmax[i+1] - xmax[i])/12;
        float dy = (ymax[i+1] - ymax[i])/12;

        xmax[i] -= dx;
        ymax[i] -= dy;
        xmax[i+1] += dx;
        ymax[i+1] += dy;
    }
    for (i = 12; i < 14; i++)  // x/y array wraparound
    {
        xmax[i] = xmax[i-12];
        ymax[i] = ymax[i-12];
    }
    for (i = 0, j = 1; i < 12; i += 2, j += 8)
    {
        x[j] = 0.155*xmax[i];
        y[j] = 0.155*ymax[i];

        x[j+1] = 0.433*xmax[i];
        y[j+1] = 0.433*ymax[i];

        x[j+2] = xmax[i];
        y[j+2] = ymax[i];

        x[j+3] = (xmax[i]+xmax[i+1])/2;
        y[j+3] = (ymax[i]+ymax[i+1])/2;

        x[j+4] = xmax[i+1];
        y[j+4] = ymax[i+1];

        x[j+5] = 0.433*xmax[i+1];
        y[j+5] = 0.433*ymax[i+1];

        x[j+6] = 0.155*xmax[i+1];
        y[j+6] = 0.155*ymax[i+1];

        x[j+7] = 0.155*(xmax[i+1]+xmax[i+2])/2;
        y[j+7] = 0.155*(ymax[i+1]+ymax[i+2])/2; 
    }
    x[0] = x[48];
    y[0] = y[48];

    for (i = 0; i < 49; i++)
    {
        xyToBary(x[i], y[i], &uvwSplat[i], -1.0, 1.0, 1.0, 1.0, 1.0, -1.0);
    }
}

// Update drawing of "splat" glyph on screen based on current
// position of parallelogram vertexes
void Splat::Update(Bounce *bounce)
{
    SDL_Rect rect;
    SDL_Point xy[49], xyPgram[5];
    int i;

    // Get vertex coordinates for current parallelogram
    bounce->Update((XYVAL*)xyPgram);
    xyPgram[4] = xyPgram[0];  // close polyline

    // Draw four sides of parallelogram
    SetColor(DARKGREEN);
    SDL_RenderDrawLines(g_renderer, xyPgram, 5);

    // Highlight spline knots and control points for splat glyph
    SetColor(BLUE);
    for (i = 0; i < 49; i++)
    {
        baryToXy(&xy[i], &uvwSplat[i], xyPgram);
        rect.x = xy[i].x-2;
        rect.y = xy[i].y-2;
        rect.w = rect.h = 5;
        SDL_RenderFillRect(g_renderer, &rect);
    }

    // Draw spline skeleton for splat glyph
    SetColor(DARKBLUE);
    SDL_RenderDrawLines(g_renderer, xy, 49);

    // Draw conic splines consisting of PI/2-radian elliptical arcs
    SetColor(WHITE);
    for (int i = 2; i < 49; i += 2)
    {
        ParabolicSpline(xy[i-2].x, xy[i-2].y, xy[i-1].x, xy[i-1].y, xy[i].x, xy[i].y); 
    }
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
            Splat splat;

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
                    splat.Update(bounce);
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


