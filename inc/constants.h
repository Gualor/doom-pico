/* Header guard ------------------------------------------------------------- */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Definitions -------------------------------------------------------------- */

/* Frame rate */
#define FPS 15
#define FRAME_TIME (1000.0f / FPS)

/* Higher values will result in lower horizontal resolution when rasterize and
 * lower process and memory usage. Lower will require more process and memory,
 * but looks nicer. */
#define RES_DIVIDER 2

/* Zbuffer resolution divider. We sacrifice resolution to save memory. */
#define Z_RES_DIVIDER 2

/* Distances are stored as uint8_t, multiplying the distance we can obtain more
 * precision taking care of keep numbers inside the type range.
 * Max is 256 / MAX_RENDER_DEPTH */
#define DISTANCE_MULTIPLIER 20
#define MAX_RENDER_DEPTH    12
#define MAX_SPRITE_DEPTH    8

/* Faster rendering of vertical lines */
#define OPTIMIZE_RAYCASTING 1

/* Depth buffer for sprites */
#define ZBUFFER_SIZE ((SCREEN_WIDTH / Z_RES_DIVIDER) + 4)

/* Display */
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define RENDER_HEIGHT 58
#define HUD_HEIGHT    6

/* Display colors */
#define COLOR_BLACK 0
#define COLOR_WHITE 1

/* Game */
#define GUN_TARGET_POS 18
#define GUN_SHOT_POS   (GUN_TARGET_POS + 8)

#define ROT_SPEED       0.05f
#define MOV_SPEED       0.1f
#define GUN_SPEED       2.5f
#define JOGGING_SPEED   0.005f
#define ENEMY_SPEED     0.04f
#define FIREBALL_SPEED  0.2f
#define FIREBALL_ANGLES 45.0f

#define MAX_ENTITIES           12
#define MAX_STATIC_ENTITIES    24
#define MAX_ENTITY_DISTANCE    200
#define MAX_ENEMY_VIEW         90
#define ITEM_COLLIDER_DIST     6
#define ENEMY_COLLIDER_DIST    4
#define FIREBALL_COLLIDER_DIST 3
#define ENEMY_MELEE_DIST       9
#define WALL_COLLIDER_DIST     0.2f

#define ENEMY_MELEE_DAMAGE_EASY      20
#define ENEMY_MELEE_DAMAGE_NORMAL    30
#define ENEMY_MELEE_DAMAGE_HARD      40
#define ENEMY_MELEE_DAMAGE_VERY_HARD 50

#define ENEMY_FIREBALL_DAMAGE_EASY      25
#define ENEMY_FIREBALL_DAMAGE_NORMAL    40
#define ENEMY_FIREBALL_DAMAGE_HARD      60
#define ENEMY_FIREBALL_DAMAGE_VERY_HARD 80

#define GUN_MAX_DAMAGE_EASY      80
#define GUN_MAX_DAMAGE_NORMAL    60
#define GUN_MAX_DAMAGE_HARD      40
#define GUN_MAX_DAMAGE_VERY_HARD 20

#define PLAYER_MAX_HEALTH     100
#define MEDKIT_HEAL_EASY      100
#define MEDKIT_HEAL_NORMAL    80
#define MEDKIT_HEAL_HARD      50
#define MEDKIT_HEAL_VERY_HARD 30

#define PLAYER_MAX_AMMO       255
#define AMMO_PICKUP_EASY      13
#define AMMO_PICKUP_NORMAL    10
#define AMMO_PICKUP_HARD      9
#define AMMO_PICKUP_VERY_HARD 5

#define ENEMY_KILL_GOAL1 20
#define ENEMY_KILL_GOAL2 8

#define SCORE_SECRET_ENDING 200

/* Settings */
#define BUTTON_PRESS_WAIT 200

#endif /* CONSTANTS_H */

/* -------------------------------------------------------------------------- */