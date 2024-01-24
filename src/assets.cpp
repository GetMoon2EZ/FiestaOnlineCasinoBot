#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <map>

#include "assets.h"
#include "bitmap.h"
#include "capture.h"
#include "input.h"

#define ASSET_MATCHING_THRESHOLD    11.0F
#define GRADE_MATCHING_THRESHOLD    10.0F

/*
 * All this is originally in FiestaOnline/resmenu/Icon.
 * Conversion to 8-bit color bmp is done with paint.net.
 */
#define ICON_PRODUCTION_0_PATH  "./img/Prdct000.bmp"
#define ICON_PRODUCTION_1_PATH  "./img/Prdct001.bmp"
#define ICON_PRODUCTION_2_PATH  "./img/RotGCrafting000.bmp"
#define ICON_ABILITY_0_PATH     "./img/Ability000.bmp"
#define ICON_ABILITY_1_PATH     "./img/ability001.bmp"
#define ICON_PET_1_PATH         "./img/pet001.bmp"
#define ICON_CAPSULE_PATH       "./img/VarItem001.bmp"
#define ICON_ITEM_GRADE_0_PATH  "./img/ItemGrade.bmp"
#define ICON_ITEM_GRADE_1_PATH  "./img/RotGItemGrade000.bmp"
#define ICON_EMPTY_SLOT_PATH    "./img/EmptySlotT.bmp"
#define ICON_EQUIPMENT_16_PATH  "./img/ItemEqu016.bmp"

#define ASSET_INVENTORY_PATH    "./img/Inventory.bmp"
#define ASSET_SHOP_PATH         "./img/CasinoShop.bmp"

#define ICON_STACK_COUNT_HEIGHT 9

asset_t grades[] = {
    { ItemGrade1,       "Item Grade 1",             ICON_ITEM_GRADE_0_PATH, 3, 0, 0, NULL },
    { ItemGrade2,       "Item Grade 2",             ICON_ITEM_GRADE_0_PATH, 0, 1, 0, NULL },
    { ItemGrade3,       "Item Grade 3",             ICON_ITEM_GRADE_0_PATH, 1, 1, 0, NULL },
    { ItemGrade4,       "Item Grade 4",             ICON_ITEM_GRADE_0_PATH, 2, 1, 0, NULL },
    { ItemGrade5,       "Item Grade 5",             ICON_ITEM_GRADE_0_PATH, 3, 1, 0, NULL },
    { ItemGrade6,       "Item Grade 6",             ICON_ITEM_GRADE_0_PATH, 3, 2, 0, NULL },
    { ItemGrade7,       "Item Grade 7",             ICON_ITEM_GRADE_1_PATH, 0, 0, 0, NULL },
    { GRADE_ID_MAX, NULL, NULL, 0, NULL }
};

