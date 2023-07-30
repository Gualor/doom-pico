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

typedef enum
{
    TEXT_BLANK_SPACE,
    TEXT_FOUND_AMMO,
    TEXT_FOUND_MEDKIT,
    TEXT_FOUND_SECRET,
    TEXT_GOAL_KILLS,
    TEXT_GOAL_FIND_EXIT,
    TEXT_GAME_OVER,
    TEXT_YOU_WIN
} GameText;

typedef enum
{
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_VERY_HARD,
} GameDifficulty;

typedef enum
{
    CUTSCENE_E1M1,
    CUTSCENE_E1M2,
    CUTSCENE_END
} GameCutscene;

/* Function prototypes ------------------------------------------------------ */

/* Level */
static void game_init_level_scene(const uint8_t level[]);

/* Entities */
static EntityType game_get_level_entity(const uint8_t level[], int16_t x,
                                        int16_t y);
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
static void game_melee_attack(void);
EntityType game_get_item_drop(void);

/* Graphics */
static Coords game_translate_into_view(Coords *pos);
static void game_render_map(const uint8_t level[], float view_height);
static void game_render_entities(float view_height);
static void game_render_gun(uint8_t pos, float jogging, bool fired,
                            uint8_t reload);
static void game_render_hud(void);
static void game_render_hud_text(void);

/* Scenes */
static void game_jump_to_scene(GameScene scene);
static void game_run_intro_scene(void);
static void game_run_difficulty_scene(void);
static void game_run_music_scene(void);
static void game_run_story_scene(void);
static void game_run_level_scene(void);
static void game_run_score_scene(void);

/* Global variables --------------------------------------------------------- */

static uint8_t z = 6;
static bool coll = false;
static uint8_t jump_state = 0;
static uint8_t jump_height = 0;
static uint8_t vel = 1;
static uint8_t noclip = 0;
static bool enable_music = true;
static uint8_t rc1 = 0;
static int16_t a = 0;
static uint8_t enemy_count2 = 0;
static uint8_t enemy_goal2 = E1M2_ENEMY_GOAL;
// game
// player and entities
static Player player;
static Entity entity[MAX_ENTITIES];
static uint8_t num_entities = 0;
static StaticEntity static_entity[MAX_STATIC_ENTITIES];
static uint8_t num_static_entities = 0;

static uint8_t del = 0;
static uint8_t enemy_count = 0;
static uint8_t enemy_goal = E1M1_ENEMY_GOAL;
static uint8_t r = 0;
static bool reload1 = false;
static int16_t game_score;
static bool bss = false;
static bool mc = false;
// init
static bool gun_fired = false;
static bool walk_sound_toggle = false;
static uint8_t gun_pos = 0;
static float view_height;
static float jogging;

static bool flash_screen = false;
static uint8_t fade_screen = GRADIENT_COUNT - 1;

