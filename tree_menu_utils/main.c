#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h> // Для системного трея
#include "resource.h"

#define VERSION "1.0.1"

HINSTANCE hInst;
HWND hEdit, hStatusBar;
NOTIFYICONDATA nid;
HFONT hFont;

PROCESS_INFORMATION pi; // Для хранения информации о процессе другого исполняемого файла
HANDLE hProcess; // Для хранения дескриптора процесса другого исполняемого файла

// Функция для добавления иконки в системный трей
void AddTrayIcon(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = IDI_ICON1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
    strcpy(nid.szTip, "Chibi Cat");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Функция для удаления иконки из системного трея
void RemoveTrayIcon(HWND hwnd) {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void StartExternalProcess(const char *exePath) {
    STARTUPINFO si = { sizeof(si) };
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
        exePath,    // Путь к исполняемому файлу
        NULL,       // Командная строка
        NULL,       // Дескриптор процесса не наследуется
        NULL,       // Дескриптор потока не наследуется
        FALSE,      // Не наследовать дескрипторы
        0,          // Стандартные флаги
        NULL,       // Использовать текущую переменную среды
        NULL,       // Использовать текущий каталог
        &si,        // Указатель на информацию о запуске
        &pi         // Указатель на информацию о процессе
    )) {
        MessageBox(NULL, "Failed to start external process.", "Error", MB_ICONEXCLAMATION | MB_OK);
    } else {
        hProcess = pi.hProcess; // Сохраните дескриптор процесса
    }
}

void StopExternalProcess() {
    if (hProcess) {
        TerminateProcess(hProcess, 0); // Завершение процесса
        CloseHandle(hProcess); // Закрытие дескриптора процесса
        hProcess = NULL;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                hwnd,
                NULL,
                hInst,
                NULL
            );
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            break;

        case WM_SIZE:
            {
                RECT rc;
                GetClientRect(hwnd, &rc);
                SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom - 20, SWP_NOZORDER);
                SetWindowPos(hStatusBar, NULL, 0, rc.bottom - 20, rc.right, 20, SWP_NOZORDER);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_EXIT:
                    StopExternalProcess(); // Завершение другого процесса
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_USER + 1: // Сообщение из трея
            if (lParam == WM_RBUTTONUP) {
                HMENU hTrayMenu = CreatePopupMenu();
                AppendMenu(hTrayMenu, MF_STRING, ID_FILE_EXIT, "Exit");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hTrayMenu);
            } else if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hwnd, SW_RESTORE); // Восстановление окна
                SetForegroundWindow(hwnd); // Активировать окно
            }
            break;

        case WM_CLOSE:
            // Скрытие окна вместо его закрытия
            ShowWindow(hwnd, SW_HIDE);
            return 0;

        case WM_DESTROY:
            RemoveTrayIcon(hwnd); // Удаление иконки трея
            StopExternalProcess(); // Завершение другого процесса
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND myConsole = GetConsoleWindow(); //window handle
    ShowWindow(myConsole, 0); //handle window

    // Создание шрифта
    hFont = CreateFont(
        20,                        // Высота шрифта
        0,                         // Ширина шрифта (0 означает автоматическая ширина)
        0,                         // Угол наклона шрифта
        0,                         // Угол наклона шрифта
        FW_NORMAL,                 // Толщина шрифта
        FALSE,                     // Курсив
        FALSE,                     // Подчеркивание
        FALSE,                     // Перечеркивание
        DEFAULT_CHARSET,           // Набор символов
        OUT_DEFAULT_PRECIS,        // Точность вывода
        CLIP_DEFAULT_PRECIS,       // Точность обрезки
        DEFAULT_QUALITY,           // Качество вывода
        DEFAULT_PITCH | FF_SWISS,  // Стиль шрифта и семья шрифтов
        "Arial"                    // Название шрифта
    );

    WNDCLASSEX wcex;
    HWND hwnd;
    MSG msg;

    hInst = hInstance;

    // Регистрация класса окна
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = "ChibiCat";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Создание окна
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "ChibiCat",
        "Chibi Cat",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Запуск другого исполняемого файла
    StartExternalProcess("bin\\basic.exe");

    // Скрываем окно сразу при запуске
    ShowWindow(hwnd, SW_HIDE);

    UpdateWindow(hwnd);

    AddTrayIcon(hwnd); // Добавление иконки в трей

    // Основной цикл сообщений
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
