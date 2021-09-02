#include "model.hpp"

LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Model* model = new Model("obj/african_head.obj");
static int width = 800;
static int height = 800;

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow){
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = TEXT("RENDERER");
    RegisterClass(&windowClass);

    HWND hwnd = CreateWindow(
    windowClass.lpszClassName,
    TEXT("WinAPI"),
    WS_OVERLAPPEDWINDOW,
    50, 50,
    width + 50, height + 50,
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

            // �������� �������� ��������� ��� ������� �����������
            hCmpDC = CreateCompatibleDC(hdc);
            hBmp = CreateCompatibleBitmap(hdc, Rect.right - Rect.left,
            Rect.bottom - Rect.top);
            SelectObject(hCmpDC, hBmp);

            // �������� ������� ������
            LOGBRUSH br;
            br.lbStyle = BS_SOLID;
            br.lbColor = 0x000000;
            HBRUSH brush;
            brush = CreateBrushIndirect(&br);
            FillRect(hCmpDC, &Rect, brush);
            DeleteObject(brush);

            // ����� ������ �� ��������� hCmpDC
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<model->nfaces(); i++) {
                std::vector<int> face = model->face(i);
                for (int j=0; j<3; j++) {
                    Vec3f v0 = model->vert(face[j]);
                    Vec3f v1 = model->vert(face[(j+1)%3]);
                    int x0 = (v0.x+1.) * width / 2.;
                    int y0 = (v0.y+1.) * height / 2.;
                    int x1 = (v1.x+1.) * width / 2.;
                    int y1 = (v1.y+1.) * height / 2.;
                    line(hCmpDC, x0, y0, x1, y1);
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // �������� ����������� �� �������� ��������� �� �����
            SetStretchBltMode(hdc, COLORONCOLOR);
            BitBlt(hdc, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top,
            hCmpDC, 0, 0, SRCCOPY);

            // ������� �������� ��������� �������
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