static uint8_t enemy_melee_damage = ENEMY_MELEE_DAMAGE_LOW;
static uint8_t enemy_fireball_damage = ENEMY_FIREBALL_DAMAGE_LOW;
static uint8_t player_max_damage = GUN_MAX_DAMAGE_HIGH;
static uint8_t medkit_heal_value = MEDKIT_HEAL_HIGH;
static uint8_t ammo_pickup_value = AMMO_PICKUP_HIGH;
static GameCutscene game_cutscene = CUTSCENE_E1M1;
static GameDifficulty game_difficulty = DIFFICULTY_EASY;
static GameText game_hud_text = TEXT_GOAL_KILLS;
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
    view_height = 0.0f;
    jogging = 0.0f;
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
EntityType game_get_level_entity(const uint8_t level[], int16_t x, int16_t y)
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
    int16_t dir = ((atan2f(y - player.pos.y, x - player.pos.x) / PI) + 1) *
                  FIREBALL_ANGLES;

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
        if ((!found) && (static_entity[i].uid == uid))
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
EntityUID game_detect_collision(const uint8_t level[], Coords *pos,
                                float rel_x, float rel_y, bool only_walls)
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
    else if ((block == E_DOOR) && (player.secret == false))
    {
        player.secret = true;
        game_hud_text = TEXT_FOUND_SECRET;
        sound_play(s_snd, S_SND_LEN);
    }
    else if ((block == E_DOOR2) && (player.secret2 == false))
    {
        player.secret2 = true;
        game_hud_text = TEXT_FOUND_SECRET;
        sound_play(s_snd, S_SND_LEN);
    }
    else if ((block == E_DOOR3) && (player.secret3 == false))
    {
        player.secret3 = true;
        game_hud_text = TEXT_FOUND_SECRET;
        sound_play(s_snd, S_SND_LEN);
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
            // Damage decrease with distance
            uint8_t damage = MIN(
                player_max_damage,
                (player_max_damage /
                 (fabsf(transform.x) * entity[i].distance) / 5.0f));

            entity[i].health = MAX(0, entity[i].health - damage);
            entity[i].state = S_HIT;
            entity[i].timer = 2;
        }
    }
}

/**
 * @brief GAME player perform melee attack and compute damage.
 *
 */
