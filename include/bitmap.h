#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdint.h>
#include <stdbool.h>

/*
 * This arbitrary alpha value is used to flag which pixels to ignore when doing
 * a bitmap comparison using BitmapCmp.
 */
#define IGNORE_PIXEL_ALPHA     0x00

#define BITMAP_CMP_ERROR    -1.0F

#define __BITMAP_ALPHA_OFFSET    3
#define __BITMAP_RED_OFFSET     2
#define __BITMAP_GREEN_OFFSET   1
#define __BITMAP_BLUE_OFFSET    0

/* Use these macros to get RGB values at position x, y in the capture bitmap */
#define BITMAP_GET_RED(btm, x, y) (btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_RED_OFFSET])
#define BITMAP_GET_GREEN(btm, x, y) (btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_GREEN_OFFSET])
#define BITMAP_GET_BLUE(btm, x, y) (btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_BLUE_OFFSET])
#define BITMAP_GET_ALPHA(btm, x, y) (btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_ALPHA_OFFSET])

#define BITMAP_SET_RED(btm, x, y, r) do { btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_RED_OFFSET] = (r); } while (0)
#define BITMAP_SET_GREEN(btm, x, y, g) do { btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_GREEN_OFFSET] = (g); } while (0)
#define BITMAP_SET_BLUE(btm, x, y, b) do { btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_BLUE_OFFSET] = (b); } while (0)
#define BITMAP_SET_ALPHA(btm, x, y, a) do { btm->data[4 * ((y) * btm->width + (x)) + __BITMAP_ALPHA_OFFSET] = (a); } while (0)


typedef struct {
    int width;
    int height;
    BYTE *data;
} btm_t;

btm_t *BitmapInit(int width, int height);

/**
 * @brief Get the color distance between two colors.
 *
 * @param ar First color red component.
 * @param ag First color green component.
 * @param ab First color blue component.
 * @param br Second color red component.
 * @param bg Second color green component.
 * @param bb Second color blue component.
 * @return The color distance.
 */
double GetColorDistance(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba);
// double GetColorDistance(int ar, int ag, int ab, int br, int bg, int bb);

/**
 * @brief Compare two bitmap object.
 *
 * @param a Bitmap A.
 * @param b Bitmap B.
 * @return On success, a positive floating point value is returned. It represents
 * the average of the colour distance between each pixel. The closer to 0, the
 * more the bitmaps look alike, 0 being an exact pixel to pixel match.
 * BITMAP_CMP_ERROR is returned if the two bitmaps do not share the same dimensions.
 */
double BitmapCmp(btm_t *a, btm_t *b, bool print_diff);

/**
 * @brief Load a bitmap image.
 *
 * @param path Path to the BMP image.
 * @return Pointer to the bitmap on success, NULL otherwise.
 */
btm_t *BitmapLoad(const char *path);

/**
 * @brief Extract a bitmap from a bigger one.
 *
 * @param btm Input bitmap (the bigger one).
 * @param x X coordinate of the upper left corner of the sub-image in the main image.
 * @param y Y coordinate of the upper left corner of the sub-image in the main image.
 * @param width Width of the extracted bitmap.
 * @param height Height of the extracted bitmap.
 * @return A pointer to the extracted bitmap on success, NULL otherwise.
 *
 * Note: The returned bitmap must be freed using BitmapDestroy.
 */
btm_t *BitmapExtract(btm_t *btm, int x, int y, int width, int height);

/**
 * @brief Free memory for a bitmap.
 *
 * @param btm Bitmap to free.
 *
 * Note: Do not call twice on the same bitmap, double free may occur.
 */
void BitmapDestroy(btm_t *btm);

/**
 * @brief Print the bitmap as an array of hexadecimal RGB values
 *
 * @param btm Bitmap object to print.
 */
void BitmapPrint(btm_t *btm);
void BitmapPrintWithAlpha(btm_t *btm);

/**
 * @brief Locate a bitmap within another bitmap
 *
 * @param haystack Haystack bitmap
 * @param needle Smaller bitmap we are looking for
 * @param x X coordinate of the found needle
 * @param y Y coordinate of the found needle
 * @return 0 on success, 1 otherwise. When an error occur, x and y are set to
 * undefined.
 */
int BitmapLocate(btm_t *haystack, btm_t *needle, int *x, int *y);

/**
 * @brief Duplicate a bitmap.
 *
 * @param btm Bitmap to duplicate.
 * @return A newly allocated copy of the provided bitmap on success, NULL otherwise.
 *
 * Note: The returned bitmap must be freed using BitmapDestroy.
 */
btm_t *BitmapDup(btm_t *btm);

void BitmapPrintPixel(btm_t *btm, int x, int y);

int BitmapSetAlpha(btm_t *btm, int x, int y, int width, int height, uint8_t alpha);
