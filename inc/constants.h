/* Header guard ------------------------------------------------------------- */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Definitions -------------------------------------------------------------- */

/* Frame rate */
#define FPS 15
#define FRAME_TIME (1000.0f / FPS)
/* Display */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define HALF_WIDTH (SCREEN_WIDTH / 2)
#define HALF_HEIGHT (SCREEN_HEIGHT / 2)
#define RENDER_HEIGHT 58 // Raycaster working height
#define HUD_HEIGHT 6     // HUD working height

#define RES_DIVIDER 2          // Higher values will result in lower horizontal resolution when rasterize and lower process and memory usage
                               // Lower will require more process and memory, but looks nicer
#define Z_RES_DIVIDER 2        // Zbuffer resolution divider. We sacrifice resolution to save memory
#define DISTANCE_MULTIPLIER 20 // Distances are stored as uint8_t, multiplying the distance we can obtain more precision taking care
                               // of keep numbers inside the type range. Max is 256 / MAX_RENDER_DEPTH
#define MAX_RENDER_DEPTH 12
#define MAX_SPRITE_DEPTH 8

#define ZBUFFER_SIZE SCREEN_WIDTH / Z_RES_DIVIDER + 4

// Level
#define LEVEL_WIDTH_BASE 6
#define LEVEL_WIDTH (1 << LEVEL_WIDTH_BASE)
#define LEVEL_HEIGHT 57
#define LEVEL_SIZE LEVEL_WIDTH / 2 * LEVEL_HEIGHT

// scenes
#define INTRO 0
#define GAME_PLAY 1
#define DIFF 2
#define MUS 3
#define MID 4
#define SCORE 5
// Game
#define GUN_TARGET_POS 18
#define GUN_SHOT_POS GUN_TARGET_POS + 8

#define ROT_SPEED .12
#define MOV_SPEED .1
#define MOV_SPEED_INV 2.5 // 1 / MOV_SPEED

#define JOGGING_SPEED .005
#define ENEMY_SPEED .04
#define FIREBALL_SPEED .2
#define FIREBALL_ANGLES 45 // Num of angles per PI

#define MAX_ENTITIES 12        // Max num of active entities
#define MAX_STATIC_ENTITIES 24 // Max num of entities in sleep mode

#define MAX_ENTITY_DISTANCE 200  // * DISTANCE_MULTIPLIER
#define MAX_ENEMY_VIEW 90        // * DISTANCE_MULTIPLIER
#define ITEM_COLLIDER_DIST 6     // * DISTANCE_MULTIPLIER
#define ENEMY_COLLIDER_DIST 4    // * DISTANCE_MULTIPLIER
#define FIREBALL_COLLIDER_DIST 3 // * DISTANCE_MULTIPLIER
#define ENEMY_MELEE_DIST 9       // * DISTANCE_MULTIPLIER
#define WALL_COLLIDER_DIST .2

#define ENEMY_MELEE_DAMAGE 18
#define ENEMY_FIREBALL_DAMAGE 25
#define GUN_MAX_DAMAGE 80

#endif /* CONSTANTS_H */

/* -------------------------------------------------------------------------- */