#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>

#include "bitmap.h"

#define ICON_WIDTH      32
#define ICON_HEIGHT     32

#define GRADE_ICON_WIDTH    13
#define GRADE_ICON_HEIGHT   13
#define GRADE_ICON_X_OFFSET 0
#define GRADE_ICON_Y_OFFSET 0

#define STACK_COUNT_WIDTH    23
#define STACK_COUNT_HEIGHT   9
#define STACK_COUNT_X_OFFSET 8
#define STACK_COUNT_Y_OFFSET 23

#define LUCKY_CAPSULE_RED_GOLD_X    1
#define LUCKY_CAPSULE_RED_GOLD_Y    0

#define ITEM_GRADED             (0x1 << 0)
#define ITEM_STACKABLE          (0x1 << 1)
#define ITEM_IS_SCROLL          (0x1 << 2)
#define ITEM_IS_UPGRADE_ITEM    (0x1 << 3)
#define ITEM_IS_WEAPON          (0x1 << 4)
#define ITEM_IS_PREMIUM         (0x1 << 5)
#define ITEM_2                  (0x1 << 6)
#define ITEM_3                  (0x1 << 7)

typedef enum {
    ItemGrade1 = 0,
    ItemGrade2,
    ItemGrade3,
    ItemGrade4,
    ItemGrade5,
    ItemGrade6,
    ItemGrade7,
    GRADE_ID_MAX,
    LuckyCapsuleRedGold = GRADE_ID_MAX,
    AimScrollT1,
    AgilityScrollT1,
    DefenseScrollT1,
    ConcentrationScrollT1,
    VitalityScrollT1,
    MentalityScrollT1,
    SpeedScrollT1,
    MDefenseScrollT1,
    SPPotionT1,
    HPPotionT1,
    SPPotionT2,
    HPPotionT2,
    SPPotionT3,
    HPPotionT3,
    Elrue,
    Lix,
    Xir,
    ElrueL,
    LixL,
    XirL,
    ElrueB,
    LixB,
    XirB,
    TearOfLegel,
    DropBoost,
    EXPBoost,
    CasinoBlade,
    CasinoDualSwords,
    CasinoClaws,
    CasinoWand,
    CasinoStaff,
    CasinoXBow,
    CasinoBow,
    CasinoHammer,
    CasinoMace,
    CasinoShield,
    CasinoAxe,
    CasinoTSword,
    CasinoSword,
    Empty,
    Unknown,
    ICON_ID_MAX,
    Inventory = ICON_ID_MAX,
    Shop,
    ASSET_ID_MAX,
} ASSET_ID;

typedef struct {
    ASSET_ID id;
    char *name;
    char *bmp_path;
    int icon_x;
    int icon_y;
    int flags;
    btm_t *bitmap;
} asset_t;

/* On Screen Asset */
typedef struct {
    ASSET_ID id;
    int x;
    int y;
    btm_t *bitmap;
} osasset_t;

typedef struct {
    const asset_t *asset;
    int grade;
    int cnt;
} item_t;

inline ASSET_ID& operator++(ASSET_ID & id)
{
    id = static_cast<ASSET_ID>(static_cast<int>(id) + 1);
    return id;
}

int LoadAssets(void);
int UnloadAssets(void);
const asset_t *GetAsset(ASSET_ID id);
const asset_t *FindMatchingIcon(btm_t *btm);
const asset_t * FindMatchingGrade(btm_t *btm);
item_t *IdentifySlotContent(btm_t *slot);
osasset_t *LocateAsset(HWND window, ASSET_ID id);
void OSAssetDestroy(osasset_t *p);

item_t *ItemInit(void);
void ItemDestroy(item_t *i);
