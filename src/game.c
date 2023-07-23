/* Includes ----------------------------------------------------------------- */

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Data types --------------------------------------------------------------- */

typedef enum
{
    SCENE_INTRO,
    SCENE_DIFFICULTY,
    SCENE_MUSIC,
    SCENE_STORY,
    SCENE_LEVEL,
    SCENE_SCORE
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
static void game_clear_dead_enemy(void);
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
EntityType game_get_item_drop(void);

/* Graphics */
static Coords game_translate_into_view(Coords *pos);
static void game_render_map(const uint8_t level[], float view_height);
static void game_render_entities(float view_height);
static void game_render_gun(uint8_t pos, float jogging, bool fired,
                            uint8_t reload);
static void game_render_hud(void);

/* Scenes */
static void game_jump_to_scene(GameScene scene);
static void game_run_intro_scene(void);
static void game_run_difficulty_scene(void);
static void game_run_music_scene(void);
static void game_run_story_scene(void);
static void game_run_level_scene(void);
static void game_run_score_scene(void);

void updateHud(void); /** TODO: merge with render HUD */

/* Global variables --------------------------------------------------------- */

static uint8_t z = 6;
static bool coll = false;
static uint8_t jump = 0;
static uint8_t jump_height = 0;
static uint8_t vel = 1;
static uint8_t game_difficulty = 1;
static uint8_t noclip = 0;
static bool m = true;
static uint8_t rc1 = 0;
static int16_t a = 0;
static uint8_t enemy_count2 = 0;
static uint8_t enemy_goal2 = 8;
// game
// player and entities
static Player player;
static Entity entity[MAX_ENTITIES];
static uint8_t num_entities = 0;
static StaticEntity static_entity[MAX_STATIC_ENTITIES];
static uint8_t num_static_entities = 0;

static uint8_t del = 0;
static uint8_t enemy_count = 0;
static uint8_t enemy_goal = 20;
static bool fade_e = true;
static bool debug = false;
static uint8_t r = 0;
static bool reload1 = false;
static int16_t game_score;
static uint8_t k;
static int8_t mid = 1;
static bool bss = false;
static bool mc = false;
// init
static bool gun_fired = false;
static bool walk_sound_toggle = false;
static uint8_t gun_pos = 0;
static float rot_speed;
static float old_dir_x;
static float old_plane_x;
static float view_height;
static float jogging;

static bool flash_screen = false;
static uint8_t fade_screen = GRADIENT_COUNT - 1;

static const uint8_t *game_level = E1M1;
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

    case SCENE_SCORE:
        game_run_scene = game_run_score_scene;
        break;

    case SCENE_MUSIC:
        game_run_scene = game_run_music_scene;
        break;

    case SCENE_DIFFICULTY:
        game_run_scene = game_run_difficulty_scene;
        break;

    case SCENE_STORY:
        game_run_scene = game_run_story_scene;
        break;

    case SCENE_LEVEL:
        game_init_level_scene(game_level);
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
    /** TODO: move this stuff somewhere else */
    gun_fired = false;
    walk_sound_toggle = false;
    gun_pos = 0;
    rot_speed;
    old_dir_x;
    old_plane_x;
    view_height;
    jogging;
    fade_screen = GRADIENT_COUNT - 1;
    mc = false;

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
 * @return EntityType Entity type
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
 * @return bool Entity is spawned
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
 * @return bool Static entity is spawned
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

    case E_AMMO:
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

    // Remove if already exists, don't throw anything
    // Not the best, but shouldn't happen too often
    EntityUID uid = entities_get_uid(E_FIREBALL, (uint8_t)x, (uint8_t)y);
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
 * @brief GAME clear farthest dead enemy entity.
 *
 */
void game_clear_dead_enemy(void)
{
    uint8_t i = num_entities - 1;
    while (i >= 0)
    {
        EntityType type = entities_get_type(entity[i].uid);
        if ((type == E_ENEMY) && (entity[i].state == S_DEAD))
        {
            game_remove_entity(entity[i].uid);
            return;
        }
        i--;
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
 * @return EntityUID Entity UID number
 */
EntityUID game_detect_collision(const uint8_t level[], Coords *pos, float rel_x,
                                float rel_y, bool only_walls)
{
    // Wall collision
    uint8_t round_x = pos->x + rel_x;
    uint8_t round_y = pos->y + rel_y;
    uint8_t block = game_get_level_entity(level, round_x, round_y);

    if (block == E_WALL & debug == false)
    {
        sound_play(hit_wall_snd, HIT_WALL_SND_LEN);
        return entities_get_uid(block, round_x, round_y);
    }
    else if (block == E_DOOR && player.secret == false)
    {
        z = 8;
        player.secret = true;
        updateHud();
        sound_play(s_snd, S_SND_LEN);
    }
    else if (block == E_DOOR2 && player.secret2 == false)
    {
        z = 8;
        player.secret2 = true;
        updateHud();
        sound_play(s_snd, S_SND_LEN);
    }
    else if (block == E_DOOR3 && player.secret3 == false)
    {
        z = 8;
        player.secret3 = true;
        sound_play(s_snd, S_SND_LEN);
        updateHud();
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
        uint8_t distance = coords_get_distance(pos, &new_coords);

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
 * @return EntityUID Entity UID number
 */
EntityUID game_update_position(const uint8_t level[], Coords *pos, float rel_x,
                               float rel_y, bool only_walls)
{
    EntityUID collide_x =
        game_detect_collision(level, pos, rel_x, 0.0f, only_walls);
    EntityUID collide_y =
        game_detect_collision(level, pos, 0.0f, rel_y, only_walls);

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
    if (player.ammo != 0)
    {
        z = 3;
        sound_play(shoot_snd, SHOOT_SND_LEN);
        for (uint8_t i = 0; i < num_entities; i++)
        {
            // Shoot only ALIVE enemies
            if ((entities_get_type(entity[i].uid) != E_ENEMY) ||
                (entity[i].state == S_DEAD) ||
                (entity[i].state == S_HIDDEN))
                continue;

            Coords transform = game_translate_into_view(&(entity[i].pos));
            if ((fabsf(transform.x) < 20.0f) && (transform.y > 0))
            {
                uint8_t damage = MIN(
                    GUN_MAX_DAMAGE,
                    GUN_MAX_DAMAGE /
                        (fabsf(transform.x) * entity[i].distance) / 5.0f);

                if ((jump == 1) || (jump == 2))
                    damage = damage / 3;

                /** TODO: check if difficulty is easy->1, medium->2 etc..*/
                if (game_difficulty == 1)
                    damage *= 1.5f;
                else if (game_difficulty == 2)
                    damage = damage;
                else
                    damage *= 0.7f;

                if (damage > 0)
                {
                    /** TODO: change this equation so that game difficulty only
                     * appears in damage calculation
                     */
                    entity[i].health = MAX(
                        0, entity[i].health - damage / game_difficulty);
                    entity[i].state = S_HIT;
                    entity[i].timer = 2;
                }
            }
        }
    }
    else
    {
        z = 3;
        sound_play(melee_snd, MELEE_SND_LEN);
        for (uint8_t i = 0; i < num_entities; i++)
        {
            if (entity[i].distance <= ENEMY_MELEE_DIST)
            {
                // Shoot only ALIVE enemies
                if ((entities_get_type(entity[i].uid) != E_ENEMY) ||
                    (entity[i].state == S_DEAD) ||
                    (entity[i].state == S_HIDDEN))
                    continue;

                Coords transform = game_translate_into_view(&(entity[i].pos));
                if (fabsf(transform.x) < 20 && transform.y > 0)
                {
                    uint8_t damage = MIN(
                        GUN_MAX_DAMAGE,
                        GUN_MAX_DAMAGE /
                            (fabsf(transform.x) * entity[i].distance) / 5);

                    /** TODO: why is it different now? cut down duplication */
                    if ((jump == 1) || (jump == 2))
                        damage = damage / 3;
                    if (game_difficulty == 1)
                        damage = damage + damage / 4.0;
                    else if (game_difficulty == 2)
                        damage = damage - damage * 0.1;
                    else
                        damage = damage * 0.60;

                    if (damage > 0)
                    {
                        entity[i].health = MAX(0, entity[i].health - damage / game_difficulty);
                        entity[i].state = S_HIT;
                        entity[i].timer = 2;
                    }
                }
            }
        }
    }

    /** TODO: move all rendering to the end of the scene not in the middle */
    updateHud();
}

/**
 * @brief GAME get random item drop.
 *
 * @return EntityType Random item between ammo, medkit, or nothing
 */
EntityType game_get_item_drop(void)
{
    EntityType item = 0;

    uint8_t random_item = rand() % 4;
    if (random_item > 0)
    {
        if (random_item < 3)
            item = E_AMMO;
        else
            item = E_MEDKIT;
    }

    return item;
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
        // update distance
        entity[i].distance = coords_get_distance(&(player.pos), &(entity[i].pos));

        // Run the timer. Works with actual frames.
        // Todo: use delta_time here. But needs float type and more memory
        if (entity[i].timer > 0)
            entity[i].timer--;

        // too far away. put it in doze mode
        if (entity[i].distance > MAX_ENTITY_DISTANCE)
        {
            game_remove_entity(entity[i].uid);
            // don't increase 'i', since current one has been removed
            continue;
        }
        // bypass render if hidden
        if (entity[i].state == S_HIDDEN)
        {
            i++;
            continue;
        }

        uint8_t type = entities_get_type(entity[i].uid);

        switch (type)
        {
        /** TODO: move enemy AI to separate function */
        case E_ENEMY:
        {
            // Enemy "IA"
            if (entity[i].health == 0)
            {
                if (entity[i].state != S_DEAD)
                {
                    entity[i].state = S_DEAD;
                    entity[i].timer = 6;
                }

                /** TODO: what is .a property? */
                if (entity[i].a == false)
                {
                    EntityType item = game_get_item_drop();
                    game_spawn_entity(item, entity[i].pos.x, entity[i].pos.y);

                    entity[i].a = true;
                    enemy_count++;
                    z = 7;

                    if (bss == true)
                        enemy_count2++;
                }

                /** TODO: why is this here? */
                if (bss == true && enemy_count > 2)
                {
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
                        if (!debug)
                        {
                            uint8_t damage = ENEMY_MELEE_DAMAGE * game_difficulty;
                            player.health = MAX(0, player.health - damage);
                        }
                        entity[i].timer = 14;
                        flash_screen = true;
                        updateHud();
                    }
                }
                else
                    entity[i].state = S_STAND;
            }
            break;
        }

        case E_FIREBALL:
        {
            if (entity[i].distance < FIREBALL_COLLIDER_DIST)
            {
                // Hit the player and disappear
                if (!debug)
                {
                    uint8_t damage = ENEMY_MELEE_DAMAGE * game_difficulty;
                    player.health = MAX(0, player.health - damage);
                }
                flash_screen = true;
                game_remove_entity(entity[i].uid);
                updateHud();
                continue;
            }
            else
            {
                // Move. Only collide with walls.
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
            if ((entity[i].distance < ITEM_COLLIDER_DIST) &&
                (player.health != PLAYER_MAX_HEALTH) &&
                (jump_height < 14))
            {
                // Pickup
                sound_play(medkit_snd, MEDKIT_SND_LEN);
                entity[i].state = S_HIDDEN;

                uint16_t health;
                if (game_difficulty == 1)
                    health = player.health + MEDKIT_HEAL_HIGH;
                else if (game_difficulty == 2)
                    health = player.health + MEDKIT_HEAL_MEDIUM;
                else
                    health = player.health + MEDKIT_HEAL_LOW;

                player.health = MIN(PLAYER_MAX_HEALTH, health);
                flash_screen = true;

                /** TODO: why so many updates of the HUD? */
                updateHud();
                z = 3;
                updateHud();
                z = 2;
                updateHud();
            }
            break;
        }

        case E_AMMO:
        {
            if ((entity[i].distance < ITEM_COLLIDER_DIST) &&
                (player.ammo < PLAYER_MAX_AMMO) &&
                (jump_height < 14))
            {
                // pickup
                sound_play(get_key_snd, GET_KEY_SND_LEN);
                entity[i].state = S_HIDDEN;

                uint16_t ammo;
                if (game_difficulty == 1)
                    ammo = player.ammo + AMMO_NUM_HIGH;
                else if (game_difficulty == 2)
                    ammo = player.ammo + AMMO_NUM_MEDIUM;
                else
                    ammo = player.ammo + AMMO_NUM_LOW;

                player.ammo = MIN(PLAYER_MAX_AMMO, ammo);

                /** TODO: why so many updates of the HUD? */
                updateHud();
                z = 3;
                updateHud();
                z = 1;
                updateHud();
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
        float delta_x = fabsf(1.0f / ray_x);
        float delta_y = fabsf(1.0f / ray_y);

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
            side_x = (map_x + 1.0f - player.pos.x) * delta_x;
        }

        if (ray_y < 0)
        {
            step_y = -1;
            side_y = (player.pos.y - map_y) * delta_y;
        }
        else
        {
            step_y = 1;
            side_y = (map_y + 1.0f - player.pos.y) * delta_y;
        }

        // Wall detection
        uint8_t depth = 0;
        bool hit = false;
        bool side;
        bool coll = false;
        while ((!hit) && (depth < MAX_RENDER_DEPTH))
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

            if ((block == E_WALL) || (block == E_DOOR) || (block == E_DOOR2) ||
                (block == E_DOOR3) || (block == E_COLL))
            {
                hit = true;
                if (block == E_COLL)
                    coll = true;
            }
            else
            {
                // Spawning entities here, as soon they are visible for the
                // player. Not the best place, but would be a very performance
                // cost scan for them in another loop
                if ((block == E_ENEMY) || (block & 0b00001000))
                {
                    // Check that it's close to the player
                    if (coords_get_distance(&(player.pos), &map_coords) <
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

            if (side)
                distance =
                    MAX(1, (map_y - player.pos.y + (1 - step_y) / 2) / ray_y);
            else
                distance =
                    MAX(1, (map_x - player.pos.x + (1 - step_x) / 2) / ray_x);

            // Store zbuffer value for the column
            zbuffer[x / Z_RES_DIVIDER] =
                MIN(distance * DISTANCE_MULTIPLIER, 0xff);

            // Render vertical line
            uint8_t line_height = RENDER_HEIGHT / distance - 1;
            display_draw_vline(
                x,
                view_height / distance - line_height / 2 + RENDER_HEIGHT / 2 +
                    (-17 ? coll : 0),
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
 * @return Coords Camera coordinates
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

        case E_AMMO:
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
void game_render_gun(uint8_t pos, float jogging, bool fired, uint8_t reload)
{
    // Jogging
    int8_t x = 48 + sinf(platform_millis() * JOGGING_SPEED) * 10 * jogging - 9;
    int8_t y = RENDER_HEIGHT - pos +
               fabsf(cosf(platform_millis() * JOGGING_SPEED)) * 8 * jogging - 3;

    // Gun fire
    if ((pos > GUN_SHOT_POS - 2) && (player.ammo > 0) && (fired))
        display_draw_bitmap(x + 14, y - 11, bmp_fire_bits, BMP_FIRE_WIDTH,
                            BMP_FIRE_HEIGHT, 1);

    // Reload animation
    uint8_t clip_height;
    switch (reload)
    {
    case 1:
        clip_height = MAX(0, MIN(y + BMP_RE1_HEIGHT, RENDER_HEIGHT) - y + 22);
        display_draw_bitmap(x - 10, y - 22, bmp_re1_mask, BMP_RE1_WIDTH,
                            clip_height, 0);
        display_draw_bitmap(x - 10, y - 22, bmp_re1_bits, BMP_RE1_WIDTH,
                            clip_height, 1);
        break;

    case 2:
        clip_height = MAX(0, MIN(y + BMP_RE2_HEIGHT, RENDER_HEIGHT) - y + 22);
        display_draw_bitmap(x - 10, y - 22, bmp_re2_mask, BMP_RE2_WIDTH,
                            clip_height, 0);
        display_draw_bitmap(x - 10, y - 22, bmp_re2_bits, BMP_RE2_WIDTH,
                            clip_height, 1);

    default:
        clip_height = MAX(0, MIN(y + BMP_GUN_HEIGHT, RENDER_HEIGHT) - y);
        display_draw_bitmap(x, y, bmp_gun_mask, BMP_GUN_WIDTH, clip_height, 0);
        display_draw_bitmap(x, y, bmp_gun_bits, BMP_GUN_WIDTH, clip_height, 1);
        break;
    }
}

// Only needed first time
void game_render_hud(void)
{
    if (debug == false)
    {
        display_draw_text(2, 58, "{}", false);   // Health symbol
        display_draw_text(105, 58, "[]", false); // Keys symbol
        updateHud();
    }
    else
    {
        display_draw_text(2, 58, "X", false);   // Health symbol
        display_draw_text(105, 58, "Y", false); // Keys symbol
        updateHud();
    }
}

// Render values for the HUD
void updateHud(void)
{
    display_draw_rect(12, 58, 100, 6, false);
    display_draw_rect(50, 58, 15, 6, false);
    display_draw_rect(58, 58, 70, 6, false);

    if (z == 1)
    {
        display_draw_text(31, 58, "FOUND ", false);
        display_draw_text(65, 58, " SHELLS", false);
        if (game_difficulty == 1)
        {
            display_draw_text(57, 58, "13", false);
        }
        else if (game_difficulty == 2)
        {
            display_draw_text(57, 58, "10", false);
        }
        else
        {
            display_draw_text(60, 58, "9", false);
        }
    }

    else if (z == 2)
    {
        display_draw_text(31, 58, "FOUND A MEDKIT", false);
    }
    else if (z == 3)
    {
        display_draw_rect(12, 58, 100, 6, false);
        display_draw_rect(1, 58, 100, 6, false);
        display_draw_rect(50, 58, 15, 6, false);
        display_draw_rect(58, 58, 70, 6, false);
    }
    else if (z == 4)
    {
        display_draw_text(38, 58, "GAME OVER", false);
    }
    else if (z == 5)
    {
        display_draw_text(44, 58, "YOU WIN", false);
    }
    else if (z == 6)
    {
        display_draw_text(33, 58, "GOAL-20 KILLS", false);
    }
    else if (z == 7)
    {
        if (game_level == E1M2 && bss == true)
        {
            display_draw_int(37, 58, enemy_count2);
            display_draw_text(52, 58, "OUT OF ", false);
            display_draw_int(87, 58, enemy_goal2);
        }
        else if (game_level == E1M1)
        {
            display_draw_int(37, 58, enemy_count);
            display_draw_text(52, 58, "OUT OF ", false);
            display_draw_int(87, 58, enemy_goal);
        }
    }
    else if (z == 8)
    {
        display_draw_text(35, 58, "SECRET FOUND", false);
    }
    else if (z == 9)
    {
        display_draw_text(31, 58, "GOAL-FIND EXIT", false);
    }
    else if (z == 10)
    {
        display_draw_text(31, 58, "DEBUG MODE OFF", false);
    }
    else
    {
        display_draw_text(32, 58, "DEBUG MODE ON", false);
    }

    if (debug == false)
    {
        display_draw_rect(1, 58, 8, 6, false);
        display_draw_text(2, 58, "{}", false);
        display_draw_text(103, 58, "[]", false);
        display_draw_int(12, 58, player.health);
        display_draw_int(113, 58, player.ammo);
    }
    else
    {
        display_draw_rect(1, 58, 8, 6, false);
        display_draw_text(2, 58, "X", false);
        display_draw_text(105, 58, "Y", false);
        char posx[10];
        char posy[10];
        sprintf(posx, "%f", player.pos.x);
        sprintf(posy, "%f", player.pos.y);
        display_draw_text(12, 58, posx, false);
        display_draw_text(113, 58, posy, false);
    }
}

// Debug stats
void renderStats(void) {}

void softReset(void) {}

void game_run_story_scene(void)
{
    display_draw_rect(1, 1, 127, 63, false);

    if (mid == 1)
    {
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .14,
                          "YEAR 2027. HUMANS REACHED", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .23,
                          "OTHER PLANETS, BUT WE ARE", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .32,
                          "NOT ALONE, THERE IS ALSO", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .42,
                          "HOSTILE ALIENS HERE. YOU", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .51,
                          "ARE AN UNKNOWN MARINE,", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .60,
                          "WHO FIGHT IN OLD LAB FOR", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .70,
                          "REMNANTS OF EARTH. RESIST", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .80,
                          "ALIENS TO ESCAPE.", false);
    }
    else if (mid == 2)
    {
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .14,
                          "AFTER KILLING BUNCH OF ", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .23,
                          "ALIENS, LIGHTS TURNED OFF", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .32,
                          "AND THE FLOOR COLLAPSED", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .42,
                          "UNDER YOUR FEET AND YOU ", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .51,
                          "FELL INTO THE UTILITY", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .60,
                          "ROOMS. YOU HAVE NO CHOICE", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .70,
                          "BUT TO START LOOKING FOR ", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .80,
                          "EXIT, WHILE FIGHT ALIENS.", false);

        // Go to next level
        game_level = E1M2;
    }
    else
    {
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .14,
                          "AFTER HARD FIGHT YOU WENT", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .23,
                          "TO EXIT. AND AS SOON AS", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .32,
                          "YOU STEP OUT, AN ALIEN", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .42,
                          "ATTACKS YOU FROM BEHIND", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .51,
                          "AND KILLS YOU. YOU DIDNT", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .60,
                          "EXPECT THIS. YOUR FIGHT", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .70,
                          "CAN NOT END LIKE THIS...", false);
        display_draw_text(SCREEN_WIDTH / 4.6 - 26, SCREEN_HEIGHT * .80,
                          "THE END (MAYBE...)", false);
    }
    display_draw_text(SCREEN_WIDTH / 2.1 - 24, SCREEN_HEIGHT * .01, "THE STORY",
                      false);
    display_draw_text(SCREEN_WIDTH / 2 - 27, SCREEN_HEIGHT * .91, "PRESS FIRE",
                      false);

    if (input_fire())
    {
        fade_e = true;
        if (mid < 3)
        {
            game_jump_to_scene(SCENE_LEVEL);
        }
        else
        {
            game_jump_to_scene(SCENE_SCORE);
        }
    }
}

void game_run_score_scene(void)
{
    game_score = player.ammo / 2;
    game_score += player.health;
    game_score *= 43;
    game_score *= game_difficulty;

    if (player.secret != 0)
    {
        game_score += 69;
    }
    if (player.secret2 += 0)
    {
        game_score += 69;
    }
    if (player.secret3 += 0)
    {
        game_score += 69;
    }
    game_score += k;

    display_draw_rect(1, 1, 127, 63, false);

    display_draw_bitmap((SCREEN_WIDTH - BMP_LOGO_WIDTH) / 2 - 27,
                        (SCREEN_HEIGHT - BMP_LOGO_HEIGHT) / 6, bmp_logo_bits,
                        BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, 1);

    display_draw_text(SCREEN_WIDTH / 2.36 - 52, SCREEN_HEIGHT * .79,
                      "NANO BRUTALITY", false);
    display_draw_text(SCREEN_WIDTH / 0.99 - 45, SCREEN_HEIGHT * .2, "YOU WIN",
                      false);
    if (player.cheats == false)
    {
        display_draw_text(SCREEN_WIDTH / 0.99 - 40, SCREEN_HEIGHT * .4,
                          "SCENE_SCORE", false);
        if (a < game_score)
        {
            a += 155;
            display_draw_int(SCREEN_WIDTH / 0.99 - 40, SCREEN_HEIGHT * .5, a);
            sound_play(walk1_snd, WALK1_SND_LEN);
        }
        else if (a > game_score)
        {
            a = game_score;
            display_draw_int(SCREEN_WIDTH / 0.99 - 40, SCREEN_HEIGHT * .5, a);
            m = false;
            music = 1;
            sound_play(shot_snd, SHOT_SND_LEN);
        }
        else
        {
            display_draw_int(SCREEN_WIDTH / 0.99 - 40, SCREEN_HEIGHT * .5, a);
            display_draw_text(SCREEN_WIDTH / 0.99 - 52, SCREEN_HEIGHT * .91,
                              "PRESS FIRE", false);
            if (input_fire())
            {
                display_draw_rect(1, 1, 127, 63, false);

                music = 99;
                platform_delay(1000);
                fade_e = true;
                softReset();
            }
        }
    }
    else if (player.cheats == true)
    {
        display_draw_text(SCREEN_WIDTH / 0.99 - 49, SCREEN_HEIGHT * .4,
                          "NO SCENE_SCORE", false);
        display_draw_text(SCREEN_WIDTH / 0.99 - 37, SCREEN_HEIGHT * .5, "FOR",
                          false);
        display_draw_text(SCREEN_WIDTH / 0.99 - 49, SCREEN_HEIGHT * .6, "CHEATERS",
                          false);
        display_draw_text(SCREEN_WIDTH / 0.99 - 52, SCREEN_HEIGHT * .91,
                          "PRESS FIRE", false);
        if (input_fire())
        {
            display_draw_rect(1, 1, 127, 63, false);

            music = 99;
            platform_delay(1000);
            fade_e = true;
            softReset();
        }
        if (mc == false)
        {
            m = false;
            music = 1;
            sound_play(mus_s1_snd, MUS_S1_SND_LEN);
            platform_delay(100);
            mc = true;
        }
    }

    fade_e = false;
    game_jump_to_scene(SCENE_SCORE);
}

/**
 * @brief GAME run intro scene.
 *
 */
void game_run_intro_scene(void)
{
    display_draw_bitmap((SCREEN_WIDTH - BMP_LOGO_WIDTH) / 2,
                        (SCREEN_HEIGHT - BMP_LOGO_HEIGHT) / 3, bmp_logo_bits,
                        BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, true);

    display_draw_text(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT * 0.8f, "PRESS FIRE",
                      true);

    sound_play(mus_s1_snd, MUS_S1_SND_LEN);

    if (input_fire())
        game_jump_to_scene(SCENE_DIFFICULTY);
}

void game_run_difficulty_scene(void)
{
    platform_delay(200);

    display_draw_rect(1, 1, 127, 63, false);
    display_draw_text(SCREEN_WIDTH / 2.66 - 25, SCREEN_HEIGHT * .05,
                      "CHOOSE SKILL LEVEL", false);
    display_draw_text(SCREEN_WIDTH / 2.75 - 25, SCREEN_HEIGHT * .3, "I", false);
    display_draw_text(SCREEN_WIDTH / 2.4 - 25, SCREEN_HEIGHT * .3,
                      "M TOO YOUNG TO DIE.", false);
    display_draw_text(SCREEN_WIDTH / 2.6 - 25, SCREEN_HEIGHT * .26, ",", false);
    display_draw_text(SCREEN_WIDTH / 2.66 - 25, SCREEN_HEIGHT * .43,
                      "HURT ME PLENTY.", false);
    display_draw_text(SCREEN_WIDTH / 2.66 - 25, SCREEN_HEIGHT * .56, "NIGHTMARE.",
                      false);
    display_draw_text(SCREEN_WIDTH / 3.32 - 25, SCREEN_HEIGHT * .77,
                      "NOTE - BUTTONS OUTSIDE", false);
    display_draw_text(SCREEN_WIDTH / 4.1 - 25, SCREEN_HEIGHT * .9,
                      "GAMEPLAY WORK AS U THINK", false);

    if (game_difficulty == 2)
    {
        display_draw_text(SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * .43, "#", false);
    }
    else if (game_difficulty == 1)
    {
        display_draw_text(SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * .3, "#", false);
    }
    else
    {
        display_draw_text(SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * .56, "#", false);
    }

    if (input_down())
    {
        game_difficulty++;
        if (game_difficulty == 4)
        {
            game_difficulty = 1;
        }
        fade_e = false;
        game_jump_to_scene(SCENE_DIFFICULTY);
    }
    else if (input_up())
    {
        game_difficulty--;
        if (game_difficulty == 0)
        {
            game_difficulty = 3;
        }
        fade_e = false;
        game_jump_to_scene(SCENE_DIFFICULTY);
    }
    else if (input_fire())
    {
        fade_e = true;
        game_jump_to_scene(SCENE_MUSIC);
    }
}

void game_run_music_scene(void)
{
    fade_e = false;
    platform_delay(200);

    display_draw_rect(1, 1, 127, 63, false);
    display_draw_text(SCREEN_WIDTH / 2.75 - 25, SCREEN_HEIGHT * .25, "MUSIC",
                      false);
    display_draw_text(SCREEN_WIDTH / 2.66 - 25, SCREEN_HEIGHT * .39, "OFF",
                      false);
    display_draw_text(SCREEN_WIDTH / 2.66 - 25, SCREEN_HEIGHT * .50,
                      "ON (NOT RECOMENDED)", false);

    if (m == false)
    {
        display_draw_text(SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * .50, "#", false);
    }
    else if (m == true)
    {
        display_draw_text(SCREEN_WIDTH / 3 - 25, SCREEN_HEIGHT * .39, "#", false);
    }

    if (input_down() || input_up())
    {
        m = !m;
        game_jump_to_scene(SCENE_MUSIC);
    }
    else if (input_fire())
    {
        fade_e = true;
        game_jump_to_scene(SCENE_STORY);
    }
}

void game_run_level_scene(void)
{
    display_get_fps();

    if (player.ammo == 0)
        coll = false;
    else
        coll = true;

    if (game_level == E1M1)
    {
        k = player.ammo;
    }

    display_draw_rect(1, 58, 100, 2, false);

    updateHud();

    // Clear only the 3d view
    memset(display_buf, 0, SCREEN_WIDTH * (RENDER_HEIGHT / 8));

    if (player.pos.x >= 2 && player.pos.x <= 3 && player.pos.y >= 54 &&
        player.pos.y <= 55 && z == 1 && player.secret < 2)
    {
        game_spawn_entity(E_ENEMY, 1, 51);
        game_spawn_entity(E_ENEMY, 3, 51);
        player.secret++;
    }

    if (player.pos.x >= 46 && player.pos.x <= 47 && player.pos.y >= 35 &&
        player.pos.y <= 36 && game_level == E1M2)
    {
        player.pos.x = 12.5;
        player.pos.y = 33.5;
        enemy_count = 0;
        game_spawn_entity(E_ENEMY, 10, 38);
        game_spawn_entity(E_ENEMY, 13, 38);
        bss = true;
    }
    if (player.pos.y >= 55 && player.pos.y <= 56 && player.pos.x >= 12 &&
        player.pos.x <= 23 && game_level == E1M2)
    {
        mid = 3;
        m = false;
        sound_play(mus_s1_snd, MUS_S1_SND_LEN);
        game_jump_to_scene(SCENE_STORY);
    }
    if (game_level == E1M2 && bss == true)
    {
        if (enemy_count == 1 || enemy_count == 5 || enemy_count == 9)
        {
            game_clear_dead_enemy();
            enemy_count++;
            game_spawn_entity(E_ENEMY, 13, 38);
        }
        else if (enemy_count == 3 || enemy_count == 7 || enemy_count == 11)
        {
            game_clear_dead_enemy();
            enemy_count++;
            game_spawn_entity(E_ENEMY, 10, 38);
        }
        else if (enemy_count == 13)
        {
            player.pos.y = player.pos.y + 12;
            enemy_count = 0;
            enemy_count2 = 8;
            updateHud();
        }
    }

    if (m == true)
    {
        music = 99;
    }

    // If the player is alive
    if (player.health > 0)
    {
        if (jump == 1 || jump == 2)
        {
            if (jump_height > 0 && jump == 2)
            {
                view_height -= 4;
                jump_height -= 4;
            }
            else if (jump_height < 20 && jump == 1)
            {
                view_height += 4;
                jump_height += 4;
            }
            else if (jump_height == 20)
                jump = 2;
            else if (jump_height == 0)
                jump = 0;

            vel = 2;
        }
        if (jump == 0)
        {
            view_height =
                fabsf(sinf(platform_millis() * JOGGING_SPEED)) * 6 * jogging;
            vel = 1;
        }

        // Player speed
        if (input_up())
        {
            player.velocity += (MOV_SPEED - player.velocity) * .4;
            if (jump == 1 || jump == 2)
            {
                jogging = 0;
                gun_pos = 22;
            }
            else
            {
                jogging = fabsf(player.velocity) * GUN_SPEED * 2;
            }
        }
        else if (input_down())
        {
            jogging = fabsf(player.velocity) * GUN_SPEED * 2;
            player.velocity += (-MOV_SPEED - player.velocity) * .4;
            if (jump == 1 || jump == 2)
            {
                jogging = 0;
                gun_pos = 22;
            }
            else
            {
                jogging = fabsf(player.velocity) * GUN_SPEED * 2;
            }
        }
        else
        {
            if (jump == 1 || jump == 2)
            {
                jogging = 0;
                gun_pos = 22;
            }
            else
            {
                jogging = fabsf(player.velocity) * GUN_SPEED * 2;
            }
            player.velocity *= .5;
        }

        // Player rotation
        if (input_right())
        {
            rot_speed = ROT_SPEED * delta_time;
            old_dir_x = player.dir.x;
            player.dir.x =
                player.dir.x * cosf(-rot_speed) - player.dir.y * sinf(-rot_speed);
            player.dir.y =
                old_dir_x * sinf(-rot_speed) + player.dir.y * cosf(-rot_speed);
            old_plane_x = player.plane.x;
            player.plane.x =
                player.plane.x * cosf(-rot_speed) - player.plane.y * sinf(-rot_speed);
            player.plane.y =
                old_plane_x * sinf(-rot_speed) + player.plane.y * cosf(-rot_speed);
        }
        else if (input_left())
        {
            rot_speed = ROT_SPEED * delta_time;
            old_dir_x = player.dir.x;
            player.dir.x =
                player.dir.x * cosf(rot_speed) - player.dir.y * sinf(rot_speed);
            player.dir.y =
                old_dir_x * sinf(rot_speed) + player.dir.y * cosf(rot_speed);
            old_plane_x = player.plane.x;
            player.plane.x =
                player.plane.x * cosf(rot_speed) - player.plane.y * sinf(rot_speed);
            player.plane.y =
                old_plane_x * sinf(rot_speed) + player.plane.y * cosf(rot_speed);
        }

        if (input_left() && input_right() && jump == 0)
        {
            jump = 1;
            sound_play(jump_snd, JUMP_SND_LEN);
        }

        if (view_height > 2.95 && jump == 0)
        {
            if (sound == false)
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
        }
        // Update gun
        if (gun_pos > GUN_TARGET_POS)
        {
            // Right after fire
            gun_pos -= 2;
        }
        else if (gun_pos < GUN_TARGET_POS)
        {
            // Showing up
            gun_pos += 2;
        }
        else if (!gun_fired && input_fire() && player.ammo > 0 &&
                 reload1 == false)
        {
            // ready to fire and fire pressed
            gun_pos = GUN_SHOT_POS;
            gun_fired = true;
            game_fire_shootgun();
            if (debug == false)
            {
                player.ammo--;
            }
        }
        else if (gun_fired && !input_fire())
        {
            // Just fired and restored position
            gun_fired = false;
            reload1 = true;
        }
        else if (!gun_fired && input_fire() && player.ammo == 0 &&
                 reload1 == false)
        {
            gun_pos = GUN_SHOT_POS;
            gun_fired = true;
            game_fire_shootgun();
        }

        if (enemy_count == enemy_goal && game_level == E1M1)
        {
            z = 3;
            updateHud();
            z = 5;
            updateHud();
            if (del == 0)
            {
                platform_delay(200);
                del++;
            }
            if (input_fire())
            {
                player.pos.x = 230;
                player.pos.y = 50;
                mid = 2;
                enemy_count = 0;
                z = 3;
                updateHud();
                game_jump_to_scene(SCENE_STORY);
            }
        }

        game_update_position(game_level, &(player.pos),
                             player.dir.x * player.velocity * delta_time * vel,
                             player.dir.y * player.velocity * delta_time * vel,
                             false);

        game_update_entities(game_level);
    }
    else
    {
        // The player is dead
        z = 3;
        updateHud();
        z = 4;
        updateHud();

        if (view_height > -5)
            view_height--;

        else if (input_fire())
        {
            game_jump_to_scene(SCENE_INTRO);
        }
        if (gun_pos > 0)
            gun_pos -= 2;
        else
        {
            rc1 = 3;
        }
    }

    if (reload1 == true)
    {
        r++;
    }

    if (coll == false)
    {
        r = 7;
    }

    if (r == 1)
    {
        rc1 = 1;
    }
    else if (r == 3)
    {
        rc1 = 2;
        sound_play(r1_snd, R1_SND_LEN);
    }

    else if (r == 5)
    {
        rc1 = 1;
        sound_play(r2_snd, R2_SND_LEN);
    }
    else if (r == 7)
    {
        r = 0;
        reload1 = false;
        rc1 = 0;
    }

    // Render stuff
    updateHud();
    game_render_map(game_level ? game_level : E1M2, view_height);
    game_render_entities(view_height);
    game_render_gun(gun_pos, jogging, gun_fired, rc1);

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
        display_invert();
        flash_screen = false;
    }

    // Exit routine
    if (input_exit())
        game_jump_to_scene(SCENE_INTRO);

    // Exit routine
    // if (input_left() && input_right() && input_up() && input_down() &&
    // input_fire())
    // {
    //     z = 3;
    //     updateHud();
    //     if (debug == true)
    //     {
    //         z = 3;
    //         updateHud();
    //         z = 10;
    //         debug = false;
    //         updateHud();
    //     }
    //     else
    //     {
    //         z = 3;
    //         updateHud();
    //         z = 11;
    //         debug = true;
    //         updateHud();
    //         player.cheats = true;
    //     }
    //     updateHud();
    //     platform_delay(500);
    // }
}

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

        printf("vel: %f\thp: %d\tammo: %d\ts: %d\ts1: %d\ts2: %d\tscore: %d\tcheats: %d\n",
               player.velocity,
               player.health,
               player.ammo,
               player.secret,
               player.secret2,
               player.secret3,
               player.score,
               player.cheats);

        /* Stop drawing */
        display_draw_stop();
    }
}
