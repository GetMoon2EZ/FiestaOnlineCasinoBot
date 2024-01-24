#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "input.h"

#include <stdio.h>

void
KeyboardPress(int keycode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;
    input.ki.wScan = MapVirtualKeyA(keycode, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.time = 0;
    input.ki.dwExtraInfo = NULL;

    SendInput(1, &input, sizeof(INPUT));

    Sleep(INPUT_RESPONSE_TIME_MS);

    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void
KeyboardHold(int keycode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;
    input.ki.wScan = MapVirtualKeyA(keycode, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.time = 0;
    input.ki.dwExtraInfo = NULL;

    SendInput(1, &input, sizeof(INPUT));
    Sleep(INPUT_RESPONSE_TIME_MS);
}

void
KeyboardRelease(int keycode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;
    input.ki.wScan = MapVirtualKeyA(keycode, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = NULL;

    SendInput(1, &input, sizeof(INPUT));
    Sleep(INPUT_RESPONSE_TIME_MS);
}