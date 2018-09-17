//-----------------------------------------------------------
// 
// Demo2 -- Demonstrates use of Pitteway's algorithm for
//     drawing conic splines 
//
//-----------------------------------------------------------

#include <windows.h>
#include <math.h> 
#include <assert.h>
#include "conic.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, 
                   HINSTANCE hPrevInstance, 
                   PSTR szCmdLine, 
                   int iCmdShow) 
{ 
    static TCHAR szAppName[] = TEXT("CONIC CURVE DEMO 2"); 
    HWND         hwnd; 
    MSG          msg; 
    WNDCLASS     wndclass;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW; 
    wndclass.lpfnWndProc   = WndProc; 
    wndclass.cbClsExtra    = 0; 
    wndclass.cbWndExtra    = 0; 
    wndclass.hInstance     = hInstance; 
    wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION); 
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW); 
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
    wndclass.lpszMenuName  = NULL; 
    wndclass.lpszClassName = szAppName; 

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, 
                   TEXT("This program requires Windows NT!"), 
                   szAppName, 
                   MB_ICONERROR); 
        return 0; 
    }
    hwnd = CreateWindow(szAppName, 
                        TEXT("Conic Curve Demo 2"), 
                        WS_OVERLAPPEDWINDOW, 
                        CW_USEDEFAULT, CW_USEDEFAULT, 
                        CW_USEDEFAULT, CW_USEDEFAULT, 
                        NULL, NULL, hInstance, NULL); 

    ShowWindow(hwnd, iCmdShow); 
    UpdateWindow(hwnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
    return msg.wParam; 
}

// Specifies an animated parallelogram that bounces off the sides of
// a rectangular drawing area. The structure specifies the four
// vertexes of the parallelogram and the velocities at which the 
// vertexes are moving. Only integer arithmetic is used. 
struct BOUNCE
{
    // Points J = pt[0] and K = pt[2] are opposing vertexes of the
    // parallelogram, as are P = pt[1] and Q = pt[3]. A parallelogram
    // is uniquely defined by three vertexes (P, K, Q); the fourth
    // vertex (J) is determined by symmetry. Note: vpt[0] is unused.
    POINT pt[4];   // vertexes (in order) J, P, K, Q
    POINT vpt[4];  // velocities of points -, P, K, Q
    int frameWidth;  // width of rectangular drawing region
    int frameHeight;  // height of rectangular drawing region

    BOUNCE(int width, int height)
    {
        POINT vxy[] = { { 0, 0 }, { 5, 3 }, { 9, 7 }, { 6, 2 } };

        for (int i = 0; i < 4; i++)
            vpt[i] = vxy[i];

        pt[0].x = 10;
        pt[0].y = 10;
        pt[1].x = width - 10;
        pt[1].y = 10;
        pt[2].x = width - 10;
        pt[2].y = height - 10;
        pt[3].x = 10;
        pt[3].y = height - 10;
        frameWidth = width;
        frameHeight = height;
    }

    ~BOUNCE()
    {
    }

