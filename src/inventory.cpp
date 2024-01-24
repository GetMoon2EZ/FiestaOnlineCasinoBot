#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <vector>

#include "inventory.h"
#include "capture.h"
#include "assets.h"
#include "input.h"

/* Sub-inventories */
#define SUBINVENTORY_OFFSET_X   35
#define SUBINVENTORY_OFFSET_Y   430
#define SUBINVENTORY_H_GAP      47
#define SUBINVENTORY_V_GAP      42

/* Slot positions */
#define INVENTORY_SLOT_OFFSET_X     32
#define INVENTORY_SLOT_OFFSET_Y     512
#define SHOP_SLOT_OFFSET_X          18
#define SHOP_SLOT_OFFSET_Y          102

#define SLOT_H_GAP      37
#define SLOT_V_GAP      37

/* Inventory sort buttons */
#define INVENTORY_SORT_OFFSET_X     300
#define INVENTORY_SORT_OFFSET_Y     530

/* Void Inventory */
#define VOID_INVENTORY_OFFSET_X     250
#define VOID_INVENTORY_OFFSET_Y     475

using namespace std::chrono;

// TODO: Cache the last position and try to locate it from here
typedef struct {
    int x;
    int y;
    time_point<system_clock> last_update;
} inventory_t;

static int SlotCoordinateToPositionX(osasset_t *win, int x);
static int SlotCoordinateToPositionY(osasset_t *win, int y);

/* Keep track of the current inventory */
static int c_inv = -1;

osasset_t *
LocateInventory(HWND window)
{
    osasset_t *os_inv;

    if ((os_inv = LocateAsset(window, Inventory)) == NULL) {
        /* Inventory not located, try to open it */
        KeyboardPress(0x49);
        if ((os_inv = LocateAsset(window, Inventory)) == NULL) {
            return NULL;
        }
    }

    return os_inv;
}

int
SwitchInventory(HWND window, int inventory)
{
    INPUT input;
    osasset_t *inv;

    if (c_inv == inventory) {
        /* Nothing to do */
        return 0;
    }

    if (inventory >= SUBINVENTORY_COUNT) {
        fprintf(stderr, "Cannot switch to inventory %d (max: %d inventories).\n", inventory, SUBINVENTORY_COUNT);
        return 1;
    }

    if ((inv = LocateInventory(window)) == NULL) {
        fprintf(stderr, "Fail to locate inventory.\n");
        return 1;
    }

    inv->x += SUBINVENTORY_OFFSET_X;
    inv->x += ((inventory % 4) * SUBINVENTORY_H_GAP);

    inv->y += SUBINVENTORY_OFFSET_Y;
    if (inventory >= 4) {
        inv->y += SUBINVENTORY_V_GAP;
    }

    MouseSetup(&input);
    MouseMoveAbsolute(&input, inv->x, inv->y);
    Sleep(INPUT_RESPONSE_TIME_MS);
    MouseClickLeft(&input);

    c_inv = inventory;

    OSAssetDestroy(inv);
    return 0;
}

int
SwitchInventoryF(osasset_t *inv, int inventory)
{
    INPUT input;
    int sub_x, sub_y;

    if (c_inv == inventory) {
        /* Nothing to do */
        return 0;
    }

    if (inv == NULL || inventory >= SUBINVENTORY_COUNT) {
        return 1;
    }

    sub_x = inv->x + SUBINVENTORY_OFFSET_X + ((inventory % 4) * SUBINVENTORY_H_GAP);

    sub_y = inv->y + SUBINVENTORY_OFFSET_Y;
    if (inventory >= 4) {
        sub_y += SUBINVENTORY_V_GAP;
    }

    MouseSetup(&input);
    MouseMoveAbsolute(&input, sub_x, sub_y);
    Sleep(INPUT_RESPONSE_TIME_MS);
    MouseClickLeft(&input);

    c_inv = inventory;

    Sleep(GAME_RESPONSE_TIME_MS);
    return 0;
}


