#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

#define MATCHING_THRESHOLD  30.0F
#define BITMAP_MAGIC_NUMBER "BM"
#define BITMAP_MAGIC_NUMBER_SIZE 2

btm_t *
BitmapInit(int width, int height)
{
    btm_t *btm;

    btm = (btm_t *) malloc(sizeof(*btm));
    if (btm == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    btm->width = width;
    btm->height = height;
    btm->data = (BYTE *) calloc(4 * width * height, sizeof(*btm->data));
    if (btm->data == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        free(btm);
        return NULL;
    }

    return btm;
}

double
GetColorDistance(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba)
{
    int delta_r = ar - br;
    int delta_g = ag - bg;
    int delta_b = ab - bb;
    int min_a = aa > ba ? ba : aa;

    double x = min_a / 255.0;
    double y = x * x;
    // double y = std::exp(std::exp(1.0) * x * x) - 1.0;
    // printf("min_a = %d => x = %f => f(x) = %f\n", min_a, x, y);
    return (double) sqrt(delta_r * delta_r + delta_g * delta_g + delta_b * delta_b) * y;
}

double
BitmapCmp(btm_t *a, btm_t *b, bool print_diff)
{
    double ret = 0.0F;
    double color_diff;
    int n = 0;
    btm_t *diff;

    if (
        a == NULL || b == NULL ||
        a->width != b->width || a->height != b->height
    ) {
        return BITMAP_CMP_ERROR;
    }

    if (print_diff) {
        diff = BitmapInit(a->width, a->height);
        if (diff == NULL) {
            print_diff = false;
        }
    }

    for (int y = 0; y < a->height; y++) {
        for (int x = 0; x < a->width; x++) {
            if (print_diff) {
                BITMAP_SET_RED(diff, x, y, 0x00);
                BITMAP_SET_GREEN(diff, x, y, 0x00);
                BITMAP_SET_BLUE(diff, x, y, 0x00);
            }

            if (
                BITMAP_GET_ALPHA(a, x, y) == IGNORE_PIXEL_ALPHA ||
                BITMAP_GET_ALPHA(b, x, y) == IGNORE_PIXEL_ALPHA
            ) {
                if (print_diff) {
                    BITMAP_SET_ALPHA(diff, x, y, 0xFF);
                }
                continue;
            }

            color_diff = GetColorDistance(
                BITMAP_GET_RED(a, x, y),
                BITMAP_GET_GREEN(a, x, y),
                BITMAP_GET_BLUE(a, x, y),
                BITMAP_GET_ALPHA(a, x, y),
                BITMAP_GET_RED(b, x, y),
                BITMAP_GET_GREEN(b, x, y),
                BITMAP_GET_BLUE(b, x, y),
                BITMAP_GET_ALPHA(b, x, y)
            );

            ret = (n * ret + color_diff) / (double) (n + 1);
            n++;

            if (print_diff) {
                uint8_t a = (uint8_t) round(255 - color_diff * sqrt(255*255*3) / 255);
                BITMAP_SET_ALPHA(diff, x, y, a);
            }
        }
    }

    if (print_diff) {
        BitmapPrintWithAlpha(diff);
        BitmapDestroy(diff);
    }

    return ret;
}

btm_t *
BitmapLoad(const char *path)
{
    int ret;
    FILE *f;
    btm_t *btm;
    BITMAPFILEHEADER header;
    BITMAPV5HEADER info_header;
    BITMAPINFO info;
    size_t bytes_read;
    size_t curr;
    char err_buf[1024];

    ret = fopen_s(&f, path, "rb");
    if (ret != 0 || f == NULL) {
        strerror_s(err_buf, sizeof(err_buf), ret);
        fprintf(stderr, "Fail to open %s: %s.\n", path, err_buf);
        return NULL;
    }

    bytes_read = fread(&header, 1, sizeof(header), f);
    if (bytes_read != sizeof(header)) {
        fprintf(stderr, "Fail to read file header: %s.\n", path);
        fclose(f);
        return NULL;
    }

    if (memcmp(&header.bfType, BITMAP_MAGIC_NUMBER, BITMAP_MAGIC_NUMBER_SIZE) != 0) {
        fprintf(stderr, "File is not in bmp format: %s.\n", path);
        fclose(f);
        return NULL;
    }

    bytes_read = fread(&info_header, 1, sizeof(info_header), f);
    if (bytes_read != sizeof(info_header)) {
        fprintf(stderr, "Fail to read file info header: %s.\n", path);
        fclose(f);
        return NULL;
    }

    if (info_header.bV5Size != sizeof(info_header) || info_header.bV5BitCount != 32 || info_header.bV5Compression != BI_BITFIELDS) {
        fprintf(stderr, "File format is incorrect: %s.\n", path);
        fprintf(stderr, "\tbiSize       : %d\n", info_header.bV5Size);
        fprintf(stderr, "\tbiBitCount   : %d\n", info_header.bV5BitCount);
        fprintf(stderr, "\tbiCompression: %d\n", info_header.bV5Compression);
        fclose(f);
        return NULL;
    }

    if (info_header.bV5Compression == BI_BITFIELDS) {
        // fprintf(stderr, "Skipping byte layout: %d bytes.\n", 3 * sizeof(DWORD));
        fseek(f, 3 * sizeof(DWORD), SEEK_CUR);
    }

    if (info_header.bV5Height < 0) {
        btm = BitmapInit(info_header.bV5Width, -1 * info_header.bV5Height);
    } else {
        btm = BitmapInit(info_header.bV5Width, info_header.bV5Height);
    }
    if (btm == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        fclose(f);
        return NULL;
    }

    // /* The pixel RGB masks is here once more for some reason */
    // fseek(f, 3 * sizeof(DWORD), SEEK_CUR);

    curr = 0;
    while (!feof(f)) {
        size_t p = btm->height < 0 ? curr * 4 : btm->height * btm->width * 4 - (curr + 1 * btm->width) * 4;
        curr += fread(btm->data + p, 4, btm->width, f);
        if (curr > (btm->width * btm->height)) {
            fprintf(
                stderr, "Read %d items but size is %dx%d (%d).\n",
                curr, btm->width, btm->height,
                btm->height * btm->width
            );
            fclose(f);
            BitmapDestroy(btm);
            return NULL;
        }

        /* No padding on 32 bit per pixel images */
    }

    // fprintf(stderr, "curr: %d.\n", curr);
    fclose(f);
    return btm;

    // HDC     hScreen;
    // HDC     hDC;
    // HBITMAP hBitmap;
    // BITMAP bmpScreen;
    // BITMAPINFOHEADER bmi = {0};

    // hScreen = GetDC(NULL);
    // if (hScreen == NULL) {
    //     fprintf(stderr, "GetWindowDC returned NULL\n");
    //     return NULL;
    // }

    // /* Create in memory Device Context */
    // hDC = CreateCompatibleDC(hScreen);
    // if (hDC == NULL) {
    //     fprintf(stderr, "CreateCompatibleDC returned NULL\n");
    //     return NULL;
    // }

    // hBitmap = (HBITMAP) LoadImageA(NULL, (LPCSTR) path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    // if (hBitmap == NULL) {
    //     fprintf(stderr, "LoadImage returned NULL.\n");
    //     return NULL;
    // }

    // GetObject(hBitmap, sizeof(BITMAP), &bmpScreen);

    // bmi.biSize = sizeof(BITMAPINFOHEADER);
    // bmi.biPlanes = 1;
    // bmi.biBitCount = 32;
    // bmi.biWidth = bmpScreen.bmWidth;
    // bmi.biHeight = -bmpScreen.bmHeight;
    // bmi.biCompression = BI_RGB;
    // bmi.biSizeImage = 0;

    // btm = (btm_t *) calloc(1, sizeof(*btm));
    // if (btm == NULL) {
    //     fprintf(stderr, "Memory allocation error.\n");
    //     DeleteObject(hBitmap);
    //     return NULL;
    // }

    // btm->width = bmpScreen.bmWidth;
    // btm->height = bmpScreen.bmHeight;
    // btm->data = (BYTE *) malloc(4 * btm->width * btm->height);

    // /* Extract pixel colors as RGB */
    // GetDIBits(hDC, hBitmap, 0, bmpScreen.bmHeight, btm->data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    // DeleteObject(hBitmap);
    // return btm;
}

btm_t *
BitmapExtract(btm_t *btm, int x, int y, int width, int height)
{
    btm_t *ret;

    if (btm == NULL || btm->width < x + width || btm->height < y + height) {
        fprintf(stderr, "BitmapExtract error %d < %d or %d < %d.\n", btm->width, x + width, btm->height, y + height);
        return NULL;
    }

    ret = BitmapInit(width, height);
    if (ret == NULL) {
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            memcpy(
                &ret->data[4 * (i * width + j)],
                &btm->data[4 * ((y + i) * btm->width + (x + j))],
                4
            );
        }
    }

    return ret;
}

