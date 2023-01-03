
#define DEV

//#define SSHOT_ON_AUTOSAVE

//#define SLOW_LOAD

// Careful - can seem to break things 
//#define FAST_PROGRESS

//#define DEMO

//#define TITLE_LOGO_ONLY

//#define ONLY_SLOW_TICKS 

//#define ONLY_FAST_TICKS

//#define SYNCHRONOUS_SAVE_ENABLED

// Aggressive free: dealocate used sprites
// Agressive alocate: allocate all sprites at load
// Neither: Allocate as we go, don't free

//#define AGGRESSIVE_FREE_CARGO_SPRITES

//#define AGGRESSIVE_ALLOCATE_CARGO_SPRITES

//#define PRINT_MAX_RECURSION

//#define ALLOW_ROTATED_MENU_ICONS

#define ALWAYS_FPS false

#define PRETEND_ZOOMED_IN false

/// ///

#define TICK_FREQUENCY 32

#define NEAR_TICK_FREQUENCY 2
#define FAR_TICK_FREQUENCY 16

#define NEAR_TICK_AMOUNT 1
#define FAR_TICK_AMOUNT 8

#define TICKS_PER_SEC (NEAR_TICK_AMOUNT*TICK_FREQUENCY/NEAR_TICK_FREQUENCY)

#define TILE_PIX 16

#define EXTRACTOR_PIX (3*TILE_PIX)


#define DEVICE_PIX_X 400
#define DEVICE_PIX_Y 240

#define SCREEN_PIX_X (DEVICE_PIX_X-TILE_PIX)
#define SCREEN_PIX_Y (DEVICE_PIX_Y-TILE_PIX)

#define CHUNK_PIX_X (SCREEN_PIX_X/2)
#define CHUNK_PIX_Y (SCREEN_PIX_Y/2)

#define CHUNK_NEIGHBORS_CORNER 3
#define CHUNK_NEIGHBORS_ALL 8

// The -1 is for "me"
#define CHUNK_NONNEIGHBORS_CORNER (TOT_CHUNKS - CHUNK_NEIGHBORS_CORNER - 1) 
#define CHUNK_NONNEIGHBORS_ALL (TOT_CHUNKS - CHUNK_NEIGHBORS_ALL - 1)

#define TILES_PER_CHUNK_X (CHUNK_PIX_X/TILE_PIX)
#define TILES_PER_CHUNK_Y (CHUNK_PIX_Y/TILE_PIX)
#define TILES_PER_CHUNK (TILES_PER_CHUNK_X*TILES_PER_CHUNK_Y)

// 16x16 sprite sheet has how many rows/columns
#define SHEET16_SIZE_X 16
#define SHEET16_SIZE_Y 26
#define SHEET16_SIZE (SHEET16_SIZE_X*SHEET16_SIZE_Y)

#define SHEET18_SIZE_X 6
#define SHEET18_SIZE_Y 8
#define SHEET18_SIZE (SHEET18_SIZE_X*SHEET18_SIZE_Y)

#define SHEET48_SIZE_X 4
#define SHEET48_SIZE_Y 8
#define SHEET48_SIZE (SHEET48_SIZE_X*SHEET48_SIZE_Y)

#define TOT_CARGO_OR_BUILDINGS 8192

// Number of available floor sprites
#define FLOOR_TYPES 6
#define FLOOR_VARIETIES 8
#define TOT_FLOOR_TILES (FLOOR_VARIETIES*FLOOR_TYPES)
#define TOT_FLOOR_TILES_INC_LANDFILL (TOT_FLOOR_TILES+1)
#define TOT_FLOOR_TILES_INC_PAVED (FLOOR_VARIETIES*(FLOOR_TYPES+1))

// Where the conveyor tiles start from in Y
#define CONV_START_Y 7

#define TUTORIAL_DISABLED 255
#define TUTORIAL_FINISHED 254

 #define TUT_Y_SPACING 13
 #define TUT_Y_SHFT 4

#define MAX_DROP_RATE 8