slot_t *
GetInventorySlot(HWND window, int inventory, int slot_x, int slot_y)
{
    osasset_t *inv = NULL;
    slot_t *slot = NULL;
    capture_t *cap = NULL;

    if ((inv = LocateInventory(window)) == NULL) {
        fprintf(stderr, "Fail to locate inventory.\n");
        return NULL;
    }

    slot = SlotInit(inventory, slot_x, slot_y);
    if (slot == NULL) {
        fprintf(stderr, "Fail to initialize slot.\n");
        goto err;
    }

    if (SwitchInventoryF(inv, slot->inventory) != 0) {
        fprintf(stderr, "Fail to switch inventory.\n");
        goto err;
    }

    cap = CaptureInit(
        inv->x + SlotCoordinateToPositionX(inv, slot->x),
        inv->y + SlotCoordinateToPositionY(inv, slot->y),
        ICON_WIDTH,
        ICON_HEIGHT
    );
    // Sleep(1000);
    CaptureScreenshot(window, cap);
    slot->bitmap = BitmapDup(cap->bitmap);
    CaptureDestroy(cap);

    return slot;
err:
    OSAssetDestroy(inv);
    SlotDestroy(slot);
    return NULL;
}

std::vector<slot_t *>
GetInventorySlots(HWND window, osasset_t **inv)
{
    std::vector<slot_t *> ret;
    slot_t *slot = NULL;
    capture_t *cap = NULL;

    if (inv == NULL) {
        return { };
    }

    if (*inv == NULL) {
        if ((*inv = LocateInventory(window)) == NULL) {
            fprintf(stderr, "Fail to locate inventory.\n");
            return { };
        }
    }

    cap = CaptureInit((*inv)->x, (*inv)->y, INVENTORY_WIDTH, INVENTORY_HEIGHT);
    if (cap == NULL) {
        fprintf(stderr, "Fail to initialize capture.\n");
        goto err;
    }

    for (int i = 0; i < SUBINVENTORY_COUNT; i++) {
        if (SwitchInventoryF(*inv, i) != 0) {
            fprintf(stderr, "Fail to switch to inventory: %d.\n", i);
            goto err;
        }

        CaptureScreenshot(window, cap);

        for (int y = 0; y < SUBINVENTORY_ROW_COUNT; y++) {
            for (int x = 0; x < SUBINVENTORY_COL_COUNT; x++) {
                slot = SlotInit(i, x, y);
                if (slot == NULL) {
                    fprintf(stderr, "Fail to initialize slot.\n");
                    goto err;
                }

                slot->bitmap = BitmapExtract(
                    cap->bitmap,
                    SlotCoordinateToPositionX(*inv, x),
                    SlotCoordinateToPositionY(*inv, y),
                    INVENTORY_SLOT_WIDTH,
                    INVENTORY_SLOT_HEIGHT
                );

                if (slot->bitmap == NULL) {
                    fprintf(stderr, "Fail to extract slot bitmap.\n");
                    SlotDestroy(slot);
                    goto err;
                }

                slot->content = IdentifySlotContent(slot->bitmap);
                if (slot->content == NULL) {
                    fprintf(stderr, "Fail to identify slot content in inventory %d, row %d, col %d.\n", i, x, y);
                    SlotDestroy(slot);
                    goto err;
                }

                printf("Slot: %d, %d, %d: %s.\n", slot->inventory, slot->x, slot->y, slot->content->asset->name);
                ret.push_back(slot);
            }
        }
    }

    return ret;
err:
    CaptureDestroy(cap);
    for (slot_t *slot: ret) {
        SlotDestroy(slot);
    }
    ret.clear();
    return ret;
}

int
Purchase(osasset_t *shop, int item_x, int item_y)
{
    INPUT input;

    /* Click in the middle of the slot */
    item_x = shop->x + SlotCoordinateToPositionX(shop, item_x) + ICON_WIDTH / 2;
    item_y = shop->y + SlotCoordinateToPositionY(shop, item_y) + ICON_HEIGHT / 2;

    MouseSetup(&input);
    MouseMoveAbsolute(&input, item_x, item_y);
    MouseClickRight(&input);

    return 0;
}