void
BitmapDestroy(btm_t *btm)
{
    if (btm) {
        free(btm->data);
        free(btm);
    }
}

void
BitmapPrint(btm_t *btm)
{
    for (int y = 0; y < btm->height; y++) {
        for (int x = 0; x < btm->width; x++) {
            BitmapPrintPixel(btm, x, y);
            printf(" ");
        }
        printf("\n");
    }
}

void
BitmapPrintWithAlpha(btm_t *btm)
{
    for (int y = 0; y < btm->height; y++) {
        for (int x = 0; x < btm->width; x++) {
            printf(
                "%02X%02X%02X%02X ",
                BITMAP_GET_RED(btm, x, y),
                BITMAP_GET_GREEN(btm, x, y),
                BITMAP_GET_BLUE(btm, x, y),
                BITMAP_GET_ALPHA(btm, x, y)
            );
        }
        printf("\n");
    }
}


void
BitmapPrintPixel(btm_t *btm, int x, int y)
{
    printf(
        "%02X%02X%02X",
        BITMAP_GET_RED(btm, x, y),
        BITMAP_GET_GREEN(btm, x, y),
        BITMAP_GET_BLUE(btm, x, y)
    );
}

int
BitmapLocate(btm_t *haystack, btm_t *needle, int *x, int *y)
{
    if (haystack == NULL || needle == NULL || x == NULL || y == NULL) {
        fprintf(stderr, "LocateBitmap: Invalid input.\n");
        return 1;
    }

    if (haystack->height < needle->height || haystack->width < needle->width) {
        fprintf(
            stderr, "LocateBitmap: Needle does not fit in haystack: %d < %d or %d < %d\n",
            haystack->height, needle->height, haystack->width, needle->width
        );
        return 1;
    }

    for (int hy = 0; hy < haystack->height - needle->height; hy++) {
        for (int hx = 0; hx < haystack->width - needle->width; hx++) {
            bool match = true;
            for (int ny = 0; ny < needle->height; ny++) {
                for (int nx = 0; nx < needle->width; nx++) {

                    int n_r = BITMAP_GET_RED(needle, nx, ny);
                    int n_g = BITMAP_GET_GREEN(needle, nx, ny);
                    int n_b = BITMAP_GET_BLUE(needle, nx, ny);

                    /* Ignore transparent pixels */
                    if (BITMAP_GET_ALPHA(needle, nx, ny) == IGNORE_PIXEL_ALPHA) {
                        continue;
                    }

                    double dist = GetColorDistance(
                        n_r, n_g, n_b, BITMAP_GET_ALPHA(needle, nx, ny),
                        BITMAP_GET_RED(haystack, hx + nx, hy + ny),
                        BITMAP_GET_GREEN(haystack, hx + nx, hy + ny),
                        BITMAP_GET_BLUE(haystack, hx + nx, hy + ny),
                        BITMAP_GET_ALPHA(haystack, hx + nx, hy + ny)
                    );

                    if (dist > MATCHING_THRESHOLD) {
                        match = false;
                        break;
                    }

                }

                if (!match) {
                    break;
                }
            }

            if (match) {
                *x = hx;
                *y = hy;
                return 0;
            }
        }
    }

    return 1;
}

btm_t *
BitmapDup(btm_t *btm)
{
    btm_t *ret;

    if (btm == NULL) {
        return NULL;
    }

    ret = BitmapExtract(btm, 0, 0, btm->width, btm->height);
    if (ret == NULL) {
        fprintf(stderr, "Fail to extract bitmap.\n");
        return NULL;
    }

    return ret;
}

int
BitmapSetAlpha(btm_t *btm, int x, int y, int width, int height, uint8_t alpha)
{
    if (btm == NULL) {
        return 1;
    }

    if (btm->width < x + width || btm->height < y + height) {
        fprintf(stderr, "BitmapSetAlpha error: %d < %d or %d < %d.\n", btm->width, x + width, btm->height, y + height);
        return 1;
    }

    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            BITMAP_SET_ALPHA(btm, x + dx, y + dy, alpha);
        }
    }

    return 0;
}
