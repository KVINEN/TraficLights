// TraficLights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TraficLights.h"
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

const COLORREF RED = RGB(255, 0, 0);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF GRAY = RGB(69, 69, 69);

const COLORREF states[4][3] = {
    {RED, GRAY, GRAY},
    {RED, YELLOW, GRAY},
    {GRAY, GRAY, GREEN},
    {GRAY, YELLOW, GRAY}
};
int colorState = 0;
int colorState2 = 2;
COLORREF circlecolors[3];
COLORREF circlecolors2[3];

HBITMAP hCarBitmap = NULL;

struct Car {
    int x, y;
    bool isMoving;
    bool isHorizontal;
};

std::vector<Car> cars;

void UpdateColors() {
    memcpy(circlecolors, states[colorState], sizeof(circlecolors));
    memcpy(circlecolors2, states[colorState2], sizeof(circlecolors2));
}

void MoveCars() {
    const int minDistance = 50;

    for (size_t i = 0; i < cars.size(); i++) {
        Car& car = cars[i];

        if (car.isHorizontal) {
            bool canMove = true;

            for (size_t j = 0; j < cars.size(); j++) {
                if (j != i && cars[j].isHorizontal) {
                    if (cars[j].x > car.x && (cars[j].x - car.x) < minDistance) {
                        canMove = false;
                        break;
                    }
                }
            }

            if (canMove && (car.x < 470 || car.x > 500 || (colorState2 == 2 && car.x < MAXIMUM_ALLOWED))) {
                car.x += 5;
            }
        }
        
        else {
            bool canMove = true;

            for (size_t j = 0; j < cars.size(); j++) {
                if (j != i && !cars[j].isHorizontal) {
                    if (cars[j].y > car.y && (cars[j].y - car.y) < minDistance) {
                        canMove = false;
                        break;
                    }
                }
            }

            if (canMove && (car.y < 140 || car.y > 170 || (colorState == 2 && car.y < MAXIMUM_ALLOWED))) {
                car.y += 5;
            }
        }
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRAFICLIGHTS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRAFICLIGHTS));
    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAFICLIGHTS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRAFICLIGHTS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hCarBitmap = (HBITMAP)LoadImage(NULL, L"car.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
   if (!hCarBitmap) {
       MessageBox(hWnd, L"Failed to load car.bmp", L"Error", MB_OK);
   }

   UpdateColors();
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   SetTimer(hWnd, 0, 5000, 0);
   SetTimer(hWnd, 1, 50, 0);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            HBRUSH hbb = CreateSolidBrush(RGB(0, 0, 0));
            HGDIOBJ ho = SelectObject(hdc, hbb);

            Rectangle(hdc, 500, 0, 600, MAXIMUM_ALLOWED);
            Rectangle(hdc, 0, 200, MAXIMUM_ALLOWED, 300);
            Rectangle(hdc, 440, 80, 470, 170);
            Rectangle(hdc, 640, 140, 730, 170);
            DeleteObject(hbb);

            for (int i = 0; i < 3; i++) {
                HBRUSH hBrush = CreateSolidBrush(circlecolors[i]);
                INT offsett = 30;
                SelectObject(hdc, hBrush);
                Ellipse(hdc, 440, 80 + (i * offsett), 470, 110 + (i * offsett));
                DeleteObject(hBrush);
            }

            for (int i = 0; i < 3; i++) {
                HBRUSH hBrush = CreateSolidBrush(circlecolors2[i]);
                INT offsett = 30;
                SelectObject(hdc, hBrush);
                Ellipse(hdc, 640 + (i * offsett), 140, 670 + (i * offsett), 170);
                DeleteObject(hBrush);
            }

            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hCarBitmap);

            for (const auto& car : cars) {
                BitBlt(hdc, car.x, car.y, 30, 30, hdcMem, 0, 0, SRCCOPY);
            }

            SelectObject(hdcMem, hbmOld);
            DeleteDC(hdcMem);
            SelectObject(hdc, ho);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONDOWN:
        cars.push_back({ 0, 230, true, true });
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_RBUTTONDOWN:
        cars.push_back({ 530,0, true,false });
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_TIMER:
        if (wParam == 0) {
            colorState = (colorState + 1) % 4;
            colorState2 = (colorState2 + 1) % 4;
            UpdateColors();
        }
        else if (wParam == 1) {
            MoveCars();
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
