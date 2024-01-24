#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <ctgmath>

#include "input.h"

static int screenWidth = -1;
static int screenHeight = -1;

void
InputInit(void)
{
    HDC hdc = GetDC(NULL);
    screenWidth = GetDeviceCaps(hdc, HORZRES);
    screenHeight = GetDeviceCaps(hdc, VERTRES);

    fprintf(stderr, "Resolution: %dx%d\n", screenWidth, screenHeight);
}

void
MouseSetup(INPUT *input)
{
    input->type = INPUT_MOUSE;
    input->mi.dx = 0;
    input->mi.dy = 0;
    input->mi.mouseData = 0;
    input->mi.dwFlags = 0;
    input->mi.time = 0;
    input->mi.dwExtraInfo = 0;
}

void
MouseMoveAbsolute(INPUT *input, int x, int y)
{
    input->mi.dx = (LONG) round(x * (0xFFFF / (double) screenWidth));
    input->mi.dy = (LONG) round(y * (0xFFFF / (double) screenHeight));
    input->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

    SendInput(1, input, sizeof(INPUT));
}

void
MouseClickLeft(INPUT *input)
{
    input->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, input, sizeof(INPUT));

    Sleep(INPUT_RESPONSE_TIME_MS);

    input->mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, input, sizeof(INPUT));
}

void
MouseClickRight(INPUT *input)
{
    input->mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, input, sizeof(INPUT));

    Sleep(INPUT_RESPONSE_TIME_MS);

    input->mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, input, sizeof(INPUT));
}