void game_melee_attack(void)
{
    sound_play(melee_snd, MELEE_SND_LEN);
    for (uint8_t i = 0; i < num_entities; i++)
    {
        if (entity[i].distance <= ENEMY_MELEE_DIST)
        {
            // Attack only ALIVE enemies
            if ((entities_get_type(entity[i].uid) != E_ENEMY) ||
                (entity[i].state == S_DEAD) ||
                (entity[i].state == S_HIDDEN))
                continue;

            Coords transform = game_translate_into_view(&(entity[i].pos));
            if ((fabsf(transform.x) < 20.0f) && (transform.y > 0.0f))
            {
                // Damage decrease with distance
                uint8_t damage = MIN(
                    player_max_damage,
                    (player_max_damage /
                     (fabsf(transform.x) * entity[i].distance) / 5.0f));

                entity[i].health = MAX(0, entity[i].health - damage);
                entity[i].state = S_HIT;
                entity[i].timer = 2;
            }
        }
    }
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
        entity[i].distance =
            coords_get_distance(&(player.pos), &(entity[i].pos));

        // Run the timer. Works with actual frames.
        // TODO: use delta_time here. But needs float type and more memory
        if (entity[i].timer > 0)
            entity[i].timer--;

        // too far away. put it in doze mode
        if (entity[i].distance > MAX_ENTITY_DISTANCE)
        {
            game_remove_entity(entity[i].uid);
            // Don't increase 'i', since current one has been removed
            continue;
        }
        // bypass render if hidden
        if (entity[i].state == S_HIDDEN)
        {
            i++;
            continue;
        }

        switch (entities_get_type(entity[i].uid))
        {
        /** TODO: move enemy AI to separate function */
        case E_ENEMY:
        {
            // Enemy "IA"
            if (entity[i].health == 0)
            {
                if (entity[i].state != S_DEAD)
                {
                    game_hud_text = TEXT_GOAL_KILLS;
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
                                (SIGN(player.pos.x, entity[i].pos.x) *
                                 ENEMY_SPEED * delta_time),
                                (SIGN(player.pos.y, entity[i].pos.y) *
                                 ENEMY_SPEED * delta_time),
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
                            MAX(0, player.health - enemy_melee_damage);
                        entity[i].timer = 14;
                        flash_screen = true;
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
                player.health = MAX(0, player.health - enemy_fireball_damage);
                flash_screen = true;
                game_remove_entity(entity[i].uid);
                continue;
            }
            else
            {
                // Move. Only collide with walls.
                // Note: using health to store the angle of the movement
                EntityUID collided = game_update_position(
                    level,
                    &(entity[i].pos),
                    (cosf(entity[i].health / FIREBALL_ANGLES * PI) *
                     FIREBALL_SPEED),
                    (sinf(entity[i].health / FIREBALL_ANGLES * PI) *
                     FIREBALL_SPEED),
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

                player.health =
                    MIN(PLAYER_MAX_HEALTH, player.health + medkit_heal_value);
                flash_screen = true;
                game_hud_text = TEXT_FOUND_MEDKIT;
            }
            break;
        }

        case E_AMMO:
        {
            if ((entity[i].distance < ITEM_COLLIDER_DIST) &&
                (player.ammo < PLAYER_MAX_AMMO) &&
                (jump_height < 14))
            {
                // Pickup
                sound_play(get_key_snd, GET_KEY_SND_LEN);
                entity[i].state = S_HIDDEN;
                player.ammo =
                    MIN(PLAYER_MAX_AMMO, player.ammo + ammo_pickup_value);
                game_hud_text = TEXT_FOUND_AMMO;
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
                sprite = (millis() / 500) % 2; // Walking
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
    int8_t x = 48 + sinf(millis() * JOGGING_SPEED) * 10 * jogging - 9;
    int8_t y = RENDER_HEIGHT - pos +
               fabsf(cosf(millis() * JOGGING_SPEED)) * 8 * jogging - 3;

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

/**
 * @brief GAME render heads-up display (HUD).
 *
 */
void game_render_hud(void)
{
    display_draw_text(2, 58, "{}", false);
    display_draw_text(103, 58, "[]", false);
    display_draw_int(12, 58, player.health);
    display_draw_int(113, 58, player.ammo);
}

/**
 * @brief GAME render text on heads-up display (HUD).
 *
 */
void game_render_hud_text(void)
{
    char text[32];

    switch (game_hud_text)
    {
    case TEXT_BLANK_SPACE:
        break;

    case TEXT_FOUND_AMMO:
        sprintf(text, "FOUND %d AMMO", ammo_pickup_value);
        display_draw_text(33, 58, text, TEXT_SPACING);
        break;

    case TEXT_FOUND_MEDKIT:
        display_draw_text(33, 58, "FOUND MEDKIT", TEXT_SPACING);
        break;

    case TEXT_FOUND_SECRET:
        display_draw_text(33, 58, "FOUND SECRET", TEXT_SPACING);

    case TEXT_GOAL_KILLS:
        if (game_level == E1M1)
            sprintf(text, "%d OUT OF %d", enemy_count, enemy_goal);
        else if ((game_level == E1M2) && (bss == true))
            sprintf(text, "%d OUT OF %d", enemy_count2, enemy_goal2);
        display_draw_text(35, 58, text, TEXT_SPACING);
        break;

    case TEXT_GOAL_FIND_EXIT:
        display_draw_text(33, 58, "FIND THE EXIT", TEXT_SPACING);
        break;

    case TEXT_GAME_OVER:
        display_draw_text(38, 58, "GAME OVER", TEXT_SPACING);
        break;

    case TEXT_YOU_WIN:
        display_draw_text(44, 58, "YOU WIN", TEXT_SPACING);
        break;

    default:
        break;
    }
}

/**
 * @brief GAME run story scene.
 *
 */
void game_run_story_scene(void)
{
    if (game_cutscene == CUTSCENE_E1M1)
    {
        display_draw_text(0, 0, "YEAR 2027. HUMANS REACHED", TEXT_SPACING);
        display_draw_text(0, 6, "OTHER PLANETS, BUT WE ARE", TEXT_SPACING);
        display_draw_text(0, 12, "NOT ALONE, THERE IS ALSO", TEXT_SPACING);
        display_draw_text(0, 18, "HOSTILE ALIENS HERE. YOU", TEXT_SPACING);
        display_draw_text(0, 24, "ARE AN UNKNOWN MARINE,", TEXT_SPACING);
        display_draw_text(0, 30, "WHO FIGHT IN OLD LAB FOR", TEXT_SPACING);
        display_draw_text(0, 36, "REMNANTS OF EARTH. RESIST", TEXT_SPACING);
        display_draw_text(0, 42, "ALIENS TO ESCAPE.", TEXT_SPACING);
    }
    else if (game_cutscene == CUTSCENE_E1M2)
    {
        display_draw_text(0, 0, "AFTER KILLING BUNCH OF ", TEXT_SPACING);
        display_draw_text(0, 6, "ALIENS, LIGHTS TURNED OFF", TEXT_SPACING);
        display_draw_text(0, 12, "AND THE FLOOR COLLAPSED", TEXT_SPACING);
        display_draw_text(0, 18, "UNDER YOUR FEET AND YOU ", TEXT_SPACING);
        display_draw_text(0, 24, "FELL INTO THE UTILITY", TEXT_SPACING);
        display_draw_text(0, 30, "ROOMS. YOU HAVE NO CHOICE", TEXT_SPACING);
        display_draw_text(0, 36, "BUT TO START LOOKING FOR ", TEXT_SPACING);
        display_draw_text(0, 42, "EXIT, WHILE FIGHT ALIENS.", TEXT_SPACING);
    }
    else if (game_cutscene == CUTSCENE_END)
    {
        display_draw_text(0, 0, "AFTER HARD FIGHT YOU WENT", TEXT_SPACING);
        display_draw_text(0, 6, "TO EXIT. AND AS SOON AS", TEXT_SPACING);
        display_draw_text(0, 12, "YOU STEP OUT, AN ALIEN", TEXT_SPACING);
        display_draw_text(0, 18, "ATTACKS YOU FROM BEHIND", TEXT_SPACING);
        display_draw_text(0, 24, "AND KILLS YOU. YOU DIDNT", TEXT_SPACING);
        display_draw_text(0, 30, "EXPECT THIS. YOUR FIGHT", TEXT_SPACING);
        display_draw_text(0, 36, "CAN NOT END LIKE THIS...", TEXT_SPACING);
        display_draw_text(0, 42, "THE END (MAYBE...)", TEXT_SPACING);
    }
    display_draw_text(39, 53, "PRESS FIRE", TEXT_SPACING);

    if (input_fire())
    {
        if (game_cutscene != CUTSCENE_END)
            game_jump_to_scene(SCENE_LEVEL);
        else
            game_jump_to_scene(SCENE_SCORE);
    }

    delay(100);
}

/**
 * @brief GAME run game over scene.
 *
 */
void game_run_score_scene(void)
{
    // Compute game score
    game_score = (player.ammo / 2);
    game_score += player.health;
    game_score *= (game_difficulty + 1);
    if (player.secret > 0)
        game_score += 100;
    if (player.secret2 > 0)
        game_score += 100;
    if (player.secret3 > 0)
        game_score += 100;

    display_draw_bitmap(
        8, 8, bmp_logo_bits, BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT, true);
    display_draw_text(34, 48, "PICO", TEXT_SPACING);
    display_draw_text(88, 8, "YOU WIN", TEXT_SPACING);
    display_draw_rect(88, 26, 34, 1, true);
    display_draw_text(88, 38, "SCORE:", TEXT_SPACING);
    display_draw_int(88, 48, game_score);

    if (game_score > SCORE_SECRET_ENDING)
        sound_play(walk1_snd, WALK1_SND_LEN);
    else
        sound_play(shot_snd, SHOT_SND_LEN);

    if (input_fire())
        game_jump_to_scene(SCENE_INTRO);

    delay(100);
}

/**
 * @brief GAME run intro scene.
 *
 */
void game_run_intro_scene(void)
{
    sound_play(mus_s1_snd, MUS_S1_SND_LEN);

    display_draw_bitmap((SCREEN_WIDTH - BMP_LOGO_WIDTH) / 2,
                        (SCREEN_HEIGHT - BMP_LOGO_HEIGHT) / 3,
                        bmp_logo_bits,
                        BMP_LOGO_WIDTH, BMP_LOGO_HEIGHT,
                        true);

    display_draw_text(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT * 0.8f, "PRESS FIRE",
                      TEXT_SPACING);

    if (input_fire())
        game_jump_to_scene(SCENE_DIFFICULTY);

    delay(100);
}

/**
 * @brief GAME run difficulty selection scene.
 *
 */
void game_run_difficulty_scene(void)
{
    display_draw_text(7, 5, "CHOOSE YOUR SKILL LEVEL", TEXT_SPACING);
    display_draw_text(16, 20, "I M TOO YOUNG TO DIE", TEXT_SPACING);
    display_draw_text(20, 17, ",", TEXT_SPACING);
    display_draw_text(18, 30, "HURT ME PLENTY", TEXT_SPACING);
    display_draw_text(18, 40, "ULTRA VIOLENCE", TEXT_SPACING);
    display_draw_text(18, 50, "NIGHTMARE", TEXT_SPACING);
    display_draw_text(7, game_difficulty * 10 + 20, "#", TEXT_SPACING);

    if (input_down())
    {
        if (game_difficulty == DIFFICULTY_VERY_HARD)
            game_difficulty = DIFFICULTY_EASY;
        else
            game_difficulty++;
    }
    else if (input_up())
    {
        if (game_difficulty == DIFFICULTY_EASY)
            game_difficulty = DIFFICULTY_VERY_HARD;
        else
            game_difficulty--;
    }
    else if (input_fire())
    {
        // Adjust game settings based on difficulty
        switch (game_difficulty)
        {
        case DIFFICULTY_EASY:
            medkit_heal_value = MEDKIT_HEAL_HIGH;
            ammo_pickup_value = AMMO_PICKUP_HIGH;
            enemy_melee_damage = ENEMY_MELEE_DAMAGE_LOW;
            player_max_damage = GUN_MAX_DAMAGE_HIGH;
            break;

        case DIFFICULTY_NORMAL:
            medkit_heal_value = MEDKIT_HEAL_MED;
            ammo_pickup_value = AMMO_PICKUP_MED;
            enemy_melee_damage = ENEMY_MELEE_DAMAGE_MED;
            player_max_damage = GUN_MAX_DAMAGE_MED;
            break;

        case DIFFICULTY_HARD:
            medkit_heal_value = MEDKIT_HEAL_LOW;
            ammo_pickup_value = AMMO_PICKUP_LOW;
            enemy_melee_damage = ENEMY_MELEE_DAMAGE_HIGH;
            player_max_damage = GUN_MAX_DAMAGE_LOW;
            break;

        /** TODO: increase difficulty for very hard mode */
        case DIFFICULTY_VERY_HARD:
            medkit_heal_value = MEDKIT_HEAL_LOW;
            ammo_pickup_value = AMMO_PICKUP_LOW;
            enemy_melee_damage = ENEMY_MELEE_DAMAGE_HIGH;
            player_max_damage = GUN_MAX_DAMAGE_LOW;
            break;
        }
        game_jump_to_scene(SCENE_MUSIC);
    }

    delay(100);
}

/**
 * @brief GAME run music settings scene.
 *
 */
void game_run_music_scene(void)
{
    display_draw_text(7, 5, "MUSIC SETTINGS", TEXT_SPACING);
    display_draw_text(18, 20, "ENABLE", TEXT_SPACING);
    display_draw_text(18, 30, "DISABLE", TEXT_SPACING);
    display_draw_text(7, enable_music * 10 + 20, "#", TEXT_SPACING);

    if (input_down() || input_up())
    {
        enable_music = !enable_music;
        game_jump_to_scene(SCENE_MUSIC);
    }
    else if (input_fire())
        game_jump_to_scene(SCENE_STORY);

    delay(100);
}

/**
 * @brief GAME run level scene.
 *
 */
void game_run_level_scene(void)
{
    bool up_pressed = input_up();
    bool down_pressed = input_down();
    bool left_pressed = input_left();
    bool right_pressed = input_right();
    bool fire_pressed = input_fire();
    bool jump_pressed = input_jump();

    display_get_fps();

    if (player.ammo == 0)
        coll = false;
    else
        coll = true;

    if ((player.pos.x >= 2.0f) && (player.pos.x <= 3.0f) &&
        (player.pos.y >= 54.0f) && (player.pos.y <= 55.0f) &&
        (player.secret < 2.0f))
    {
        game_spawn_entity(E_ENEMY, 1, 51);
        game_spawn_entity(E_ENEMY, 3, 51);
        player.secret++;
    }

    if ((player.pos.x >= 46.0f) && (player.pos.x <= 47.0f) &&
        (player.pos.y >= 35.0f) && (player.pos.y <= 36.0f) &&
        (game_level == E1M2))
    {
        player.pos.x = 12.5;
        player.pos.y = 33.5;
        enemy_count = 0;
        game_spawn_entity(E_ENEMY, 10, 38);
        game_spawn_entity(E_ENEMY, 13, 38);
        bss = true;
    }
    if ((player.pos.y >= 55.0f) && (player.pos.y <= 56.0f) &&
        (player.pos.x >= 12.0f) && (player.pos.x <= 23.0f) &&
        (game_level == E1M2))
    {
        game_cutscene = CUTSCENE_END;
        enable_music = false;
        sound_play(mus_s1_snd, MUS_S1_SND_LEN);
        game_jump_to_scene(SCENE_STORY);
    }
    if ((game_level == E1M2) && (bss))
    {
        if ((enemy_count == 1) || (enemy_count == 5) || (enemy_count == 9))
        {
            game_clear_dead_enemy();
            enemy_count++;
            game_spawn_entity(E_ENEMY, 13, 38);
        }
        else if ((enemy_count == 3) || (enemy_count == 7) ||
                 (enemy_count == 11))
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
        }
    }

    // If the player is alive
    if (player.health > 0)
    {
        // Player speed
        if (up_pressed || down_pressed)
        {
            if (up_pressed)
                player.velocity += (MOV_SPEED - player.velocity) * 0.4f;
            else
                player.velocity += (-MOV_SPEED - player.velocity) * 0.4f;
            jogging = fabsf(player.velocity) * GUN_SPEED * 2.0f;
        }
        else
            player.velocity *= 0.5f;

        // Player rotation
        if (left_pressed || right_pressed)
        {
            float old_dir_x = player.dir.x;
            float old_plane_x = player.plane.x;
            float rot_speed = ROT_SPEED * delta_time;

            if (left_pressed)
            {
                player.dir.x = (player.dir.x * cosf(rot_speed)) -
                               (player.dir.y * sinf(rot_speed));
                player.dir.y = (old_dir_x * sinf(rot_speed)) +
                               (player.dir.y * cosf(rot_speed));
                player.plane.x = (player.plane.x * cosf(rot_speed)) -
                                 (player.plane.y * sinf(rot_speed));
                player.plane.y = (old_plane_x * sinf(rot_speed)) +
                                 (player.plane.y * cosf(rot_speed));
            }
            else
            {
                player.dir.x = (player.dir.x * cosf(-rot_speed)) -
                               (player.dir.y * sinf(-rot_speed));
                player.dir.y = (old_dir_x * sinf(-rot_speed)) +
                               (player.dir.y * cosf(-rot_speed));
                player.plane.x = (player.plane.x * cosf(-rot_speed)) -
                                 (player.plane.y * sinf(-rot_speed));
                player.plane.y = (old_plane_x * sinf(-rot_speed)) +
                                 (player.plane.y * cosf(-rot_speed));
            }
        }

        // Player jump
        if (jump_state)
        {
            if ((jump_height > 0) && (jump_state == 2))
            {
                view_height -= 4;
                jump_height -= 4;
            }
            else if ((jump_height < 20) && (jump_state == 1))
            {
                view_height += 4;
                jump_height += 4;
            }
            else if (jump_height == 20)
                jump_state = 2;
            else if (jump_height == 0)
                jump_state = 0;

            vel = 2;
        }
        else
        {
            view_height = fabsf(sinf(millis() * JOGGING_SPEED)) * 6 * jogging;
            vel = 1;

            if (jump_pressed)
            {
                jump_state = 1;
                jogging = 0.0f;
                gun_pos = 22;
                sound_play(jump_snd, JUMP_SND_LEN);
            }
        }

        // Player walking sound
        if ((view_height > 2.95f) && (jump_state == 0))
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
        bool press_fire = input_fire();
        if (gun_pos > GUN_TARGET_POS)
            gun_pos -= 2; // Right after fire
        else if (gun_pos < GUN_TARGET_POS)
            gun_pos += 2; // Showing up
        else if (press_fire && !gun_fired && !reload1)
        {
            // Ready to fire and fire pressed
            gun_pos = GUN_SHOT_POS;
            gun_fired = true;
            if (player.ammo > 0)
            {
                player.ammo--;
                game_fire_shootgun();
            }
            else
                game_melee_attack();

            // Clear last HUD text after shooting / melee attack
            game_hud_text = TEXT_BLANK_SPACE;
        }
        else if (!press_fire && gun_fired)
        {
            // Just fired and restored position
            gun_fired = false;
            reload1 = true;
        }

        if ((enemy_count == enemy_goal) && (game_level == E1M1))
        {
            game_hud_text = TEXT_YOU_WIN;
            if (press_fire)
            {
                game_hud_text = TEXT_BLANK_SPACE;
                player.pos.x = 230;
                player.pos.y = 50;
                enemy_count = 0;
                game_cutscene = CUTSCENE_E1M2;
                game_level = E1M2;
            }
        }

        game_update_position(
            game_level,
            &(player.pos),
            player.dir.x * player.velocity * delta_time * vel,
            player.dir.y * player.velocity * delta_time * vel,
            false);

        game_update_entities(game_level);
    }
    else
    {
        // The player is dead
        game_hud_text = TEXT_GAME_OVER;

        if (view_height > -5.0f)
            view_height--;
        else if (fire_pressed)
            game_jump_to_scene(SCENE_INTRO);

        if (gun_pos > 0)
            gun_pos -= 2;
        else
            rc1 = 3;
    }

    /** TODO: what is r, rc1? */
    if (reload1)
        r++;

    if (!coll)
        r = 7;

    if (r == 1)
        rc1 = 1;
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
        rc1 = 0;
        reload1 = false;
    }

    // Render stuff
    game_render_map(game_level, view_height);
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
    game_render_hud_text();

    // Flash screen
    if (flash_screen)
    {
        display_invert();
        flash_screen = false;
    }

    // Exit routine
    if (input_home())
        game_jump_to_scene(SCENE_INTRO);
}

void main(void)
{
    /* Initialize game */
    platform_init();
    display_init();
    sound_init();
    input_init();
    game_run_scene = game_run_intro_scene;

    /** TODO: fix bug with running animation */

    while (!input_exit())
    {
        /* Start drawing */
        display_draw_start();

        /* Read user inputs */
        input_update();

        /* Run current game scene */
        game_run_scene();

        printf("vel: %f\thp: %d\tammo: %d\ts: %d\ts1: %d\ts2: %d\tscore: %d\n",
               player.velocity,
               player.health,
               player.ammo,
               player.secret,
               player.secret2,
               player.secret3,
               player.score);

        /* Stop drawing */
        display_draw_stop();
    }
}
