#include <windows.h>

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow){
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = TEXT("RENDERER");
    RegisterClass(&windowClass);

    HWND hwnd = CreateWindow(
    windowClass.lpszClassName,
    TEXT("WinAPI"),
    WS_OVERLAPPEDWINDOW,
    100, 50, 1200, 800,
    nullptr, nullptr,
    hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

  MSG msg = {};
    while (msg.message != WM_QUIT){
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

#include <iostream>
void line(HDC hdc, int x0, int y0, int x1, int y1) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        SetPixel(hdc, (steep?y:x), (steep?x:y), RGB(255, 255, 255));
        error2 += derror2;

        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;
    static RECT Rect;
    static HDC hdc, hCmpDC;
    static HBITMAP hBmp;

    switch (message)
    {
        case WM_PAINT:
            GetClientRect(hWnd, &Rect);
            hdc = BeginPaint(hWnd, &ps);

            // Создание теневого контекста для двойной буферизации
            hCmpDC = CreateCompatibleDC(hdc);
            hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left,
            Rect.bottom - Rect.top);
            SelectObject(hCmpDC, hBmp);

            // Закраска фоновым цветом
            LOGBRUSH br;
            br.lbStyle = BS_SOLID;
            br.lbColor = 0x000000;
            HBRUSH brush;
            brush = CreateBrushIndirect(&br);
            FillRect(hCmpDC, &Rect, brush);
            DeleteObject(brush);

            // Здесь рисуем на контексте hCmpDC

            line(hCmpDC, 0, 0, 500, 500);

            // Копируем изображение из теневого контекста на экран
            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top,
            hCmpDC, 0, 0, SRCCOPY);

            // Удаляем ненужные системные объекты
            DeleteDC(hCmpDC);
            DeleteObject(hBmp);
            hCmpDC = NULL;
            EndPaint(hWnd, &ps);
        break;

        case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

