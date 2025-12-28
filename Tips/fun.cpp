#include <windows.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>  // Include this header for std::max and std::min

const std::string HEADING = "Maaregi muje?";
const std::string QUESTION = "HIHIHIHI";
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 30;
HFONT hFont, hHeadingFont;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

HWND hwndButtonYes, hwndButtonNo, hwndHeading;
bool canMoveButton = true;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char className[] = "MovingNoButtonWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(className, QUESTION.c_str(), WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, 800, 400, NULL, NULL, hInstance, NULL);

    hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));
    hHeadingFont = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

    hwndHeading = CreateWindow("STATIC", HEADING.c_str(), WS_VISIBLE | WS_CHILD | SS_CENTER,
                               150, 50, 500, 50, hwnd, NULL, hInstance, NULL);
    SendMessage(hwndHeading, WM_SETFONT, (WPARAM)hHeadingFont, TRUE);

    hwndButtonYes = CreateWindow("BUTTON", "Nahi honey, ni maarungi", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                 250, 250, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL, hInstance, NULL);
    SendMessage(hwndButtonYes, WM_SETFONT, (WPARAM)hFont, TRUE);

    hwndButtonNo = CreateWindow("BUTTON", "Bohot pitega mujhse", WS_VISIBLE | WS_CHILD,
                                450, 250, BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, NULL, hInstance, NULL);
    SendMessage(hwndButtonNo, WM_SETFONT, (WPARAM)hFont, TRUE);

    srand(static_cast<unsigned int>(time(0)));

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void MoveButton(HWND hwnd, int cursorX, int cursorY) {
    if (!canMoveButton) return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    int newX = rand() % (rect.right - BUTTON_WIDTH);
    int newY = rand() % (rect.bottom - BUTTON_HEIGHT);

    int proximity = abs(newX - cursorX) + abs(newY - cursorY);
    int speedFactor = std::max(10, 200 - proximity / 10);

    if (proximity < 200) {
        newX = (newX + (rand() % speedFactor) - speedFactor / 2) % (rect.right - BUTTON_WIDTH);
        newY = (newY + (rand() % speedFactor) - speedFactor / 2) % (rect.bottom - BUTTON_HEIGHT);
    }

    newX = std::max(0, std::min(newX, static_cast<int>(rect.right) - BUTTON_WIDTH));
    newY = std::max(0, std::min(newY, static_cast<int>(rect.bottom) - BUTTON_HEIGHT));

    SetWindowPos(hwndButtonNo, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if ((HWND)lParam == hwndButtonYes) {
                MessageBox(hwnd, "HAHAH, ", "Awlele", MB_OK | MB_ICONINFORMATION);
            } else if ((HWND)lParam == hwndButtonNo) {
                canMoveButton = false;
                MessageBox(hwnd, "Itni mehnat kari hai toh you'll get a coffee.", "Pancho", MB_OK | MB_ICONEXCLAMATION);
                canMoveButton = true;
            }
            break;

        case WM_MOUSEMOVE: {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(hwnd, &cursorPos);

            RECT rect;
            GetWindowRect(hwndButtonNo, &rect);
            POINT buttonCenter = { (rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2 };
            ScreenToClient(hwnd, &buttonCenter);

            int distance = (cursorPos.x - buttonCenter.x) * (cursorPos.x - buttonCenter.x) +
                           (cursorPos.y - buttonCenter.y) * (cursorPos.y - buttonCenter.y);

            if (distance < 10000 && canMoveButton) {
                MoveButton(hwnd, cursorPos.x, cursorPos.y);
            }
        } break;

        case WM_DESTROY:
            DeleteObject(hFont);
            DeleteObject(hHeadingFont);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

