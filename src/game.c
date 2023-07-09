/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "constants.h"
#include "coords.h"
#include "display.h"
#include "entities.h"
#include "input.h"
#include "level.h"
#include "platform.h"
#include "sound.h"
#include "sprites.h"
#include "utils.h"

/** TODO: verify if using pointers instead of returning struct is faster */
/** TODO: remove all raylib dependencies */
/** TODO: integrate doom brutality expansion */

/* Data types --------------------------------------------------------------- */

typedef enum
{
    SCENE_INTRO,
    SCENE_LEVEL1,
    /* Add more levels here */
} GameScene;

/* Function prototypes ------------------------------------------------------ */

/* Level */
static void game_init_level_scene(const uint8_t level[]);

/* Entities */
static EntityType game_get_level_entity(const uint8_t level[], uint8_t x,
                                        uint8_t y);
static bool game_is_entity_spawned(EntityUID uid);
static bool game_is_static_entity_spawned(EntityUID uid);
static void game_spawn_entity(EntityType type, uint8_t x, uint8_t y);
static void game_spawn_fireball(float x, float y);
static void game_remove_entity(EntityUID uid);
static void game_remove_static_entity(EntityUID uid);
static void game_update_entities(const uint8_t level[]);
static void game_sort_entities(void);

/* Game mechanics */
static EntityUID game_detect_collision(const uint8_t level[], Coords *pos,
                                       float rel_x, float rel_y,
                                       bool only_walls);
static EntityUID game_update_position(const uint8_t level[], Coords *pos,
                                      float rel_x, float rel_y,
                                      bool only_walls);
static void game_fire_shootgun(void);

/* Graphics */
static Coords game_translate_into_view(Coords *pos);
static void game_render_map(const uint8_t level[], float view_height);
static void game_render_entities(float view_height);
static void game_render_gun(uint8_t gun_pos, float jogging);
static void game_render_hud(void);

/* Scenes */
static void game_jump_to_scene(GameScene scene);
static void game_run_intro_scene(void);
static void game_run_level_scene(void);

/* Global variables --------------------------------------------------------- */

/* Entities */
static Player player;
static Entity entity[MAX_ENTITIES];
static uint8_t num_entities;
static StaticEntity static_entity[MAX_STATIC_ENTITIES];
static uint8_t num_static_entities;

/* Graphics */
static bool flash_screen;
static uint8_t fade_screen;

/* Sound effects */
static bool walk_sound_toggle;

/* Scene */
static void (*game_run_scene)(void);

/* Function definitions ----------------------------------------------------- */

/**
 * @brief GAME jump to another scene at the end of the current frame.
 *
 * @param scene Game scene
 */
void game_jump_to_scene(GameScene scene)
{
    switch (scene)
    {
    case SCENE_INTRO:
        game_run_scene = game_run_intro_scene;
        break;

    case SCENE_LEVEL1:
        game_init_level_scene(level_1);
        game_run_scene = game_run_level_scene;
        break;

    default:
        game_run_scene = game_run_intro_scene;
        break;
    }
}

/**
 * @brief GAME initialize level state.
 *
 * @param level Level byte map
 */
void game_init_level_scene(const uint8_t level[])
{
    // Initialize game entities
    memset(entity, 0x00, sizeof(Entity) * MAX_ENTITIES);
    memset(static_entity, 0x00, sizeof(StaticEntity) * MAX_STATIC_ENTITIES);
    num_entities = 0;
    num_static_entities = 0;

    // Initialize screen effects
    flash_screen = false;
    fade_screen = GRADIENT_COUNT - 1;

    // Initialize audio effects
    walk_sound_toggle = false;

    // Initialize game scene callback
    game_run_scene = game_run_intro_scene;

    // Find player in the map and create instance
    for (uint8_t y = LEVEL_HEIGHT - 1; y >= 0; y--)
    {
        for (uint8_t x = 0; x < LEVEL_WIDTH; x++)
        {
            uint8_t block = game_get_level_entity(level, x, y);

            if (block == E_PLAYER)
            {
                player = entities_create_player(x, y);
                return;
            }

            /** TODO: Create other static entities */
        }
    }
}

