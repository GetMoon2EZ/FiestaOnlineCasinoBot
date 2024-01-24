#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "capture.h"
#include "bitmap.h"


capture_t *
CaptureInit(int x, int y, int width, int height)
{
    capture_t *cap;
    size_t n_alloc = 0;

    cap = (capture_t *) calloc(1, sizeof(*cap));
    if (cap == NULL) {
        printf("Memory allocation error.\n");
        return NULL;
    }

    cap->x = x;
    cap->y = y;

    cap->bitmap = BitmapInit(width, height);
    if (cap->bitmap == NULL) {
        printf("Memory allocation error.\n");
        free(cap);
        return NULL;
    }

    return cap;
}

void
CaptureDestroy(capture_t *cap)
{
    if (cap != NULL) {
        BitmapDestroy(cap->bitmap);
        free(cap);
    }
}

void
CaptureScreenshot(HWND window, capture_t *cap)
{
    RECT    r;
    HDC     hScreen;
    HDC     hDC;
    HBITMAP hBitmap;
    HGDIOBJ old_obj;
    BITMAP bmpScreen;
    BITMAPINFOHEADER bmi = {0};

    r.left = cap->x;
    r.right = cap->x + cap->bitmap->width;
    r.top = cap->y;
    r.bottom = cap->y + cap->bitmap->height;

    GetWindowRect(window, &r);
    hScreen = GetWindowDC(window);
    /* Create in memory Device Context */
    hDC = CreateCompatibleDC(hScreen);
    hBitmap = CreateCompatibleBitmap(hScreen, cap->bitmap->width, cap->bitmap->height);
    old_obj = SelectObject(hDC, hBitmap);
    /* Copy the source context to the in memory one */
    BitBlt(hDC, 0, 0, cap->bitmap->width, cap->bitmap->height, hScreen, cap->x, cap->y, SRCCOPY);
    /* Get the BITMAP from the HBITMAP. */
    GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = cap->bitmap->width;
    bmi.biHeight = -cap->bitmap->height;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;

    /* Extract pixel colors as RGB */
    GetDIBits(hDC, hBitmap, 0, cap->bitmap->height, cap->bitmap->data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    ReleaseDC(window, hScreen);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
}
