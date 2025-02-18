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

class Bil
{
public:
    Bil(int ileft, int itop, int iright, int ibottom, Bil* start = nullptr) {
        left = ileft;
        top = itop;
        right = iright;
        bottom = ibottom;
        neste = start;
    }
    ~Bil() {
        delete neste;
    }

    void EndreX(int auk) {
        left = left + auk;
        right = right + auk;
    }

    void EndreY(int auk) {
        top = top + auk;
        bottom = bottom + auk;
    }

    void tegn(HDC hdc, COLORREF farge) {
        HBRUSH midler = CreateSolidBrush(farge);
        SelectObject(hdc, midler);
        Rectangle(hdc, left, top, right, bottom);
        DeleteObject(midler);
    }

    int getleft() {
        return left;
    }
    int getright() {
        return right;
    }
    int gettop() {
        return top;
    }
    int getbottom() {
        return bottom;
    }
    Bil* getneste() {
        return neste;
    }
    void setneste(Bil* ny) {
        neste = ny;
    }

    BOOL vurderOmFornerme(int offset) {
        return (neste == nullptr) || ((neste->getleft() - offset > right) || (neste->gettop() - offset > bottom));
    }

private:
    int left;
    int top;
    int right;
    int bottom;
    Bil* neste;
};

class linearBil
{
public:
    linearBil() : forste(nullptr), siste(nullptr) {}

    ~linearBil() {
        Bil* curr = forste;
        while (curr != nullptr) {
            Bil* temp = curr;
            curr = curr->getneste();
            delete temp;
        }
    }

    Bil* getforste() {
        return forste;
    }

    void slettS() {
        if (forste == nullptr || siste == nullptr) return;

        if (forste == siste) {
            delete forste;
            forste = siste = nullptr;
            return;
        }

        Bil* curr = forste;
        while (curr->getneste() != siste) {
            curr = curr->getneste();
        }

        delete siste;
        curr->setneste(nullptr);
        siste = curr;
    }

    void tegnAlle(HDC hdc) {
        Bil* curr = forste;
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        while (curr != nullptr) {
            if (curr->getleft() >= screenWidth || curr->gettop() >= screenHeight) {
                slettS();
                curr = nullptr;
            }
            else {
                curr->tegn(hdc, RGB(255, 0, 0));
                curr = curr->getneste();
            }
        }
    }

    void flyttAlle(int retning, int colorS) {
        Bil* curr = forste;
        while (curr != nullptr) {
            if (curr->vurderOmFornerme(5) && ((colorS == 2) || ((curr->getright() < 500 || curr->getright() > 510) && (curr->getbottom() < 200 || curr->getbottom() > 210)))) {
                if (retning == 0) {
                    curr->EndreX(5);
                }
                else {
                    curr->EndreY(5);
                }
            }
            curr = curr->getneste();
        }
    }

    void leggTil(Bil* ny) {
        if (forste == nullptr) {
            forste = ny;
            siste = ny;
        }
        else if (forste == siste) {
            siste = forste;
            forste = ny;
            forste->setneste(siste);
        }
        else {
            ny->setneste(forste);
            forste = ny;
        }
    }

private:
    Bil* forste;
    Bil* siste;
};

void UpdateColors() {
    memcpy(circlecolors, states[colorState], sizeof(circlecolors));
    memcpy(circlecolors2, states[colorState2], sizeof(circlecolors2));
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

   UpdateColors();
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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

linearBil* bilerx = new linearBil();
linearBil* bilery = new linearBil();
int pw = 5;
int pn = 5;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 0, 5000, 0);
        SetTimer(hWnd, 1, 50, 0);
        SetTimer(hWnd, 2, 1000, 0);
        break;
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
            HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap); 

            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            RECT rect = { 0, 0, screenWidth, screenHeight };
            FillRect(memDC, &rect, hBrush);
            DeleteObject(hBrush);
            DeleteObject(&rect);

            HBRUSH hbb = CreateSolidBrush(RGB(0, 0, 0));
            HGDIOBJ ho = SelectObject(memDC, hbb);

            Rectangle(memDC, 500, 0, 600, MAXIMUM_ALLOWED);
            Rectangle(memDC, 0, 200, MAXIMUM_ALLOWED, 300);

            Rectangle(memDC, 440, 80, 470, 170);
            Rectangle(memDC, 380, 330, 470, 360);

            SelectObject(memDC, ho);
            DeleteObject(hbb);

            for (int i = 0; i < 3; i++) {
                HBRUSH hBrush = CreateSolidBrush(circlecolors[i]);
                int offsett = 30;
                SelectObject(memDC, hBrush);
                Ellipse(memDC, 440, 140 - (i * offsett), 470, 170 - (i * offsett));
                DeleteObject(hBrush);
            }

            for (int i = 0; i < 3; i++) {
                HBRUSH hBrush = CreateSolidBrush(circlecolors2[i]);
                int offsett = 30;
                SelectObject(memDC, hBrush);
                Ellipse(memDC, 440 - (i * offsett), 330, 470 - (i * offsett), 360);
                DeleteObject(hBrush);
            }

            bilerx->tegnAlle(memDC);
            bilery->tegnAlle(memDC);

            BitBlt(hdc, 0, 0, screenWidth, screenHeight, memDC, 0, 0, SRCCOPY);
            
            SelectObject(memDC, oldBitmap);
            DeleteDC(memDC);
            DeleteObject(memBitmap);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_UP && pn != 100 ){
            pn = pn + 10;
        }
        if (wParam == VK_DOWN && pn != 0) {
            pn = pn - 10;
        }
        if (wParam == VK_LEFT && pw != 0) {
            pw = pw - 10;
        }
        if (wParam == VK_RIGHT && pw != 100) {
            pw = pw + 10;
        }
            break;
    case WM_LBUTTONDOWN:
        /*colorState = (colorState + 1) % 4;
        colorState2 = (colorState2 + 1) % 4;
        UpdateColors();*/
        
        /*bilerx->leggTil(new Bil(525, -50, 575, 0));*/

        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_TIMER:

        if (wParam == 0) {
            colorState = (colorState + 1) % 4;
            colorState2 = (colorState2 + 1) % 4;
            UpdateColors();
            InvalidateRect(hWnd, 0, FALSE);
        }
        else if (wParam == 1) {
            bilery->flyttAlle(0, colorState2);
            bilerx->flyttAlle(1, colorState);
            InvalidateRect(hWnd, 0, FALSE);
        }
        else if (wParam == 2) {
            if ((rand() % 100) < pw) {
                bilery->leggTil(new Bil(-50, 225, 0, 275));
            }

            if ((rand() % 100) < pn) {
                bilerx->leggTil(new Bil(525, -50, 575, 0));
            }
        }
        return 0;
        break;
    case WM_DESTROY:
        delete bilerx;
        delete bilery;
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