int
SortInventory(HWND window, osasset_t **inv)
{
    INPUT input;

    if (inv == NULL) {
        return 1;
    }

    if (*inv == NULL) {
        if ((*inv = LocateInventory(window)) == NULL) {
            fprintf(stderr, "Fail to locate inventory.\n");
            return 1;
        }
    }

    MouseSetup(&input);
    MouseMoveAbsolute(&input, (*inv)->x + INVENTORY_SORT_OFFSET_X, (*inv)->y + INVENTORY_SORT_OFFSET_Y);
    MouseClickLeft(&input);

    return 0;
}

slot_t *
SlotInit(int inventory, int x, int y)
{
    slot_t *slot;

    if (
        inventory >= SUBINVENTORY_COUNT ||
        x >= SUBINVENTORY_COL_COUNT ||
        y >= SUBINVENTORY_ROW_COUNT
    ) {
        fprintf(stderr, "Cannot initialize slot for sub-inventory %d, row %d, col %d.\n", inventory, x, y);
        return NULL;
    }

    slot = (slot_t *) malloc(sizeof(*slot));
    if (slot == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    slot->inventory = inventory;
    slot->x = x;
    slot->y = y;
    slot->bitmap = NULL;

    return slot;
}

int
SlotUse(osasset_t *inv, slot_t *slot)
{
    INPUT input;

    if (inv == NULL || slot == NULL) {
        return 1;
    }

    if (SwitchInventoryF(inv, slot->inventory) != 0) {
        fprintf(stderr, "Fail to switch to inventory %d.\n", slot->inventory);
        return 1;
    }

    /* Right click in the middle of the slot */
    MouseSetup(&input);
    MouseMoveAbsolute(
        &input,
        inv->x + SlotCoordinateToPositionX(inv, slot->x) + ICON_WIDTH / 2,
        inv->y + SlotCoordinateToPositionY(inv, slot->y) + ICON_HEIGHT / 2
    );
    MouseClickRight(&input);
    Sleep(GAME_RESPONSE_TIME_MS);

    return 0;
}

int
SlotSell(osasset_t *inv, slot_t *slot)
{
    INPUT input;

    if (inv == NULL || slot == NULL) {
        return 1;
    }

    if (SwitchInventoryF(inv, slot->inventory) != 0) {
        fprintf(stderr, "Fail to switch to inventory %d.\n", slot->inventory);
        return 1;
    }

    /* CTRL + Right click in the middle of the slot */
    KeyboardHold(VK_CONTROL);
    MouseSetup(&input);
    MouseMoveAbsolute(
        &input,
        inv->x + SlotCoordinateToPositionX(inv, slot->x) + ICON_WIDTH / 2,
        inv->y + SlotCoordinateToPositionY(inv, slot->y) + ICON_HEIGHT / 2
    );
    MouseClickRight(&input);
    Sleep(GAME_RESPONSE_TIME_MS);
    KeyboardRelease(VK_CONTROL);

    return 0;
}

void
SlotDestroy(slot_t *s)
{
    if (s != NULL) {
        BitmapDestroy(s->bitmap);
        ItemDestroy(s->content);
        free(s);
    }
}

static int
SlotCoordinateToPositionX(osasset_t *win, int x)
{
    int ret = 0;

    switch (win->id) {
        case Shop:
            ret += SHOP_SLOT_OFFSET_X;
            break;
        case Inventory:
            ret += INVENTORY_SLOT_OFFSET_X;
            break;
    }

    return ret + (x * SLOT_H_GAP);
}

static int
SlotCoordinateToPositionY(osasset_t *win, int y)
{
    int ret = 0;

    switch (win->id) {
        case Shop:
            ret += SHOP_SLOT_OFFSET_Y;
            break;
        case Inventory:
            ret += INVENTORY_SLOT_OFFSET_Y;
            break;
    }

    return ret + (y * SLOT_V_GAP);
}
