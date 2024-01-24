/**
 * @file capture.h
 * @author GetMoon2EZ
 * @brief Screenshot capture API.
 * @version 0.1
 * @date 2024-01-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "bitmap.h"


typedef struct {
    int x;
    int y;
    btm_t *bitmap;
} capture_t;

/**
 * @brief Initialize a capture object.
 *
 * @param x X coordinate of the capture upper left corner
 * @param y Y coordinate of the capture upper left corner
 * @param width Width of the capture rectangle
 * @param height Height of the capture rectangle
 * @return Pointer to a capture object on success, NULL otherwise.
 *
 * Note: The returned value must be destroyed with CaptureDestroy in order to
 * free memory.
 */
capture_t* CaptureInit(int x, int y, int width, int height);

/**
 * @brief Destroy the capture object.
 *
 * @param cap Capture object to destroy.
 */
void CaptureDestroy(capture_t *cap);

/**
 * @brief Capture a screenshot
 *
 * @param window Window to capture the screenshot in.
 * @param cap Capture object which indicate where to take the screenshot. The
 * screenshot is saved as a bitmap in the capture object.
 */
void CaptureScreenshot(HWND window, capture_t *cap);
