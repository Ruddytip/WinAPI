#include "model.hpp"
#include <ctime>

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Model* model = new Model("obj/Heads/african_head.obj");
//Model* model = new Model("obj/Heads/Elizabeth.obj");
//Model* model = new Model("obj/Elizabeth/source/Elizabeth.obj");
static int size = 800;

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
    50, 50,
    size + 40, size + 40,
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
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            //model->drawMesh(hCmpDC, size, RGB(255, 255, 255));
            model->drawMeshTriangle(hCmpDC, size, RGB(rand()%255, 255, 255));
            //triangle(hCmpDC, Vec2i(rand()%800, rand()%800), Vec2i(rand()%800, rand()%800), Vec2i(rand()%800, rand()%800), RGB(255, 255, 255));
            ////////////////////////////////////////////////////////////////////////////////////////////////////
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

