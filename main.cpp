#include "model.hpp"

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
Vec2i size_screen(1500, 1000);
// Model* model = new Model("obj/Man", size_screen);
// Model* model = new Model("obj/Orc", size_screen);
// Model* model = new Model("obj/Elizabeth", size_screen);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow){
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = TEXT("RENDERER");
    RegisterClass(&windowClass);

    HWND hwnd = CreateWindow(
    windowClass.lpszClassName,
    TEXT("Окно рендера модели"),
    WS_OVERLAPPEDWINDOW,
    0, 0,
    size_screen.x + 20, size_screen.y + 40,
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
            hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left, Rect.bottom - Rect.top);
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
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // model->draw(hCmpDC);
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // Копируем изображение из теневого контекста на экран
            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, hCmpDC, 0, 0, SRCCOPY);

            // Удаляем ненужные системные объекты
            DeleteDC(hCmpDC);
            DeleteObject(hBmp);
            hCmpDC = NULL;
            EndPaint(hWnd, &ps);
        break;

        case WM_DESTROY:
        // delete model;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}