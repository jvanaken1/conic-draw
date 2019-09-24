//---------------------------------------------------------------------
// 
// demo1.cpp -- Demonstrates the use of Pitteway's algorithm for
//     drawing rotated ellipses. This version of the demo runs on the
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
    static TCHAR szAppName[] = TEXT("CONIC CURVE DEMO 1"); 
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
                        TEXT("Conic Curve Demo 1"), 
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

// Draw the minimum bounding box for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse.
void BoundingBox(HDC hdc, int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    int xbox, ybox;

    // Translate center of ellipse to origin
    xp = x1 - x0;
    yp = y1 - y0;
    xq = x2 - x0;
    yq = y2 - y0;
    xbox = sqrt(xp*xp + xq*xq) + 0.5;
    ybox = sqrt(yp*yp + yq*yq) + 0.5;
    Rectangle(hdc, x0-xbox-1, y0-ybox-1, x0+xbox+2, y0+ybox+2);
}

// Draw the enclosing polygon for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse. The polygon
// has 8 sides: two opposing sides are vertical, two are hori-
// zontal, two are +45-degree diagonals, and two are -45-degree
// diagonals. Each side of the polygon touches the ellipse at
// exactly one point, at which the side is tangent to the ellipse.
void BoundingPgon(HDC hdc, int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float tmp1, tmp2;
    float X, Y, Z, W, U, V;
    POINT xy[9];

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
    Polyline(hdc, xy, 9);
}

// Draw an 8-sided polygon inscribed in an ellipse specified
// by its center point (x0,y0) and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse. Each
// vertex of the polygon is a point at which the drawing
// octant changes in the ellipse-drawing algorithm. This is
// the same point at which one vertical, horizontal, or diagonal
// side of the bounding polygon (drawn by the BoundingPolygon
// function) touches (and is tangent to) the ellipse.
void InscribedPgon(HDC hdc, int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float tmp1, tmp2;
    float X, Y, Z, W, U, V;
    POINT xy[9];

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
    Polyline(hdc, xy, 9);
}

// Draw the major and minor axes for an ellipse given the center
// point (x0,y0) of the ellipse and the end points (x1,y1) and 
// (x2,y2) of two conjugate diameters of the ellipse.
void DrawAxes(HDC hdc, int x0, int y0, int x1, int y1, int x2, int y2)
{
    float xp, yp, xq, yq;
    float A, B, C, F;
    float xprod, beta, root, slope, denom, x, y;
    POINT xy[2];

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
        Polyline(hdc, xy, 2);
        slope = beta - root;
    } 
}

// Draw updated ellipse in window. Also draw the parallelogram in
// which the ellipse is incribed, the bounding box for the ellipse,
// the bounding polygon for the ellipse, the polygon inscribed in
// the ellipse, and the major and minor axes of the ellipse.
void UpdateEllipse(HDC hdc, Bounce *bounce)
{
    HBRUSH hBrush;
    HPEN hPen;
    RECT rect;
    POINT xy[5];
    int x0, y0, x1, y1, x2, y2;
    COLORREF color[] = { ORANGE, GREEN, MAGENTA, YELLOW};

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
    hPen = CreatePen(PS_NULL, 1, 0);
    hBrush = CreateSolidBrush(DARKGREEN);
    SelectObject(hdc, hBrush);
    BoundingBox(hdc, x0, y0, x1, y1, x2, y2);
    DeleteObject(hBrush);

    // Highlight vertices of parallelogram
    for (int i = 0; i < 4; i++)
    {
        SetRect(&rect, xy[i].x-2, xy[i].y-2, xy[i].x+3, xy[i].y+3);
        hBrush = CreateSolidBrush(color[i]);
        SelectObject(hdc, hBrush);
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
    }

    // Draw four sides of parallelogram
    hPen = CreatePen(PS_SOLID, 1, BLUE);
    SelectObject(hdc, hPen);
    Polyline(hdc, xy, 5);
    DeleteObject(hPen);

    // Draw 8-sided bounding polygon around ellipse
    hPen = CreatePen(PS_SOLID, 1, RED);
    SelectObject(hdc, hPen);
    BoundingPgon(hdc, x0, y0, x1, y1, x2, y2);
    DeleteObject(hPen);

    // Draw 8-sided polygon inscribed in ellipse
    hPen = CreatePen(PS_SOLID, 1, MAGENTA);
    SelectObject(hdc, hPen);
    InscribedPgon(hdc, x0, y0, x1, y1, x2, y2);
    DeleteObject(hPen);

    // Draw major and minor axes of ellipse
    hPen = CreatePen(PS_SOLID, 1, YELLOW);
    SelectObject(hdc, hPen);
    DrawAxes(hdc, x0, y0, x1, y1, x2, y2);
    DeleteObject(hPen);

    // Draw ellipse inscribed in parallelogram
    g_hdc = hdc;  // set global handle used by DrawPixel function
    g_color = WHITE;
    Ellipse(x0, y0, x1, y1, x2, y2);
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

        // Create offscreen bitmap for drawing ellipses
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
            UpdateEllipse(hdcMem, bounce);
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