/**
 * @brief GAME get entity type from level byte map.
 *
 * @param level Level byte map
 * @param x     X coordinate
 * @param y     Y coordinate
 * @return EntityType
 */
EntityType game_get_level_entity(const uint8_t level[], uint8_t x, uint8_t y)
{
    if ((x < 0) || (x >= LEVEL_WIDTH) || (y < 0) || (y >= LEVEL_HEIGHT))
        return E_FLOOR;

    // Y is read in inverse order
    uint8_t byte = level[((LEVEL_HEIGHT - 1 - y) * LEVEL_WIDTH + x) / 2];
    byte >>= (x % 2) ? 0 : 4;

    return byte & 0x0f;
}

/**
 * @brief GAME check if an entity with given UID is already spawned.
 *
 * @param uid Entity UID number
 * @return true
 * @return false
 */
bool game_is_entity_spawned(EntityUID uid)
{
    for (uint8_t i = 0; i < num_entities; i++)
    {
        if (entity[i].uid == uid)
            return true;
    }

    return false;
}

/**
 * @brief GAME check if a static entity with given UID is already spawned.
 *
 * @param uid Entity UID number
 * @return true
 * @return false
 */
bool game_is_static_entity_spawned(EntityUID uid)
{
    for (uint8_t i = 0; i < num_static_entities; i++)
    {
        if (static_entity[i].uid == uid)
            return true;
    }

    return false;
}

/**
 * @brief GAME spawn a new entity at a given location.
 *
 * @param type Entity type
 * @param x    X coordinate
 * @param y    Y coordinate
 */
void game_spawn_entity(EntityType type, uint8_t x, uint8_t y)
{
    // Limit the number of spawned entities
    if (num_entities >= MAX_ENTITIES)
        return;

    /** TODO: Read static entity status */

    switch (type)
    {
    case E_ENEMY:
        entity[num_entities] = entities_create_enemy(x, y);
        num_entities++;
        break;

    case E_KEY:
        entity[num_entities] = entities_create_key(x, y);
        num_entities++;
        break;

    case E_MEDKIT:
        entity[num_entities] = entities_create_medkit(x, y);
        num_entities++;
        break;
    }
}

/**
 * @brief GAME spawn a fireball at a given location.
 *
 * @param x X coordinate
 * @param y Y coordinate
 */
void game_spawn_fireball(float x, float y)
{
    // Limit the number of spawned entities
    if (num_entities >= MAX_ENTITIES)
        return;

    // Remove if already exists, don't throw anything. Not the best,
    // but shouldn't happen too often
    EntityUID uid = entities_get_uid(E_FIREBALL, x, y);
    if (game_is_entity_spawned(uid))
        return;

    // Calculate direction. 32 angles
    int16_t dir = FIREBALL_ANGLES *
                  ((atan2f(y - player.pos.y, x - player.pos.x) / PI) + 1);

    if (dir < 0)
        dir += FIREBALL_ANGLES * 2;

    entity[num_entities] = entities_create_fireball(x, y, dir);
    num_entities++;
}

/**
 * @brief GAME remove an entity.
 *
 * @param uid Entity UID number
 */
void game_remove_entity(EntityUID uid)
{
    uint8_t i = 0;
    bool found = false;

    while (i < num_entities)
    {
        if (!found && entity[i].uid == uid)
        {
            found = true;
            num_entities--;
        }

        if (found)
            entity[i] = entity[i + 1];

        i++;
    }
}

/**
 * @brief GAME remove a static entity.
 *
 * @param uid Entity UID number
 */
void game_remove_static_entity(EntityUID uid)
{
    uint8_t i = 0;
    bool found = false;

    while (i < num_static_entities)
    {
        if (!found && static_entity[i].uid == uid)
        {
            found = true;
            num_static_entities--;
        }

        if (found)
            static_entity[i] = static_entity[i + 1];

        i++;
    }
}