asset_t icons[] = {
    { LuckyCapsuleRedGold,  "Lucky Capsule Red Gold",   ICON_CAPSULE_PATH,      3, 2, 0, NULL },
    { AimScrollT1,          "Aim Scroll",               ICON_PRODUCTION_0_PATH, 4, 1, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { AgilityScrollT1,      "Agility Scroll",           ICON_PRODUCTION_0_PATH, 5, 1, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { DefenseScrollT1,      "Defense Scroll",           ICON_PRODUCTION_0_PATH, 6, 1, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { ConcentrationScrollT1,"Concentration Scroll",     ICON_PRODUCTION_0_PATH, 7, 1, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { VitalityScrollT1,     "Vitality Scroll",          ICON_PRODUCTION_0_PATH, 0, 2, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { MentalityScrollT1,    "Mentality Scroll",         ICON_PRODUCTION_0_PATH, 1, 2, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { SpeedScrollT1,        "Speed Scroll",             ICON_PRODUCTION_1_PATH, 6, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { MDefenseScrollT1,     "Magical Defense Scroll",   ICON_PRODUCTION_1_PATH, 5, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { SPPotionT1,           "SP Potion",                ICON_PRODUCTION_0_PATH, 3, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { HPPotionT1,           "HP Potion",                ICON_PRODUCTION_0_PATH, 0, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { SPPotionT2,           "SP Potion",                ICON_PRODUCTION_0_PATH, 4, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { HPPotionT2,           "HP Potion",                ICON_PRODUCTION_0_PATH, 1, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { SPPotionT3,           "SP Potion",                ICON_PRODUCTION_0_PATH, 5, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { HPPotionT3,           "HP Potion",                ICON_PRODUCTION_0_PATH, 2, 0, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { Elrue,                "Elrue",                    ICON_PRODUCTION_0_PATH, 6, 2, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { Lix,                  "Lix",                      ICON_PRODUCTION_0_PATH, 1, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { Xir,                  "Xir",                      ICON_PRODUCTION_0_PATH, 4, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { ElrueL,               "Lucky Elrue",              ICON_PRODUCTION_0_PATH, 0, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { LixL,                 "Lucky Lix",                ICON_PRODUCTION_0_PATH, 3, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { XirL,                 "Lucky Xir",                ICON_PRODUCTION_0_PATH, 6, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { ElrueB,               "Blessed Elrue",            ICON_PRODUCTION_0_PATH, 7, 2, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { LixB,                 "Blessed Lix",              ICON_PRODUCTION_0_PATH, 2, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { XirB,                 "Blessed Xir",              ICON_PRODUCTION_0_PATH, 5, 3, ITEM_GRADED | ITEM_STACKABLE, NULL },
    { TearOfLegel,          "Tear Of Legel",            ICON_ABILITY_1_PATH,    3, 5, ITEM_STACKABLE | ITEM_IS_PREMIUM, NULL },
    { DropBoost,            "Drop Boost",               ICON_ABILITY_1_PATH,    7, 1, ITEM_STACKABLE | ITEM_IS_PREMIUM, NULL },
    { EXPBoost,             "EXP Boost",                ICON_ABILITY_0_PATH,    0, 5, ITEM_STACKABLE | ITEM_IS_PREMIUM, NULL },
    { CasinoSword,          "Nightmare Queen's Sword",  ICON_EQUIPMENT_16_PATH, 5, 4, ITEM_IS_WEAPON, NULL },
    { CasinoTSword,         "Nightmare Queen's T Sword",ICON_EQUIPMENT_16_PATH, 6, 4, ITEM_IS_WEAPON, NULL },
    { CasinoAxe,            "Nightmare Queen's Axe",    ICON_EQUIPMENT_16_PATH, 7, 4, ITEM_IS_WEAPON, NULL },
    { CasinoShield,         "Nightmare Queen's Shield", ICON_EQUIPMENT_16_PATH, 0, 5, ITEM_IS_WEAPON, NULL },
    { CasinoMace,           "Nightmare Queen's Mace",   ICON_EQUIPMENT_16_PATH, 1, 5, ITEM_IS_WEAPON, NULL },
    { CasinoHammer,         "Nightmare Queen's Hammer", ICON_EQUIPMENT_16_PATH, 2, 5, ITEM_IS_WEAPON, NULL },
    { CasinoBow,            "Nightmare Queen's Bow",    ICON_EQUIPMENT_16_PATH, 3, 5, ITEM_IS_WEAPON, NULL },
    { CasinoXBow,           "Nightmare Queen's Crossbow", ICON_EQUIPMENT_16_PATH, 4, 5, ITEM_IS_WEAPON, NULL },
    { CasinoStaff,          "Nightmare Queen's Staff",  ICON_EQUIPMENT_16_PATH, 5, 5, ITEM_IS_WEAPON, NULL },
    { CasinoWand,           "Nightmare Queen's Wand",   ICON_EQUIPMENT_16_PATH, 6, 5, ITEM_IS_WEAPON, NULL },
    { CasinoClaws,          "Nightmare Queen's Claws",  ICON_EQUIPMENT_16_PATH, 7, 5, ITEM_IS_WEAPON, NULL },
    { CasinoDualSwords,     "Nightmare Queen's Dual Swords", ICON_EQUIPMENT_16_PATH, 0, 6, ITEM_IS_WEAPON, NULL },
    { CasinoBlade,          "Nightmare Queen's Blade",  ICON_EQUIPMENT_16_PATH, 1, 6, ITEM_IS_WEAPON, NULL },
    { Empty,                "Empty",                    ICON_EMPTY_SLOT_PATH,   0, 0, 0,  NULL },
    { Unknown,              "Unknown",                  NULL, 0, NULL },
    { ICON_ID_MAX, NULL, NULL, 0, NULL },
};

asset_t special_assets[] = {
    { Inventory,    "Inventory",  ASSET_INVENTORY_PATH,   0, 0, 0, NULL },
    { Shop,         "Shop",       ASSET_SHOP_PATH,        0, 0, 0, NULL },
    { ASSET_ID_MAX, NULL, NULL, 0, NULL }
};

struct cmp_str
{
    bool operator()(char const *a, char const *b) const
    {
        return strcmp(a, b) < 0;
    }
};

static std::map<char *, btm_t *, cmp_str> loaded;

static int LoadIcons(void);
static int LoadSpecialAssets(void);

int
LoadAssets(void)
{
    if (LoadIcons() != 0) {
        fprintf(stderr, "Fail to load icons.\n");
        return 1;
    }

    if (LoadSpecialAssets() != 0) {
        fprintf(stderr, "Fail to load special assets.\n");
        return 1;
    }

    return 0;
}

int
UnloadAssets(void)
{
    for (asset_t *a = grades; a->name != NULL; a++) {
        BitmapDestroy(a->bitmap);
    }

    for (asset_t *a = icons; a->name != NULL; a++) {
        BitmapDestroy(a->bitmap);
    }

    for (std::pair<char *, btm_t*> it: loaded) {
        BitmapDestroy(it.second);
    }

    loaded.clear();
    return 0;
}


const asset_t *
GetAsset(ASSET_ID id)
{
    if (id >= ASSET_ID_MAX) {
        return NULL;
    }

    if (id < GRADE_ID_MAX) {
        return &grades[id];
    }

    if (id < ICON_ID_MAX) {
        return &icons[id - GRADE_ID_MAX];
    }

    return &special_assets[id - ICON_ID_MAX];
}

const asset_t *
FindMatchingIcon(btm_t *btm)
{
    asset_t *best = NULL;
    double best_score = DBL_MAX;
    double score;

    for (asset_t *a = icons; a->name != NULL; a++) {
        if (a->bmp_path == NULL) {
            continue;
        }

        score = BitmapCmp(btm, a->bitmap, false);
        if (score == BITMAP_CMP_ERROR) {
            fprintf(stderr, "Error comparing %s to provided bitmap.\n", a->name);
            continue;
        }

        if (score < best_score) {
            best_score = score;
            best = a;
        }
    }

    return best;
}

const asset_t *
FindMatchingGrade(btm_t *btm)
{
    btm_t *grade_btm, *current;
    asset_t* best = NULL;
    double best_score = DBL_MAX;

    if (btm->width < GRADE_ICON_WIDTH || btm->height < GRADE_ICON_HEIGHT) {
        return NULL;
    }

    grade_btm = BitmapExtract(btm, 0, 0, GRADE_ICON_WIDTH, GRADE_ICON_HEIGHT);
    if (grade_btm == NULL) {
        fprintf(stderr, "Fail to extract grade icon.\n");
        return NULL;
    }

    for (asset_t * icon = grades; icon->name != NULL; icon++) {
        if (icon->bmp_path == NULL) {
            continue;
        }

        current = BitmapExtract(icon->bitmap, 0, 0, GRADE_ICON_WIDTH, GRADE_ICON_HEIGHT);
        if (current == NULL) {
            fprintf(stderr, "Fail to extract grade icon asset.\n");
            continue;
        }


        double score = BitmapCmp(grade_btm, current, false);
        BitmapDestroy(current);
        if (score == BITMAP_CMP_ERROR) {
            fprintf(stderr, "Error comparing %s to provided bitmap.\n", icon->name);
            continue;
        }

        if (score < best_score) {
            best_score = score;
            best = icon;
        }
    }

    BitmapDestroy(grade_btm);
    return best;
}

static int
LoadIcons(void)
{
    asset_t *categories[] = { grades, icons };

    for (int i = 0; i < 2; i++) {
        for (asset_t *a = categories[i]; a->name != NULL; a++) {
            printf("Loading icon: %s.\n", a->name);
            if (a->bmp_path == NULL) {
                continue;
            }

            auto bitmap = loaded.find(a->bmp_path);
            if (bitmap == loaded.end()) {
                loaded[a->bmp_path] = BitmapLoad(a->bmp_path);
                if (loaded[a->bmp_path] == NULL) {
                    fprintf(stderr, "Fail to load bitmap: %s\n", a->bmp_path);
                    return 1;
                }
            }

            a->bitmap = BitmapExtract(
                loaded[a->bmp_path],
                a->icon_x * ICON_WIDTH,
                a->icon_y * ICON_HEIGHT,
                ICON_WIDTH,
                ICON_HEIGHT
            );

            if (a->bitmap == NULL) {
                fprintf(stderr, "Fail to extract bitmap for asset %s\n", a->name);
                return 1;
            }

            if (a->flags & ITEM_GRADED) {
                if (
                    BitmapSetAlpha(
                        a->bitmap, GRADE_ICON_X_OFFSET, GRADE_ICON_Y_OFFSET,
                        GRADE_ICON_WIDTH, GRADE_ICON_HEIGHT, IGNORE_PIXEL_ALPHA
                    ) != 0
                ) {
                    fprintf(stderr, "Fail to set bitmap alpha.\n");
                    return 1;
                }
            }

            if (a->flags & ITEM_STACKABLE) {
                if (
                    BitmapSetAlpha(
                        a->bitmap, STACK_COUNT_X_OFFSET, STACK_COUNT_Y_OFFSET,
                        STACK_COUNT_WIDTH, STACK_COUNT_HEIGHT, IGNORE_PIXEL_ALPHA
                    ) != 0
                ) {
                    fprintf(stderr, "Fail to set bitmap alpha.\n");
                    return 1;
                }
            }
        }
    }

    return 0;
}

static int
LoadSpecialAssets(void)
{
    for (asset_t *a = special_assets; a->name != NULL; a++) {
        printf("Loading special asset: %s.\n", a->name);
        if (a->bmp_path == NULL) {
            continue;
        }

        auto bitmap = loaded.find(a->bmp_path);
        if (bitmap == loaded.end()) {
            loaded[a->bmp_path] = BitmapLoad(a->bmp_path);
            if (loaded[a->bmp_path] == NULL) {
                fprintf(stderr, "Fail to load bitmap: %s", a->bmp_path);
                return 1;
            }
        }

        a->bitmap = loaded[a->bmp_path];
        if (a->bitmap == NULL) {
            fprintf(stderr, "Fail to extract bitmap for asset %s\n", a->name);
            return 1;
        }
    }

    return 0;
}

osasset_t *
LocateAsset(HWND window, ASSET_ID id)
{
    osasset_t *ret;
    int screenWidth;
    int screenHeight;
    capture_t *cap;
    const asset_t *asset;
    INPUT input;

    asset = GetAsset(id);
    if (asset == NULL || asset->bitmap == NULL) {
        fprintf(stderr, "Fail to get asset.\n");
        return NULL;
    }

    screenWidth = GetDeviceCaps(GetDC(NULL), HORZRES);
    screenHeight = GetDeviceCaps(GetDC(NULL), VERTRES);

    cap = CaptureInit(0, 0, screenWidth, screenHeight);
    if (cap == NULL) {
        fprintf(stderr, "Fail to initialize capture.\n");
        return NULL;
    }

    ret = (osasset_t *) malloc(sizeof(*ret));
    if (ret == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    ret->id = id;

    /* Move the mouse out of the screen */
    MouseSetup(&input);
    MouseMoveAbsolute(&input, screenWidth, screenHeight);
    CaptureScreenshot(window, cap);

    if (BitmapLocate(cap->bitmap, asset->bitmap, &ret->x, &ret->y) != 0) {
        fprintf(stderr, "Fail to locate asset: %s\n", asset->name);
        CaptureDestroy(cap);
        return NULL;
    }

    ret->bitmap = BitmapDup(cap->bitmap);
    CaptureDestroy(cap);
    return ret;
}

item_t *
IdentifySlotContent(btm_t *slot_btm)
{
    item_t *item;
    const asset_t *grade;
    double match_score;

    if (slot_btm == NULL || slot_btm->width != ICON_WIDTH || slot_btm->height != ICON_HEIGHT) {
        return NULL;
    }

    item = ItemInit();
    if (item == NULL) {
        fprintf(stderr, "Fail to init item.\n");
        return NULL;
    }

    item->asset = FindMatchingIcon(slot_btm);
    if (item->asset == NULL) {
        ItemDestroy(item);
        return NULL;
    }

    /*
     * Check this asset score:
     *  - Unknown items will match something but score will be bad
     */
    match_score = BitmapCmp(slot_btm, item->asset->bitmap, false);
    if (match_score > ASSET_MATCHING_THRESHOLD) {
        printf("Unknown: best match is %s with %f > %f\n\n", item->asset->name, match_score, ASSET_MATCHING_THRESHOLD);
        BitmapCmp(slot_btm, item->asset->bitmap, true);
        printf("\n\n");
        item->asset = GetAsset(Unknown);
        return item;
    }

    if (item->asset->flags & ITEM_GRADED) {
        grade = FindMatchingGrade(slot_btm);
        item->grade = grade->id;
    }

    return item;
}

void
OSAssetDestroy(osasset_t *p)
{
    if (p != NULL) {
        BitmapDestroy(p->bitmap);
        free(p);
    }
}

item_t *
ItemInit(void)
{
    item_t *item;

    item = (item_t *) calloc(1, sizeof(*item));
    if (item == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }

    item->grade = -1;

    return item;
}

void
ItemDestroy(item_t *i)
{
    if (i != NULL) {
        free(i);
    }
}