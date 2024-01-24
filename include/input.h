#pragma once

#define GAME_RESPONSE_TIME_MS   300
#define INPUT_RESPONSE_TIME_MS  50

void InputInit(void);

/**
 * @brief Setup a mouse input
 */
void MouseSetup(INPUT *input);

/**
 * @brief Move the mouse cursor to an absolute position
 *
 * @param input Mouse input object
 * @param x X coordinate
 * @param y Y coordiante
 */
void MouseMoveAbsolute(INPUT *input, int x, int y);

/**
 * @brief Left click with the mouse
 *
 * @param input Mouse input object
 */
void MouseClickLeft(INPUT *input);

/**
 * @brief Right click with the mouse
 *
 * @param input Mouse input object
 */
void MouseClickRight(INPUT *input);

/**
 * @brief Press a key on the keyboard
 *
 * @param keycode Virtual Key code
 *
 * Note: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
 */
void KeyboardPress(int keycode);

void KeyboardHold(int keycode);

void KeyboardRelease(int keycode);