    // Copies the x-y coordinates for the four vertexes of the current
    // parallelogram to output array xy. Then moves the vertexes to
    // their updated positions in preparation for the next Update call.
    void Update(POINT xy[])
    {
        int i, diff;

        for (i = 0; i < 4; i++)
        {
            xy[i] = pt[i];
        }

        // Update P, K, and Q. Note that vpt[0] is never used.
        for (i = 1; i < 4; i++)  
        {
            pt[i].x += vpt[i].x;
            pt[i].y += vpt[i].y;
        }

        // Make sure that the x and y offsets between K and the
        // two vertexes P and Q are even so that the integer
        // coordinates of the conjugate diameters will exactly
        // divide the sides of the parallelogram.
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
        // the rectangular drawing window.
        for (i = 1; i < 4; i++)  
        {
            if (pt[i].x < 0 || frameWidth <= pt[i].x)
            {
                vpt[i].x = -vpt[i].x;
                if (pt[i].x < 0)
                {
                    pt[i].x = -pt[i].x;
                }
                else
                {
                    pt[i].x = 2*frameWidth - pt[i].x - 1;
                }                        
            }
            if (pt[i].y < 0 || frameHeight <= pt[i].y)
            {
                vpt[i].y = -vpt[i].y;
                if (pt[i].y < 0)
                {
                    pt[i].y = -pt[i].y;
                }
                else
                {
                    pt[i].y = 2*frameHeight - pt[i].y - 1;
                }                        
            }
        }

        // To update J, reflect K through the center of the line
        // connecting P and Q. If the resulting J is out of bounds,
        // bounce it off the side of the bounding box, then update
        // K by reflecting J through the center of line PQ.
        pt[0].x = pt[1].x + pt[3].x - pt[2].x;
        if (pt[0].x < 0 || frameWidth <= pt[0].x)
        {
            vpt[2].x = -vpt[2].x;
            
            if (pt[0].x < 0)
            {
                pt[0].x = -pt[0].x;
            }
            else
            {
                pt[0].x = 2*frameWidth - pt[0].x - 1;
            }
            pt[2].x = pt[1].x + pt[3].x - pt[0].x;
        }
        pt[0].y = pt[1].y + pt[3].y - pt[2].y;
        if (pt[0].y < 0 || frameHeight <= pt[0].y)
        {
            vpt[2].y = -vpt[2].y;
            
            if (pt[0].y < 0)
            {
                pt[0].y = -pt[0].y;
            }
            else
            {
                pt[0].y = 2*frameHeight - pt[0].y - 1;
            }
            pt[2].y = pt[1].y + pt[3].y - pt[0].y;
        }
    }
};

const COLORREF DARKGREEN = RGB(  0,  96,   0);
const COLORREF BLUE      = RGB(  0,   0, 255);
const COLORREF DARKBLUE  = RGB(  0,   0, 175);
const COLORREF WHITE     = RGB(255, 255, 255);
const COLORREF DARKGRAY  = RGB(128, 128, 128);

// Global handle and color values used by SetPixel function
HDC g_hdc = 0;
COLORREF g_color = 0;

// Function used by Line and Conic functions to set the pixel at
// drawing coordinates (x,y) to the currently selected color
//
void DrawPixel(int x, int y)
{
    SetPixel(g_hdc, x, y, g_color);  
}

// Barycentric coordinates
struct BARY
{
    float u;
    float v;
    float w;
};