/**
 * @brief GAME detect collision between entities and level blocks.
 *
 * @param level      Level byte map
 * @param pos        Position to be checked
 * @param rel_x      X relative direction
 * @param rel_y      Y relative direction
 * @param only_walls Check only walls collisions
 * @return EntityUID
 */
EntityUID game_detect_collision(const uint8_t level[], Coords *pos, float rel_x,
                                float rel_y, bool only_walls)
{
    // Wall collision
    uint8_t round_x = pos->x + rel_x;
    uint8_t round_y = pos->y + rel_y;
    uint8_t block = game_get_level_entity(level, round_x, round_y);

    if (block == E_WALL)
    {
        sound_play(hit_wall_snd, HIT_WALL_SND_LEN);
        return entities_get_uid(block, round_x, round_y);
    }

    if (only_walls)
        return UID_NULL;

    // Entity collision
    for (uint8_t i = 0; i < num_entities; i++)
    {
        // Don't collide with itself
        if (&(entity[i].pos) == pos)
            continue;

        EntityType type = entities_get_type(entity[i].uid);

        // Only ALIVE enemy collision
        if ((type != E_ENEMY) ||
            (entity[i].state == S_DEAD) ||
            (entity[i].state == S_HIDDEN))
            continue;

        Coords new_coords = {entity[i].pos.x - rel_x, entity[i].pos.y - rel_y};
        uint8_t distance = coords_compute_distance(pos, &new_coords);

        // Check distance and if it's getting closer
        if ((distance < ENEMY_COLLIDER_DIST) &&
            (distance < entity[i].distance))
            return entity[i].uid;
    }

    return UID_NULL;
}

/**
 * @brief GAME update position if possible, otherwise return collided uid.
 *
 * @param level      Level byte map
 * @param pos        Position to be checked
 * @param rel_x      X relative direction
 * @param rel_y      Y relative direction
 * @param only_walls Check only walls collisions
 * @return EntityUID
 */
EntityUID game_update_position(const uint8_t level[], Coords *pos, float rel_x,
                               float rel_y, bool only_walls)
{
    EntityUID collide_x =
        game_detect_collision(level, pos, rel_x, 0, only_walls);
    EntityUID collide_y =
        game_detect_collision(level, pos, 0, rel_y, only_walls);

    if (!collide_x)
        pos->x += rel_x;
    if (!collide_y)
        pos->y += rel_y;

    return (collide_x || collide_y || UID_NULL);
}

/**
 * @brief GAME player fire shootgun and compute damage.
 *
 */
void game_fire_shootgun(void)
{
    sound_play(shoot_snd, SHOOT_SND_LEN);

    for (uint8_t i = 0; i < num_entities; i++)
    {
        // Shoot only ALIVE enemies
        if ((entities_get_type(entity[i].uid) != E_ENEMY) ||
            (entity[i].state == S_DEAD) ||
            (entity[i].state == S_HIDDEN))
            continue;

        Coords transform = game_translate_into_view(&(entity[i].pos));
        if ((fabsf(transform.x) < 20.0f) && (transform.y > 0.0f))
        {
            uint8_t damage = MIN(
                GUN_MAX_DAMAGE,
                GUN_MAX_DAMAGE /
                    (fabsf(transform.x) * entity[i].distance) / 5.0f);
            if (damage > 0)
            {
                entity[i].health = MAX(0, entity[i].health - damage);
                entity[i].state = S_HIT;
                entity[i].timer = 4;
            }
        }
    }
}

/**
 * @brief GAME execute entities AI logic.
 *
 * @param level Level byte map
 */
