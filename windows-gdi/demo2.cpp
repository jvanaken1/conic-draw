//---------------------------------------------------------------------
// 
// demo2.cpp -- Demonstrates the use of Pitteway's algorithm for
//     drawing conic splines. This version of the demo runs on the
//     Win32 API in Windows. 
//
//---------------------------------------------------------------------

#include <windows.h>
#include <math.h> 
#include <assert.h>
#include "conic.h"
#include "demo.h"

// Global handle and color values used by the DrawPixel function
HDC g_hdc = 0;
COLOR g_color = 0;

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
    wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); 
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
                        DEMO_WIDTH+33, DEMO_HEIGHT+56, 
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

// Function used by Line and Conic functions to set the pixel at
// drawing coordinates (x,y) to the currently selected color
//
void DrawPixel(int x, int y)
{
    SetPixel(g_hdc, x, y, g_color);
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
void baryToXy(POINT *xyOut, const BARYCENT *uvwIn, POINT xy[])
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
    void Update(HDC hdc, Bounce *bounce);
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
void Splat::Update(HDC hdc, Bounce *bounce)
{
    HPEN hPen;
    HBRUSH hBrush;
    RECT rect;
    POINT xy[49], xyPgram[5];
    int i;

    // Get vertex coordinates for current parallelogram
    bounce->Update((XYVAL*)xyPgram);

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
        ParabolicSpline(xy[i-2].x, xy[i-2].y, xy[i-1].x, xy[i-1].y, xy[i].x, xy[i].y); 
    }
    g_hdc = 0;
}

// Process the next message for this window
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC hdcMem;
    static HBITMAP hbmp = 0;
    static int keypress = -1;
    static RECT rect;
    static RECT frame;
    static Bounce *bounce = 0;
    static Splat splat;

    switch (message)
    {
    case WM_CREATE:
        // Define thin frame around rectangular drawing region
        // and initialize parallelogram animation
        GetClientRect(hwnd, &rect);
        assert(rect.right > 100 && rect.bottom > 100);
        frame.left = 2;
        frame.top = 2;
        frame.right = rect.right - 2;
        frame.bottom = rect.bottom - 2;
        bounce = new Bounce(rect.right, rect.bottom);

        // Create offscreen bitmap for drawing conic splines
        hdc = GetDC(hwnd);
        hbmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        hdcMem = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd, hdc);
        SelectObject(hdcMem, hbmp);
        FillRect(hdcMem, &rect, (HBRUSH)(GetStockObject(BLACK_BRUSH)));
        DeleteDC(hdcMem);
        return 0;

    case WM_KEYDOWN:
        keypress = 1;
        InvalidateRect(hwnd, NULL, true);
        return 0;

    case WM_PAINT: 
        hdc = BeginPaint(hwnd, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hbmp);
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
        if (keypress)
        {
            --keypress;
            FillRect(hdcMem, &rect, (HBRUSH)(GetStockObject(BLACK_BRUSH)));
            FrameRect(hdcMem, &frame, (HBRUSH)GetStockObject(GRAY_BRUSH));
            splat.Update(hdcMem, bounce);
        }
        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        if (keypress)
        {
            Sleep(17);
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