// Converts x-y coordinate pair (xin,yin) to barycentric coordinates
// (u,v,w) given the three vertexes -- (xu,yu), (xv,yv), and (xw,yw)
// -- of the reference triangle.
void xyToBary(float xin, float yin, BARY *uvwout, 
              float xu, float yu, float xv, float yv, float xw, float yw)
{
    BARY tmp;
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
void baryToXy(POINT *xyOut, const BARY *uvwIn, POINT xy[])
{
    xyOut->x = uvwIn->u*xy[0].x + uvwIn->v*xy[1].x + uvwIn->w*xy[2].x;
    xyOut->y = uvwIn->u*xy[0].y + uvwIn->v*xy[1].y + uvwIn->w*xy[2].y;
}

const float PI = 3.14159265358979323846;

// Contains conic spline control points for a "splat" glyph (an
// asterisk). The control points are stored as barycentric
// coordinates relative to three triangle vertexes. This triangle
// is half of the square that contains the splat figure. When the
// square is squashed and stretched by being affine-transformed
// into a parallelogram, the splat is affine-transformed as well.
struct SPLAT
{
    BARY uvwSplat[49];  // spline control points for "splat" figure
    
    // Initializes conic spline control points, and stores these
    // points as barycentric coordinates defined relative to triangle
    // vertexes u = (-1,1), v = (1,1), w = (1,-1). 
    SPLAT()
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

    ~SPLAT()
    {
    }

    // Update drawing of "splat" glyph on screen based on current
    // position of parallelogram vertexes
    void UpdateSplat(HDC hdc, BOUNCE *bounce)
    {
        HPEN hPen;
        HBRUSH hBrush;
        RECT rect;
        POINT xy[49], xyPgram[5];
        int i;

        // Get vertex coordinates for current parallelogram
        bounce->Update(xyPgram);
    
        // Draw four sides of parallelogram
        hPen = CreatePen(PS_SOLID, 1, DARKGREEN);
        SelectObject(hdc, hPen);
        xyPgram[4] = xyPgram[0];  // close polyline
        Polyline(hdc, xyPgram, 5);
        DeleteObject(hPen);

        // Highlight spline knots and control points for splat glyph
        hBrush = CreateSolidBrush(BLUE);
        SelectObject(hdc, hBrush);
        for (i = 0; i < 49; i++)
        {
            baryToXy(&xy[i], &uvwSplat[i], xyPgram);
            SetRect(&rect, xy[i].x-2, xy[i].y-2, xy[i].x+3, xy[i].y+3);
            FillRect(hdc, &rect, hBrush);
        }
        DeleteObject(hBrush);

        // Draw spline skeleton for splat glyph
        hPen = CreatePen(PS_SOLID, 1, DARKBLUE);
        SelectObject(hdc, hPen);
        Polyline(hdc, xy, 49);
        DeleteObject(hPen);

        // Draw conic splines consisting of PI/2-radian elliptical arcs
        g_hdc = hdc;  // set global handle used by DrawPixel function
        g_color = WHITE;
        for (int i = 2; i < 49; i += 2)
        {
            DrawParabolicArc(xy[i-2].x, xy[i-2].y, xy[i-1].x, xy[i-1].y, xy[i].x, xy[i].y); 
        }
        g_hdc = 0;
    }
};

// Offset of bounding box from top-left corner of window
const int FRAME_OFFSET = 10;

// Process the next message for this window
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC hdcMem;
    HPEN hPen;
    RECT rect;
    static HBITMAP hbmp = 0;
    static int keypress = -1;
    static RECT region;
    static POINT frame[5];
    static BOUNCE *bounce = 0;
    static SPLAT splat;

    switch (message)
    {
    case WM_CREATE:
        // Initialize parallelogram animation and define polyline
        // frame around rectangular drawing region
        GetClientRect(hwnd, &region);
        region.right -= 2*FRAME_OFFSET;
        region.bottom -= 2*FRAME_OFFSET;
        assert(region.right > 100 && region.bottom > 100);
        bounce = new BOUNCE(region.right, region.bottom);
        frame[0].x = FRAME_OFFSET - 1;
        frame[0].y = FRAME_OFFSET - 1;
        frame[1].x = FRAME_OFFSET + region.right + 1;
        frame[1].y = FRAME_OFFSET - 1;
        frame[2].x = FRAME_OFFSET + region.right + 1;
        frame[2].y = FRAME_OFFSET + region.bottom + 1;
        frame[3].x = FRAME_OFFSET - 1;
        frame[3].y = FRAME_OFFSET + region.bottom + 1;
        frame[4] = frame[0];  // close polyline

        // Create offscreen bitmap for drawing conic splines
        hdc = GetDC(hwnd);
        hbmp = CreateCompatibleBitmap(hdc, region.right, region.bottom);
    	hdcMem = CreateCompatibleDC(hdc);
    	ReleaseDC(hwnd, hdc);
        SelectObject(hdcMem, hbmp);
    	FillRect(hdcMem, &region, (HBRUSH)(GetStockObject(BLACK_BRUSH)));
        DeleteDC(hdcMem);
        return 0;

    case WM_KEYDOWN:
        keypress = 1;
        InvalidateRect(hwnd, NULL, true);
        return 0;

    case WM_PAINT: 
        hdc = BeginPaint(hwnd, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        hPen = CreatePen(PS_SOLID, 1, DARKGRAY);
        SelectObject(hdc, hPen);
        Polyline(hdc, frame, 5);
        DeleteObject(hPen);
        SelectObject(hdcMem, hbmp);
        if (keypress)
        {
            --keypress;
            FillRect(hdcMem, &region, (HBRUSH)(GetStockObject(BLACK_BRUSH)));
            splat.UpdateSplat(hdcMem, bounce);
        }
        BitBlt(hdc, FRAME_OFFSET, FRAME_OFFSET, region.right, region.bottom, 
               hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        if (keypress)
        {
            Sleep(20);
            InvalidateRect(hwnd, NULL, true);
        }
        return 0; 

    case WM_DESTROY:
        delete bounce; 
        PostQuitMessage(0); 
        return 0; 
    } 
    return DefWindowProc(hwnd, message, wParam, lParam); 
}