void game_update_entities(const uint8_t level[])
{
    uint8_t i = 0;
    while (i < num_entities)
    {
        // Update distance
        entity[i].distance = coords_compute_distance(
            &(player.pos), &(entity[i].pos));

        // Run the timer. Works with actual frames.
        if (entity[i].timer > 0)
            entity[i].timer--;

        // Too far away. put it in doze mode
        if (entity[i].distance > MAX_ENTITY_DISTANCE)
        {
            game_remove_entity(entity[i].uid);
            continue;
        }

        // Bypass render if hidden
        if (entity[i].state == S_HIDDEN)
        {
            i++;
            continue;
        }

        EntityType type = entities_get_type(entity[i].uid);
        switch (type)
        {
        case E_ENEMY:
        {
            // Enemy "IA"
            if (entity[i].health == 0)
            {
                if (entity[i].state != S_DEAD)
                {
                    entity[i].state = S_DEAD; // Entity is dead
                    entity[i].timer = 6;
                }
            }
            else if (entity[i].state == S_HIT)
            {
                if (entity[i].timer == 0)
                {
                    entity[i].state = S_ALERT; // Back to alert state
                    entity[i].timer = 40;      // Delay next fireball thrown
                }
            }
            else if (entity[i].state == S_FIRING)
            {
                if (entity[i].timer == 0)
                {
                    entity[i].state = S_ALERT; // Back to alert state
                    entity[i].timer = 40;      // Delay next fireball thrown
                }
            }
            else
            {
                if ((entity[i].distance > ENEMY_MELEE_DIST) &&
                    (entity[i].distance < MAX_ENEMY_VIEW))
                {
                    if (entity[i].state != S_ALERT)
                    {
                        entity[i].state = S_ALERT; // Back to alert state
                        entity[i].timer = 20;      // used to throw fireballs
                    }
                    else
                    {
                        if (entity[i].timer == 0)
                        {
                            // Throw a fireball
                            game_spawn_fireball(
                                entity[i].pos.x, entity[i].pos.y);
                            entity[i].state = S_FIRING;
                            entity[i].timer = 6;
                        }
                        else
                        {
                            // Move towards to the player
                            game_update_position(
                                level,
                                &(entity[i].pos),
                                SIGN(player.pos.x, entity[i].pos.x) *
                                    ENEMY_SPEED * delta_time,
                                SIGN(player.pos.y, entity[i].pos.y) *
                                    ENEMY_SPEED * delta_time,
                                true);
                        }
                    }
                }
                else if (entity[i].distance <= ENEMY_MELEE_DIST)
                {
                    if (entity[i].state != S_MELEE)
                    {
                        // Preparing the melee attack
                        entity[i].state = S_MELEE;
                        entity[i].timer = 10;
                    }
                    else if (entity[i].timer == 0)
                    {
                        // Melee attack
                        player.health =
                            MAX(0, player.health - ENEMY_MELEE_DAMAGE);
                        entity[i].timer = 14;
                        flash_screen = true;
                    }
                }
                else
                {
                    entity[i].state = S_STAND;
                }
            }
            break;
        }

        case E_FIREBALL:
        {
            if (entity[i].distance < FIREBALL_COLLIDER_DIST)
            {
                // Hit the player and disappear
                player.health =
                    MAX(0, player.health - ENEMY_FIREBALL_DAMAGE);
                flash_screen = true;
                game_remove_entity(entity[i].uid);
                continue;
            }
            else
            {
                // Move, only collide with walls.
                // Note: using health to store the angle of the movement
                EntityUID collided = game_update_position(
                    level,
                    &(entity[i].pos),
                    cosf((float)entity[i].health / FIREBALL_ANGLES * PI) *
                        FIREBALL_SPEED,
                    sinf((float)entity[i].health / FIREBALL_ANGLES * PI) *
                        FIREBALL_SPEED,
                    true);

                if (collided)
                {
                    game_remove_entity(entity[i].uid);
                    continue;
                }
            }
            break;
        }

        case E_MEDKIT:
        {
            if (entity[i].distance < ITEM_COLLIDER_DIST)
            {
                // Pickup
                sound_play(medkit_snd, MEDKIT_SND_LEN);
                entity[i].state = S_HIDDEN;
                player.health = MIN(100, player.health + 50);
                flash_screen = true;
            }
            break;
        }

        case E_KEY:
        {
            if (entity[i].distance < ITEM_COLLIDER_DIST)
            {
                // Pickup
                sound_play(get_key_snd, GET_KEY_SND_LEN);
                entity[i].state = S_HIDDEN;
                player.keys++;
                flash_screen = true;
            }
            break;
        }
        }

        i++;
    }
}

