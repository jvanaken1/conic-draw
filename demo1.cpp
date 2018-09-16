//-----------------------------------------------------------
// 
// demo1.cpp -- Demonstrates use of Pitteway's algorithm for
//     drawing rotated ellipses 
//
//-----------------------------------------------------------

#include <windows.h> 
#include <strsafe.h>
#include <math.h>
#include <stdio.h> 
#include <string.h> 
#include <assert.h>
#include "conic.h"

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
                        TEXT("Conic Curve Demo 1"), 
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

const COLORREF ORANGE    = RGB(255, 192,   0);
const COLORREF GREEN     = RGB(  0, 255,   0);
const COLORREF MAGENTA   = RGB(255,   0, 255);
const COLORREF YELLOW    = RGB(255, 255,   0);
const COLORREF DARKGREEN = RGB(  0,  96,   0);
const COLORREF BLUE      = RGB(  0,   0, 255);
const COLORREF RED       = RGB(255,   0,   0);
const COLORREF WHITE     = RGB(255, 255, 255);
const COLORREF DARKGRAY  = RGB(128, 128, 128);

// Global handle and color values used by DrawPixel function
HDC g_hdc = 0;
COLORREF g_color = 0;

// Function used by Line and Conic functions to set pixel values
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
        return;  // conjugate diameters are perpendicular
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
            return;  // degenerate ellipse
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
void UpdateEllipse(HDC hdc, BOUNCE *bounce)
{
    HBRUSH hBrush;
    HPEN hPen;
    RECT rect;
    POINT xy[5];
    int x0, y0, x1, y1, x2, y2;
    COLORREF color[] = { ORANGE, GREEN, MAGENTA, YELLOW };
    
    bounce->Update(xy);  // get new parallelogram coordinates
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
    DrawEllipse(x0, y0, x1, y1, x2, y2);
    g_hdc = 0;
}

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
    static BOUNCE *bounce;

    switch (message)
    {
    case WM_CREATE:
        // Initialize parallelogram animation and frame
        // around rectangular drawing region
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

        // Create offscreen bitmap for drawing ellipses
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
            UpdateEllipse(hdcMem, bounce);
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


