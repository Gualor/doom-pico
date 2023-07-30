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
#define MAX_RENDER_DEPTH 12
#define MAX_SPRITE_DEPTH 8

/* Faster rendering of vertical lines */
#define OPTIMIZE_RAYCASTING 1

/* Depth buffer for sprites */
#define ZBUFFER_SIZE ((SCREEN_WIDTH / Z_RES_DIVIDER) + 4)

/* Display */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define HALF_WIDTH (SCREEN_WIDTH / 2)
#define HALF_HEIGHT (SCREEN_HEIGHT / 2)
#define RENDER_HEIGHT 58
#define HUD_HEIGHT 6

/* Level */
#define LEVEL_WIDTH_BASE 6
#define LEVEL_WIDTH (1 << LEVEL_WIDTH_BASE)
#define LEVEL_HEIGHT 57
#define LEVEL_SIZE ((LEVEL_WIDTH / 2) * LEVEL_HEIGHT)

/* Game */
#define GUN_TARGET_POS 18
#define GUN_SHOT_POS (GUN_TARGET_POS + 8)

#define ROT_SPEED 0.05f
#define MOV_SPEED 0.1f
#define GUN_SPEED 2.5f
#define JOGGING_SPEED 0.005f
#define ENEMY_SPEED 0.04f
#define FIREBALL_SPEED 0.2f
#define FIREBALL_ANGLES 45.0f

#define MAX_ENTITIES 12
#define MAX_STATIC_ENTITIES 24

#define MAX_ENTITY_DISTANCE 200
#define MAX_ENEMY_VIEW 90
#define ITEM_COLLIDER_DIST 6
#define ENEMY_COLLIDER_DIST 4
#define FIREBALL_COLLIDER_DIST 3
#define ENEMY_MELEE_DIST 9
#define WALL_COLLIDER_DIST 0.2f

#define ENEMY_MELEE_DAMAGE_LOW 18
#define ENEMY_MELEE_DAMAGE_MED 27
#define ENEMY_MELEE_DAMAGE_HIGH 36

#define ENEMY_FIREBALL_DAMAGE_LOW 25
#define ENEMY_FIREBALL_DAMAGE_MED 50
#define ENEMY_FIREBALL_DAMAGE_HIGH 75

#define GUN_MAX_DAMAGE_HIGH 80
#define GUN_MAX_DAMAGE_MED 60
#define GUN_MAX_DAMAGE_LOW 40

#define PLAYER_MAX_HEALTH 100
#define MEDKIT_HEAL_LOW 25
#define MEDKIT_HEAL_MED 50
#define MEDKIT_HEAL_HIGH 65

#define PLAYER_MAX_AMMO 255
#define AMMO_PICKUP_LOW 9
#define AMMO_PICKUP_MED 10
#define AMMO_PICKUP_HIGH 13

#define E1M1_ENEMY_GOAL 20
#define E1M2_ENEMY_GOAL 8

#define SCORE_SECRET_ENDING 200

#define TEXT_SPACING 1

#endif /* CONSTANTS_H */

/* -------------------------------------------------------------------------- */