/**
 * @brief GAME render map with raycasting technique.
 * NOTE: Based on https://lodev.org/cgtutor/raycasting.html
 *
 * @param level       Level byte map
 * @param view_height View height of the camera
 */
void game_render_map(const uint8_t level[], float view_height)
{
    EntityUID last_uid;

    for (uint8_t x = 0; x < SCREEN_WIDTH; x += RES_DIVIDER)
    {
        float camera_x = 2 * (float)x / SCREEN_WIDTH - 1;
        float ray_x = player.dir.x + player.plane.x * camera_x;
        float ray_y = player.dir.y + player.plane.y * camera_x;
        uint8_t map_x = (uint8_t)(player.pos.x);
        uint8_t map_y = (uint8_t)(player.pos.y);
        Coords map_coords = {player.pos.x, player.pos.y};
        float delta_x = fabsf(1 / ray_x);
        float delta_y = fabsf(1 / ray_y);

        int8_t step_x;
        int8_t step_y;
        float side_x;
        float side_y;

        if (ray_x < 0)
        {
            step_x = -1;
            side_x = (player.pos.x - map_x) * delta_x;
        }
        else
        {
            step_x = 1;
            side_x = (map_x + 1.0 - player.pos.x) * delta_x;
        }

        if (ray_y < 0)
        {
            step_y = -1;
            side_y = (player.pos.y - map_y) * delta_y;
        }
        else
        {
            step_y = 1;
            side_y = (map_y + 1.0 - player.pos.y) * delta_y;
        }

        // Wall detection
        uint8_t depth = 0;
        bool hit = false;
        bool side;
        while (!hit && (depth < MAX_RENDER_DEPTH))
        {
            if (side_x < side_y)
            {
                side_x += delta_x;
                map_x += step_x;
                side = false;
            }
            else
            {
                side_y += delta_y;
                map_y += step_y;
                side = true;
            }

            uint8_t block = game_get_level_entity(level, map_x, map_y);

            if (block == E_WALL)
                hit = true;
            else
            {
                // Spawning entities here, as soon they are visible for the
                // player. Not the best place, but would be a very performance
                // cost scan for them in another loop
                if ((block == E_ENEMY) || (block & 0b00001000))
                {
                    // Check that it's close to the player
                    if (coords_compute_distance(&(player.pos), &map_coords) <
                        MAX_ENTITY_DISTANCE)
                    {
                        EntityUID uid = entities_get_uid(block, map_x, map_y);
                        if (last_uid != uid && !game_is_entity_spawned(uid))
                        {
                            game_spawn_entity(block, map_x, map_y);
                            last_uid = uid;
                        }
                    }
                }
            }

            depth++;
        }

        if (hit)
        {
            float distance;
            if (!side)
                distance =
                    MAX(1, (map_x - player.pos.x + (1 - step_x) / 2) / ray_x);
            else
                distance =
                    MAX(1, (map_y - player.pos.y + (1 - step_y) / 2) / ray_y);

            // store zbuffer value for the column
            zbuffer[x / Z_RES_DIVIDER] =
                MIN(distance * DISTANCE_MULTIPLIER, 0xff);

            // rendered line height
            uint8_t line_height = RENDER_HEIGHT / distance;

            display_draw_vline(
                x,
                view_height / distance - line_height / 2 + RENDER_HEIGHT / 2,
                view_height / distance + line_height / 2 + RENDER_HEIGHT / 2,
                GRADIENT_COUNT - (side * 2) -
                    (distance / MAX_RENDER_DEPTH * GRADIENT_COUNT));
        }
    }
}

/**
 * @brief GAME sort entities from far to close.
 *
 */
