#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_syswm.h"
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define WINDOW_WIDTH 150
#define WINDOW_HEIGHT 150
#define IMAGE_SCALE 1.5
#define IMAGE_MARGIN 20
#define FRAME_DELAY 100 // Delay for animation frames in milliseconds
#define FPS 30 // Desired frames per second

// Функция для скрытия иконки задачи
void HideTaskbarIcon(SDL_Window* window) {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        HWND hwnd = wmInfo.info.win.window;
        LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
        SetWindowLong(hwnd, GWL_EXSTYLE, (style | WS_EX_TOOLWINDOW) & ~WS_EX_APPWINDOW);
        ShowWindow(hwnd, SW_HIDE);
        ShowWindow(hwnd, SW_SHOW);
    } else {
        printf("SDL_GetWindowWMInfo Error: %s\n", SDL_GetError());
    }
}

void UpdateWindowStyles(HWND hwnd) {
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    style |= WS_EX_TOOLWINDOW;  // Добавляет WS_EX_TOOLWINDOW
    style &= ~WS_EX_APPWINDOW;  // Удаляет WS_EX_APPWINDOW
    SetWindowLong(hwnd, GWL_EXSTYLE, style);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateTransparentWindow(HINSTANCE hInstance, HWND& hwnd) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TransparentWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        fprintf(stderr, "RegisterClass failed: %ld\n", GetLastError());
        exit(1);
    }

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,  // Не добавляйте WS_EX_TOOLWINDOW здесь
        L"TransparentWindowClass",
        L"Transparent Window",
        WS_POPUP,
        0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        fprintf(stderr, "CreateWindowEx failed: %ld\n", GetLastError());
        exit(1);
    }

    UpdateWindowStyles(hwnd);

    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_COLORKEY | LWA_ALPHA);

    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    int windowX = desktopRect.right - WINDOW_WIDTH - IMAGE_MARGIN;
    int windowY = desktopRect.bottom - WINDOW_HEIGHT - IMAGE_MARGIN;
    SetWindowPos(hwnd, NULL, windowX, windowY, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);

    UpdateWindow(hwnd);
}

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
HWND hwnd = nullptr;

std::vector<SDL_Texture*> LoadAnimation(const std::string& path, int frameCount) {
    std::vector<SDL_Texture*> frames;
    for (int i = 1; i <= frameCount; ++i) {
        std::string filename = path + std::to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (!surface) {
            printf("IMG_Load Error for %s: %s\n", filename.c_str(), IMG_GetError());
            continue;
        }

        int newWidth = static_cast<int>(surface->w * IMAGE_SCALE);
        int newHeight = static_cast<int>(surface->h * IMAGE_SCALE);
        SDL_Surface* scaledSurface = SDL_CreateRGBSurface(0, newWidth, newHeight, surface->format->BitsPerPixel,
                                                         surface->format->Rmask, surface->format->Gmask,
                                                         surface->format->Bmask, surface->format->Amask);
        SDL_BlitScaled(surface, NULL, scaledSurface, NULL);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, scaledSurface);
        SDL_FreeSurface(surface);
        SDL_FreeSurface(scaledSurface);

        if (!texture) {
            printf("SDL_CreateTextureFromSurface Error for %s: %s\n", filename.c_str(), SDL_GetError());
        } else {
            frames.push_back(texture);
        }
    }
    return frames;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    (void)hInstance;
    HWND myConsole = GetConsoleWindow();
    ShowWindow(myConsole, 0);

    bool isDelaying = false;
    Uint32 delayStartTime = 0;
    Uint32 delayDuration = 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("GIF Viewer",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(window, &wmInfo)) {
        hwnd = wmInfo.info.win.window;
    } else {
        printf("SDL_GetWindowWMInfo Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowX = screenWidth - WINDOW_WIDTH - IMAGE_MARGIN;
    int windowY = screenHeight - WINDOW_HEIGHT - IMAGE_MARGIN;
    SetWindowPos(hwnd, HWND_TOPMOST, windowX, windowY, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_SHOWWINDOW);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Загрузка анимаций
    std::vector<std::vector<SDL_Texture*>> animations;
    animations.push_back(LoadAnimation("sprite\\cat_legs\\cat_legs", 21));
    animations.push_back(LoadAnimation("sprite\\cat_stretch\\cat_stretch", 47));
    animations.push_back(LoadAnimation("sprite\\cat_sleep\\cat_sleep", 35));

    animations.push_back(LoadAnimation("sprite\\cat_ears\\cat_ears_1\\cat_ears", 15));
    animations.push_back(LoadAnimation("sprite\\cat_ears\\cat_ears_2\\cat_ears", 15));
    animations.push_back(LoadAnimation("sprite\\cat_ears\\cat_ears_3\\cat_ears", 15));
    
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_1\\cat_eye", 11));
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_2\\cat_eye", 11));
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_3\\cat_eye", 11));
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_4\\cat_eye", 5));
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_5\\cat_eye", 5));
    animations.push_back(LoadAnimation("sprite\\cat_eye\\cat_eye_6\\cat_eye", 5));
    
    animations.push_back(LoadAnimation("sprite\\cat_tongue\\cat_tongue", 19));

    // Проверка на пустые анимации
    for (const auto& anim : animations) {
        if (anim.empty()) {
            printf("No frames loaded\n");
            IMG_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    SDL_Event e;
    bool running = true;
    size_t frameIndex = 0;
    Uint32 lastFrameTime = SDL_GetTicks();
    size_t currentAnimation = rand() % animations.size();

    SDL_Rect destRect;
    SDL_QueryTexture(animations[currentAnimation][0], NULL, NULL, &destRect.w, &destRect.h);
    destRect.x = (WINDOW_WIDTH - destRect.w) / 2;
    destRect.y = (WINDOW_HEIGHT - destRect.h) / 4;

    HideTaskbarIcon(window);

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // Обработка сообщений WinAPI
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= FRAME_DELAY) {
            lastFrameTime = currentTime;

            if (currentAnimation == 2) { // cat_sleep
                if (frameIndex == 17) { // 18-й кадр
                    if (!isDelaying) {
                        isDelaying = true;
                        delayDuration = rand() % (60 * 1000 - 1000 + 1) + 1000; // Random delay
                        delayStartTime = currentTime;
                    }

                    if (currentTime - delayStartTime >= delayDuration) {
                        frameIndex = 18; // Переход на 19-й кадр
                        isDelaying = false;
                    }
                } else {
                    frameIndex = (frameIndex + 1) % animations[currentAnimation].size();
                    if (frameIndex == 0) {
                        currentAnimation = rand() % animations.size();
                    }
                }
            } else {
                frameIndex = (frameIndex + 1) % animations[currentAnimation].size();
                if (frameIndex == 0) {
                    currentAnimation = rand() % animations.size();
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, animations[currentAnimation][frameIndex], NULL, &destRect);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (1000 / FPS > frameTime) {
            SDL_Delay(1000 / FPS - frameTime);
        }
    }

    // Очистка ресурсов
    for (auto& animation : animations) {
        for (auto& texture : animation) {
            SDL_DestroyTexture(texture);
        }
    }
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