#define TRAUMA_DECAY 0.05f;
#define TRAUMA_AMPLIFICATION 4

// How big is the world in X and Y
#define WORLD_CHUNKS_X 12
#define WORLD_CHUNKS_Y 12
#define TOT_CHUNKS (WORLD_CHUNKS_X*WORLD_CHUNKS_Y)

#define TOT_WORLD_PIX_X (WORLD_CHUNKS_X*CHUNK_PIX_X)
#define TOT_WORLD_PIX_Y (WORLD_CHUNKS_Y*CHUNK_PIX_Y)


#define TOT_TILES_X (TILES_PER_CHUNK_X*WORLD_CHUNKS_X)
#define TOT_TILES_Y (TILES_PER_CHUNK_Y*WORLD_CHUNKS_Y)
#define TOT_TILES (TOT_TILES_X*TOT_TILES_Y)

// Player acceleration and friction
#define PLAYER_A 3.5f
// 1 = np friction
#define PLAYER_FRIC 0.5f

#define SQRT_HALF 0.70710678118f

// Fraction of the screen to trigger scrolling
//#define SCROLL_EDGE 0.8f

#define Z_INDEX_UI_RIGHT 32765
#define Z_INDEX_UI_BOTTOM 32766

#define Z_INDEX_UI_TTT 32767
#define Z_INDEX_UI_TT 32766
#define Z_INDEX_UI_T 32765
#define Z_INDEX_UI_M 32764
#define Z_INDEX_UI_B 32763
#define Z_INDEX_UI_BB 32762
#define Z_INDEX_UI_BBB 32761
#define Z_INDEX_UI_BACK 32760


#define Z_INDEX_MAX 32767
#define Z_INDEX_CARGO_FLOOR (Z_INDEX_UI_BBB-16)
#define Z_INDEX_CARGO_BELT (Z_INDEX_CARGO_FLOOR-16)
#define Z_INDEX_TRUCK (Z_INDEX_CARGO_FLOOR+1)
#define Z_INDEX_BLUEPRINT 32764
#define Z_INDEX_CONVEYOR 1
#define Z_INDEX_FLOOR -1

#define Z_INDEX_PLAYER 32

#define UI_ITEMS 12

#define UI_ROTATE_ACTION 90.0f

// Note: 1 larger than actual zoom levels (0 index unused)
// CAUTION: changing this will break the chunk->obstacle add/remove 
#define ZOOM_LEVELS 3

// 5 fixed inputs plus 1 fixed output. Or three variable self-describings outputs. 
//CAUTION: changing this currently requires manual change to de/serialisation
#define MAX_STORE 6

// Number of crops before a plot upgrades to farmland
#define N_CROPS_BEFORE_FARMLAND 48

//CAUTION: changing this currently requires manual change to de/serialisation
#define WORLD_SAVE_SLOTS 8

// Versioning
#define SAVE_FORMAT 4

#define ACTIVATE_DISTANCE (TILE_PIX*3)

// Shrink the collision boxes from the edge of the sprite, feels better
#define COLLISION_OFFSET_BIG TILE_PIX
#define COLLISION_OFFSET_SMALL (TILE_PIX/2 + 2)

// Out of 1000
#define CLUTTER_CHANCE 2
#define MULTI_CLUTTER_CHANCE 50

#define MULTI_CLUTTER_MIN 6
#define MULTI_CLUTTER_MAX 12

#define UI_TITLE_OFFSET (TILE_PIX*1)

#define N_SAVES 3

#define N_MUSIC_TRACKS 5

#define BUTTON_PRESSED_FRAMES 8

#define CREDITS_DELAY (2*TICK_FREQUENCY)

#define CREDITS_FROM_ROW 31
#define CREDITS_TO_ROW 75
#define SETTINGS_TO_ROW 55

#define FAST_PROGRESS_SALES 10

#define STARTING_MONEY 50

#define MAX_RECURSION 128

#define SSHOT_PIXELS_PER_FRAME 16384

#define VERSION "v1.1"