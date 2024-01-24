#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <string.h>
#include <iostream>

#include "capture.h"
#include "input.h"
#include "bitmap.h"
#include "inventory.h"
#include "assets.h"


#define GAME_WINDOW_NAME    "FiestaOnline"
// #define GAME_WINDOW_NAME    "Photos"

HWND window = NULL;

int
SelectWindow(const char *window_name)
{
    window = FindWindowA(NULL, window_name);

    if (window == NULL) {
        return 1;
    }

    SetForegroundWindow(window);
    SetActiveWindow(window);
    SetFocus(window);

    return 0;
}

/* Returns true if an item should be sold */
bool
ShouldSell(item_t *item)
{
    /* Keep all unknown/empty items */
    if (item->asset->id == Unknown || item->asset->id == Empty) {
        return false;
    }

    /* Keep all weapons */
    if (item->asset->flags & ITEM_IS_WEAPON) {
        return false;
    }

    /* Keep all B5 items */
    if (item->grade >= ItemGrade5) {
        return false;
    }

    if (item->asset->flags & ITEM_IS_PREMIUM) {
        return false;
    }

    return true;
}

#include <vector>
#include <map>
using namespace std;

#ifdef CB_DEBUG
int
main()
#else
int WINAPI
WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd)
#endif
{
    int ret = 1;
    int purchased;
    osasset_t *shop = NULL;
    osasset_t *inv = NULL;
    vector<slot_t *> all_slots, slots;
    slot_t *tmp;

    printf("Loading assets.\n");
    if (LoadAssets() != 0) {
        fprintf(stderr, "Fail to load assets.\n");
        goto cleanup;
    }

    InputInit();

    printf("Selecting game window.\n");
    if (SelectWindow(GAME_WINDOW_NAME) != 0) {
        std::cerr << "Fail to select game window" << std::endl;
        goto cleanup;
    }

    printf("Locating Shop.\n");
    if ((shop = LocateAsset(window, Shop)) == NULL) {
        fprintf(stderr, "Fail to locate shop.\n");
        goto cleanup;
    }

    /* Check that it is the correct shop (check all content) */
    // TODO
    // printf("Retrieving inventory slots.\n");

    // slot = GetInventorySlot(window, 0, 2, 2);
    // BitmapPrintWithAlpha(slot->bitmap);
    // slot->content = IdentifySlotContent(slot->bitmap);
    // fprintf(stderr, "Slot content: %s.\n", GetAsset(slot->content)->name);
    while (true) {
        if (SortInventory(window, &inv) != 0) {
            fprintf(stderr, "Fail to sort inventory.\n");
            goto cleanup;
        }

        /* Wait for the sorting to finish */
        Sleep(5000);

        all_slots = GetInventorySlots(window, &inv);
        if (all_slots.size() == 0) {
            fprintf(stderr, "Fail to get inventory slots.\n");
            goto cleanup;
        }

        printf("Retrieving empty slots.\n");
        for (slot_t *it: all_slots) {
            if (it->content->asset->id == Empty || it->content->asset->id == LuckyCapsuleRedGold) {
                slots.push_back(it);
            } else {
                SlotDestroy(it);
            }
        }

        if (slots.size() == 0) {
            fprintf(stderr, "Inventory is full.\n");
            goto cleanup;
        }

        printf("Purchasing lucky %" PRIu64 " capsules.\n", slots.size());
        purchased = 0;
        for (size_t i = 0; i < slots.size() - 1; i++) {
            if (slots[i]->content->asset->id != Empty) {
                continue;
            }

            if (Purchase(shop, LUCKY_CAPSULE_RED_GOLD_X, LUCKY_CAPSULE_RED_GOLD_Y) != 0) {
                fprintf(stderr, "Fail to purchase item from shop.\n");
                goto cleanup;
            }

            Sleep(GAME_RESPONSE_TIME_MS);

            tmp = slots[i];
            slots[i] = GetInventorySlot(window, tmp->inventory, tmp->x, tmp->y);
            SlotDestroy(tmp);
            if (slots[i] == NULL) {
                fprintf(stderr, "Fail to get inventory slot.\n");
                goto cleanup;
            }

            slots[i]->content = IdentifySlotContent(slots[i]->bitmap);
            if (slots[i]->content->asset->id == LuckyCapsuleRedGold) {
                purchased++;
            // } else {
            //     BitmapPrint(slots[i]->bitmap);
            //     fprintf(
            //         stderr, "Fail to purchase lucky capsule: slot %d, %d, %d content is %s.\n",
            //         slots[i]->inventory, slots[i]->x, slots[i]->y,
            //         slots[i]->content->asset->name
            //     );
            //     goto cleanup;
            }
        }

        if (purchased == 0) {
            break;
        }

        for (size_t i = 0; i < slots.size() - 1; i++) {
            if (slots[i]->content->asset->id != LuckyCapsuleRedGold) {
                continue;
            }

            /* Cannot use SlotUse because the shop window prevents us from opening this way */
            KeyboardPress(0x31);
            Sleep(GAME_RESPONSE_TIME_MS);

            tmp = slots[i];
            slots[i] = GetInventorySlot(window, tmp->inventory, tmp->x, tmp->y);
            SlotDestroy(tmp);
            if (slots[i] == NULL) {
                fprintf(stderr, "Fail to get inventory slot.\n");
                goto cleanup;
            }

            slots[i]->content = IdentifySlotContent(slots[i]->bitmap);
            if (slots[i]->content == NULL || slots[i]->content->asset->id != Empty) {
                BitmapPrint(slots[i]->bitmap);
                fprintf(
                    stderr, "Fail to use lucky capsule: slot %d, %d, %d content is %s.\n",
                    slots[i]->inventory, slots[i]->x, slots[i]->y,
                    slots[i]->content == NULL ? "ERROR" : slots[i]->content->asset->name
                );
                goto cleanup;
            }
        }

        for (size_t i = 0; i < slots.size(); i++) {
            tmp = slots[i];
            slots[i] = GetInventorySlot(window, tmp->inventory, tmp->x, tmp->y);
            SlotDestroy(tmp);
            if (slots[i] == NULL) {
                fprintf(stderr, "Fail to get inventory slot.\n");
                goto cleanup;
            }

            slots[i]->content = IdentifySlotContent(slots[i]->bitmap);
            // if (slots[i]->content->asset->id == Empty || slots[i]->content->asset->id == Unknown) {
            //     continue;
            // }

            if (ShouldSell(slots[i]->content)) {
                fprintf(
                    stdout,
                    "#%d%d%d Should sell: %s Tier %d.\n",
                    slots[i]->inventory, slots[i]->x, slots[i]->y,
                    slots[i]->content->asset->name, slots[i]->content->grade + 1
                );
                BitmapPrint(slots[i]->bitmap);
                printf("\n\n");
                SlotSell(inv, slots[i]);
            } else {
                fprintf(
                    stdout,
                    "#%d%d%d Should not sell: %s Tier %d.\n",
                    slots[i]->inventory, slots[i]->x, slots[i]->y,
                    slots[i]->content->asset->name, slots[i]->content->grade + 1
                );
                BitmapPrint(slots[i]->bitmap);
                printf("\n\n");
            }
        }

        for (slot_t *it: slots) {
            SlotDestroy(it);
        }
        slots.clear();
    }

    // TODO: Key hold (CTRL)
    // TODO: Check the asset matching function with scrolls
    // TODO: Add other items from the loot boxes
    // TODO: Add inventory management logic

    fprintf(stderr, "Done!\n");
    ret = 0;

cleanup:
    OSAssetDestroy(shop);
    for (slot_t *slot: slots) {
        SlotDestroy(slot);
    }
    UnloadAssets();
    return ret;
}