void game_sort_entities(void)
{
    uint8_t gap = num_entities;
    bool swapped = false;
    while ((gap > 1) || (swapped))
    {
        // shrink factor 1.3
        gap = (gap * 10) / 13;
        if ((gap == 9) || (gap == 10))
            gap = 11;
        if (gap < 1)
            gap = 1;

        swapped = false;
        for (uint8_t i = 0; i < num_entities - gap; i++)
        {
            uint8_t j = i + gap;
            if (entity[i].distance < entity[j].distance)
            {
                SWAP(entity[i], entity[j]);
                swapped = true;
            }
        }
    }
}

/**
 * @brief GAME translate 2D map coordinates into camera coordinates.
 *
 * @param pos 2D map coordinates
 * @return Coords
 */
Coords game_translate_into_view(Coords *pos)
{
    // Translate sprite position to relative to camera
    float sprite_x = pos->x - player.pos.x;
    float sprite_y = pos->y - player.pos.y;

    // Required for correct matrix multiplication
    float inv_det =
        1.0f / (player.plane.x * player.dir.y - player.dir.x * player.plane.y);
    float transform_x =
        inv_det * (player.dir.y * sprite_x - player.dir.x * sprite_y);
    float transform_y =
        inv_det * (-player.plane.y * sprite_x + player.plane.x * sprite_y);

    return (Coords){transform_x, transform_y};
}

/**
 * @brief GAME render entities sprites.
 *
 * @param view_height View height of the camera
 */
void game_render_entities(float view_height)
{
    game_sort_entities();

    for (uint8_t i = 0; i < num_entities; i++)
    {
        if (entity[i].state == S_HIDDEN)
            continue;

        Coords transform = game_translate_into_view(&(entity[i].pos));

        // don´t render if behind the player or too far away
        if ((transform.y <= 0.1f) || (transform.y > MAX_SPRITE_DEPTH))
            continue;

        int16_t sprite_screen_x =
            HALF_WIDTH * (1.0f + (transform.x / transform.y));
        int8_t sprite_screen_y =
            (RENDER_HEIGHT / 2) + (view_height / transform.y);

        // Don't try to render if outside of screen
        // doing this pre-shortcut due int16 -> int8 conversion
        // makes out-of-screen values fit into the screen space
        if ((sprite_screen_x < -HALF_WIDTH) ||
            (sprite_screen_x > SCREEN_WIDTH + HALF_WIDTH))
            continue;

        switch (entities_get_type(entity[i].uid))
        {
        case E_ENEMY:
        {
            uint8_t sprite;
            if (entity[i].state == S_ALERT)
                sprite = (platform_millis() / 500) % 2; // Walking
            else if (entity[i].state == S_FIRING)
                sprite = 2; // Fireball
            else if (entity[i].state == S_HIT)
                sprite = 3; // Hit
            else if (entity[i].state == S_MELEE)
                sprite = entity[i].timer > 10 ? 2 : 1; // Melee atack
            else if (entity[i].state == S_DEAD)
                sprite = entity[i].timer > 0 ? 3 : 4; // Dying
            else
                sprite = 0; // Stand

            display_draw_sprite(
                sprite_screen_x - BMP_IMP_WIDTH * 0.5f / transform.y,
                sprite_screen_y - 8 / transform.y, bmp_imp_bits,
                bmp_imp_mask,
                BMP_IMP_WIDTH,
                BMP_IMP_HEIGHT,
                sprite,
                transform.y);
            break;
        }

        case E_FIREBALL:
        {
            display_draw_sprite(
                sprite_screen_x - BMP_FIREBALL_WIDTH / 2 / transform.y,
                sprite_screen_y - BMP_FIREBALL_HEIGHT / 2 / transform.y,
                bmp_fireball_bits,
                bmp_fireball_mask,
                BMP_FIREBALL_WIDTH,
                BMP_FIREBALL_HEIGHT,
                0,
                transform.y);
            break;
        }

        case E_MEDKIT:
        {
            display_draw_sprite(
                sprite_screen_x - BMP_ITEMS_WIDTH / 2 / transform.y,
                sprite_screen_y + 5 / transform.y,
                bmp_items_bits,
                bmp_items_mask,
                BMP_ITEMS_WIDTH,
                BMP_ITEMS_HEIGHT,
                0,
                transform.y);
            break;
        }

        case E_KEY:
        {
            display_draw_sprite(
                sprite_screen_x - BMP_ITEMS_WIDTH / 2 / transform.y,
                sprite_screen_y + 5 / transform.y,
                bmp_items_bits,
                bmp_items_mask,
                BMP_ITEMS_WIDTH,
                BMP_ITEMS_HEIGHT,
                1,
                transform.y);
            break;
        }
        }
    }
}

