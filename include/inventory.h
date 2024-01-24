#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

#include "assets.h"
#include "bitmap.h"

#define INVENTORY_WIDTH     340
#define INVENTORY_HEIGHT    703
#define INVENTORY_SLOT_WIDTH    ICON_WIDTH
#define INVENTORY_SLOT_HEIGHT   ICON_WIDTH

#define SUBINVENTORY_COUNT      8
#define SUBINVENTORY_ROW_COUNT  4
#define SUBINVENTORY_COL_COUNT  6


typedef struct {
    int inventory;
    int x;
    int y;
    item_t *content;
    btm_t *bitmap;
} slot_t;

/**
 * @brief Locate the inventory in the window. If the inventory is closed, open
 * it to locate it.
 *
 * @param window Game window handler.
 * @return An the inventory on screen asset if located successfully, NULL otherwise.
 *
 * Note: This function is rather slow, use it as little as possible.
 */
osasset_t *LocateInventory(HWND window);

/**
 * @brief Switch selected inventory. Inventory are numbered from left to right,
 * top to bottom.
 *
 * @param window Game window handler.
 * @param inventory Sub-inventory number to switch to.
 * @return 0 on success, 1 otherwise.
 *
 * Note: This function is rather slow, use SwitchInventoryF whenever possible.
 */
int SwitchInventory(HWND window, int inventory);

/**
 * @brief Same as SwitchInventory but much faster.
 *
 * @param inv On screen inventory data.
 * @param inventory Sub-inventory number to switch to.
 * @return 0 on success, 1 otherwise.
 */
int SwitchInventoryF(osasset_t *inv, int inventory);

/**
 * @brief Get the data from an inventory slot.
 *
 * @param window Game window handler.
 * @param inventory Sub-inventory number to get the slot from.
 * @param slot_x Slot X coordinate in the sub-inventory.
 * @param slot_y Slot Y coordinate in the sub-inventory.
 * @return The slot data on success, NULL otherwise.
 */
slot_t *GetInventorySlot(HWND window, int inventory, int slot_x, int slot_y);

/**
 * @brief Get all the slots from the inventory.
 *
 * @param window Game window handler.
 * @return Vector containing all the slot data on success, empty vector otherwise.
 */
std::vector<slot_t *> GetInventorySlots(HWND window, osasset_t **inv);

int SortInventory(HWND window, osasset_t **inv);
int Purchase(osasset_t *shop, int x, int y);

slot_t *SlotInit(int inventory, int x, int y);
int SlotUse(osasset_t *inv, slot_t *slot);
int SlotSell(osasset_t *inv, slot_t *slot);
void SlotDestroy(slot_t *s);