// TraficLights.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TraficLights.h"
#include <vector>
#include <ctime>

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
double pw = 0.5;
double pn = 0.5;

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

void MoveCars(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    const int minDistance = 50;
    const int WINDOW_WIDTH = rect.right;
    const int WINDOW_HEIGHT = rect.bottom;

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

                if (canMove && (car.x < 470 || car.x > 500 || (colorState2 == 2 && car.x < WINDOW_WIDTH))) {
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

                if (canMove && (car.y < 140 || car.y > 170 || (colorState == 2 && car.y < WINDOW_HEIGHT))) {
                    car.y += 5;

                }
            }
            cars.erase(std::remove_if(cars.begin(), cars.end(), [&](Car& car) {
                return (car.x >= WINDOW_WIDTH || car.y >= WINDOW_HEIGHT);
                }),cars.end());
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
   MoveCars(hWnd);

   SetTimer(hWnd, 0, 5000, NULL);
   SetTimer(hWnd, 1, 50, NULL);
   SetTimer(hWnd, 2, 1000, NULL);
   

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

        RECT rect;
        GetClientRect(hWnd, &rect);

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        HBRUSH hbb = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdcMem, &rect, hbb);
        DeleteObject(hbb);

        HBRUSH hRoadBrush = CreateSolidBrush(RGB(50, 50, 50));
        SelectObject(hdcMem, hRoadBrush);
        Rectangle(hdcMem, 500, 0, 600, rect.bottom);
        Rectangle(hdcMem, 0, 200, rect.right, 300);
        Rectangle(hdcMem, 440, 80, 470, 170);
        Rectangle(hdcMem, 380, 330, 470, 360);
        DeleteObject(hRoadBrush);

        for (int i = 0; i < 3; i++) {
            HBRUSH hBrush = CreateSolidBrush(circlecolors[i]);
            SelectObject(hdcMem, hBrush);
            Ellipse(hdcMem, 440, 140 - (i * 30), 470, 170 - (i * 30));
            DeleteObject(hBrush);
        }

        for (int i = 0; i < 3; i++) {
            HBRUSH hBrush = CreateSolidBrush(circlecolors2[i]);
            SelectObject(hdcMem, hBrush);
            Ellipse(hdcMem, 440 - (i * 30), 330, 470 - (i * 30), 360);
            DeleteObject(hBrush);
        }

        HDC hdcCar = CreateCompatibleDC(hdcMem);
        HBITMAP hbmOldCar = (HBITMAP)SelectObject(hdcCar, hCarBitmap);

        for (const auto& car : cars) {
            BitBlt(hdcMem, car.x, car.y, 30, 30, hdcCar, 0, 0, SRCCOPY);
        }

        SelectObject(hdcCar, hbmOldCar);
        DeleteDC(hdcCar);

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RIGHT:
            pw = min(1.0, pw * 1.1);
            break;
        case VK_LEFT:
            pw = min(0.1, pw * 0.9);
            break;
        case VK_UP:
            pn = min(1.0, pn * 1.1);
            break;
        case VK_DOWN:
            pn = min(0.1, pn * 0.9);
            break;
        }
        break;
    case WM_TIMER:
        if (wParam == 0) {
            colorState = (colorState + 1) % 4;
            colorState2 = (colorState2 + 1) % 4;
            UpdateColors();
        }
        else if (wParam == 1) {
            MoveCars(hWnd);
        }
        else if (wParam == 2) {
            if ((double)rand() / RAND_MAX < pw) {
                cars.push_back({ -30, 230, true, true });
            }
            if ((double)rand() / RAND_MAX < pn) {
                cars.push_back({ 530, -30, true, false });
            }
        }
        InvalidateRect(hWnd, NULL, FALSE);
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