/**
 * @brief GAME render player gun sprite.
 *
 * @param gun_pos Gun cyclic position
 * @param jogging Player jogging speed
 */
void game_render_gun(uint8_t gun_pos, float jogging)
{
    // jogging
    uint8_t x = 48 + sinf(platform_millis() * JOGGING_SPEED) * 10 * jogging;
    uint8_t y = RENDER_HEIGHT - gun_pos +
                fabsf(cosf(platform_millis() * JOGGING_SPEED)) * 8 * jogging;

    // Gun fire
    if (gun_pos > GUN_SHOT_POS - 2)
        display_draw_bitmap(x + 6, y - 11, bmp_fire_bits, BMP_FIRE_WIDTH,
                            BMP_FIRE_HEIGHT, true);

    // Don't draw over the hud
    uint8_t clip_height = MAX(0, MIN(y + BMP_GUN_HEIGHT, RENDER_HEIGHT) - y);

    // Draw the gun (black mask + actual sprite)
    display_draw_bitmap(x, y, bmp_gun_mask, BMP_GUN_WIDTH, clip_height, false);
    display_draw_bitmap(x, y, bmp_gun_bits, BMP_GUN_WIDTH, clip_height, true);
}

/**
 * @brief GAME render heads-up display (HUD).
 *
 */
void game_render_hud(void)
{
    // Clear HUD
    display_draw_rect(0, RENDER_HEIGHT, SCREEN_WIDTH, HUD_HEIGHT, false);

    // Draw HUD symbols
    display_draw_text(2, RENDER_HEIGHT, "{}", false);
    display_draw_text(40, RENDER_HEIGHT, "[]", false);

    // Update stats
    display_draw_int(12, RENDER_HEIGHT, player.health);
    display_draw_int(50, RENDER_HEIGHT, player.keys);
    display_draw_int(114, RENDER_HEIGHT, (uint8_t)(display_get_fps()));
    display_draw_int(82, RENDER_HEIGHT, num_entities);
}

/**
 * @brief GAME run intro scene.
 *
 */
void game_run_intro_scene(void)
{
    display_draw_bitmap(
        (SCREEN_WIDTH - BMP_LOGO_WIDTH) / 2,
        (SCREEN_HEIGHT - BMP_LOGO_HEIGHT) / 3,
        bmp_logo_bits,
        BMP_LOGO_WIDTH,
        BMP_LOGO_HEIGHT,
        true);

    display_draw_text(
        SCREEN_WIDTH / 2 - 25,
        SCREEN_HEIGHT * 0.8f,
        "PRESS FIRE",
        true);

    if (input_fire())
        game_jump_to_scene(SCENE_LEVEL1);
}

/**
 * @brief GAME run level scene.
 *
 */
void game_run_level_scene(void)
{
    static bool gun_fired;
    static uint8_t gun_pos;
    static float rot_speed;
    static float old_dir_x;
    static float old_plane_x;
    static float view_height;
    static float jogging;

    // If the player is alive
    if (player.health > 0)
    {
        // Player speed
        if (input_up())
        {
            player.velocity += (MOV_SPEED - player.velocity) * 0.4f;
            jogging = fabsf(player.velocity) * MOV_SPEED_INV;
        }
        else if (input_down())
        {
            player.velocity -= (MOV_SPEED + player.velocity) * 0.4f;
            jogging = fabsf(player.velocity) * MOV_SPEED_INV;
        }
        else
        {
            player.velocity *= 0.5f;
            jogging = fabsf(player.velocity) * MOV_SPEED_INV;
        }

        // Player rotation
        if (input_right())
        {
            rot_speed = ROT_SPEED * delta_time;
            old_dir_x = player.dir.x;
            player.dir.x = player.dir.x * cosf(-rot_speed) -
                           player.dir.y * sinf(-rot_speed);
            player.dir.y = old_dir_x * sinf(-rot_speed) +
                           player.dir.y * cosf(-rot_speed);
            old_plane_x = player.plane.x;
            player.plane.x = player.plane.x * cosf(-rot_speed) -
                             player.plane.y * sinf(-rot_speed);
            player.plane.y = old_plane_x * sinf(-rot_speed) +
                             player.plane.y * cosf(-rot_speed);
        }
        else if (input_left())
        {
            rot_speed = ROT_SPEED * delta_time;
            old_dir_x = player.dir.x;
            player.dir.x = player.dir.x * cosf(rot_speed) -
                           player.dir.y * sinf(rot_speed);
            player.dir.y = old_dir_x * sinf(rot_speed) +
                           player.dir.y * cosf(rot_speed);
            old_plane_x = player.plane.x;
            player.plane.x = player.plane.x * cosf(rot_speed) -
                             player.plane.y * sinf(rot_speed);
            player.plane.y = old_plane_x * sinf(rot_speed) +
                             player.plane.y * cosf(rot_speed);
        }

        view_height =
            fabsf(sinf(platform_millis() * JOGGING_SPEED)) * 6.0f * jogging;

        if (view_height > 5.9f)
        {
            if (walk_sound_toggle)
            {
                sound_play(walk1_snd, WALK1_SND_LEN);
                walk_sound_toggle = false;
            }
            else
            {
                sound_play(walk2_snd, WALK2_SND_LEN);
                walk_sound_toggle = true;
            }
        }

        // Update gun
        if (gun_pos > GUN_TARGET_POS)
        {
            // Right after fire
            gun_pos -= 1;
        }
        else if (gun_pos < GUN_TARGET_POS)
        {
            // Showing up
            gun_pos += 2;
        }
        else if (!gun_fired && input_fire())
        {
            // Ready to fire and fire pressed
            gun_pos = GUN_SHOT_POS;
            gun_fired = true;
            game_fire_shootgun();
        }
        else if (gun_fired && !input_fire())
        {
            // Just fired and restored position
            gun_fired = false;
        }

        // Update player
        game_update_position(
            level_1,
            &(player.pos),
            player.dir.x * player.velocity * delta_time,
            player.dir.y * player.velocity * delta_time,
            false);

        // Update entities
        game_update_entities(level_1);
    }
    else
    {
        // The player is dead
        if (view_height > -10.0f)
            view_height--;
        else if (input_fire())
            game_jump_to_scene(SCENE_INTRO);

        if (gun_pos > 1)
            gun_pos -= 2;
    }

    // Render stuff
    game_render_map(level_1, view_height);
    game_render_entities(view_height);
    game_render_gun(gun_pos, jogging);

    // Fade in effect
    if (fade_screen > 0)
    {
        display_fade(fade_screen, false);
        fade_screen--;
        return;
    }
    game_render_hud();

    // Flash screen
    if (flash_screen)
    {
        // display_invert();
        flash_screen = false;
    }

    // Exit routine
    if (input_exit())
        game_jump_to_scene(SCENE_INTRO);
}

/**
 * @brief GAME main function.
 *
 */
void main(void)
{
    /* Initialize game */
    platform_init();
    display_init();
    sound_init();
    input_init();
    game_run_scene = game_run_intro_scene;

    while (!input_exit())
    {
        /* Start drawing */
        display_draw_start();

        /* Read user inputs */
        input_update();

        /* Run current game scene */
        game_run_scene();

        /* Stop drawing */
        display_draw_stop();
    }
}

/* -------------------------------------------------------------------------- */