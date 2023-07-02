/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "constants.h"
#include "display.h"
#include "entities.h"
#include "input.h"
#include "level.h"
#include "sound.h"
#include "sprites.h"
#include "coords.h"

#include "platform.h"
#include "utils.h"

/** TODO: Remove raylib dependencies from here */
#include "raylib.h"

/* Function prototypes ------------------------------------------------------ */

/* Level */
void game_level_init(const uint8_t level[]);
uint8_t game_level_get_block(const uint8_t level[], uint8_t x, uint8_t y);

/* Entities */
bool game_is_entity_spawned(UID uid);
bool game_is_static_entity_spawned(UID uid);
void game_spawn_entity(uint8_t type, uint8_t x, uint8_t y);
void game_spawn_fireball(float x, float y);
void game_remove_entity(UID uid);
void game_remove_static_entity(UID uid);
void game_update_entities(const uint8_t level[]);
uint8_t game_sort_entities(void);

/* Game mechanics */
UID game_detect_collision(const uint8_t level[], Coords *pos, float rel_x,
						  float rel_y, bool only_walls);
UID game_update_position(const uint8_t level[], Coords *pos, float rel_x,
						 float rel_y, bool only_walls);
Coords game_translate_into_view(Coords *pos);
void game_fire_shootgun(void);

/* Graphics */
void game_render_map(const uint8_t level[], float view_height);
void game_render_entities(float view_height);
void game_render_gun(uint8_t gun_pos, float amount_jogging);
void game_render_hud_symbols(void);
void game_render_hud(void);
void game_render_stats(void);

/* Scenes */
void game_jump_to_scene(uint8_t target_scene);
void game_run_intro(void);
void game_run_level(void);
void game_run_exit(void);

/* Global variables --------------------------------------------------------- */

/* Graphics */
static bool invert_screen = false;
static uint8_t flash_screen = 0;

/* Entities */
static Player player;
static Entity entity[MAX_ENTITIES];
static StaticEntity static_entity[MAX_STATIC_ENTITIES];
static uint8_t num_entities = 0;
static uint8_t num_static_entities = 0;

/* Gameplay */
static bool gun_fired = false;
static bool walkSoundToggle = false;
static uint8_t gun_pos = 0;
static float rot_speed;
static float old_dir_x;
static float old_plane_x;
static float view_height;
static float jogging;
static uint8_t fade = 0; // GRADIENT_COUNT - 1;

/* Scenes */
static void (*game_run_scene)(void) = game_run_intro;

/* Function definitions ----------------------------------------------------- */

// Jump to another scene
void game_jump_to_scene(uint8_t target_scene)
{
	switch (target_scene)
	{
	case GAME_INTRO:
		game_run_scene = game_run_intro;
		break;

	case GAME_PLAY:
		game_level_init(level_1);
		game_run_scene = game_run_level;
		break;

	case GAME_EXIT:
		game_run_scene = game_run_exit;
		break;

	default:
		game_run_scene = game_run_intro;
		break;
	}
}

// Finds the player in the map
void game_level_init(const uint8_t level[])
{
	gun_fired = false;
	walkSoundToggle = false;
	gun_pos = 0;
	rot_speed;
	old_dir_x;
	old_plane_x;
	view_height;
	jogging;
	fade = GRADIENT_COUNT - 1;

	for (uint8_t y = LEVEL_HEIGHT - 1; y >= 0; y--)
	{
		for (uint8_t x = 0; x < LEVEL_WIDTH; x++)
		{
			uint8_t block = game_level_get_block(level, x, y);

			if (block == E_PLAYER)
			{
				player = entities_create_player(x, y);
				return;
			}

			/** TODO: Create other static entities */
		}
	}
}

uint8_t game_level_get_block(const uint8_t level[], uint8_t x, uint8_t y)
{
	if ((x < 0) || (x >= LEVEL_WIDTH) || (y < 0) || (y >= LEVEL_HEIGHT))
		return E_FLOOR;

	// y is read in inverse order
	return level[(((LEVEL_HEIGHT - 1 - y) * LEVEL_WIDTH + x) / 2)] >>
			   (!(x % 2) * 4) // displace part of wanted bits
		   & 0b1111;		  // mask wanted bits
}

bool game_is_entity_spawned(UID uid)
{
	for (uint8_t i = 0; i < num_entities; i++)
	{
		if (entity[i].uid == uid)
			return true;
	}

	return false;
}

bool game_is_static_entity_spawned(UID uid)
{
	for (uint8_t i = 0; i < num_static_entities; i++)
	{
		if (static_entity[i].uid == uid)
			return true;
	}

	return false;
}

void game_spawn_entity(uint8_t type, uint8_t x, uint8_t y)
{
	// Limit the number of spawned entities
	if (num_entities >= MAX_ENTITIES)
		return;

	// todo: read static entity status

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

void game_spawn_fireball(float x, float y)
{
	// Limit the number of spawned entities
	if (num_entities >= MAX_ENTITIES)
		return;

	UID uid = entities_get_uid(E_FIREBALL, x, y);
	// Remove if already exists, don't throw anything. Not the best,
	// but shouldn't happen too often
	if (game_is_entity_spawned(uid))
		return;

	// Calculate direction. 32 angles
	int16_t dir = FIREBALL_ANGLES + atan2f(y - player.pos.y, x - player.pos.x) /
										PI * FIREBALL_ANGLES;
	if (dir < 0)
		dir += FIREBALL_ANGLES * 2;
	entity[num_entities] = entities_create_fireball(x, y, dir);
	num_entities++;
}

void game_remove_entity(UID uid)
{
	uint8_t i = 0;
	bool found = false;

	while (i < num_entities)
	{
		if (!found && entity[i].uid == uid)
		{
			// todo: doze it
			found = true;
			num_entities--;
		}

		// displace entities
		if (found)
			entity[i] = entity[i + 1];

		i++;
	}
}

void game_remove_static_entity(UID uid)
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

		// displace entities
		if (found)
			static_entity[i] = static_entity[i + 1];

		i++;
	}
}

UID game_detect_collision(const uint8_t level[], Coords *pos, float rel_x,
						  float rel_y, bool only_walls)
{
	// Wall collision
	uint8_t round_x = (int)(pos->x + rel_x);
	uint8_t round_y = (int)(pos->y + rel_y);
	uint8_t block = game_level_get_block(level, round_x, round_y);

	if (block == E_WALL)
	{
		sound_play(hit_wall_snd, HIT_WALL_SND_LEN);
		return entities_get_uid(block, round_x, round_y);
	}

	if (only_walls)
	{
		return UID_NULL;
	}

	// Entity collision
	for (uint8_t i = 0; i < num_entities; i++)
	{
		// Don't collide with itself
		if (&(entity[i].pos) == pos)
			continue;

		uint8_t type = entities_get_type(entity[i].uid);

		// Only ALIVE enemy collision
		if ((type != E_ENEMY) || (entity[i].state == S_DEAD) ||
			(entity[i].state == S_HIDDEN))
			continue;

		Coords new_coords = {entity[i].pos.x - rel_x,
							 entity[i].pos.y - rel_y};
		uint8_t distance = coords_compute_distance(pos, &new_coords);

		// Check distance and if it's getting closer
		if (distance < ENEMY_COLLIDER_DIST && distance < entity[i].distance)
		{
			return entity[i].uid;
		}
	}

	return UID_NULL;
}

// Shoot
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
				GUN_MAX_DAMAGE / (fabsf(transform.x) * entity[i].distance) /
					5.0f);
			if (damage > 0)
			{
				entity[i].health = MAX(0, entity[i].health - damage);
				entity[i].state = S_HIT;
				entity[i].timer = 4;
			}
		}
	}
}

// Update coords if possible. Return the collided uid, if any
UID game_update_position(const uint8_t level[], Coords *pos, float rel_x,
						 float rel_y, bool only_walls)
{
	UID collide_x = game_detect_collision(level, pos, rel_x, 0, only_walls);
	UID collide_y = game_detect_collision(level, pos, 0, rel_y, only_walls);

	if (!collide_x)
		pos->x += rel_x;
	if (!collide_y)
		pos->y += rel_y;

	return collide_x || collide_y || UID_NULL;
}

void game_update_entities(const uint8_t level[])
{
	uint8_t i = 0;
	while (i < num_entities)
	{
		// update distance
		entity[i].distance = coords_compute_distance(&(player.pos), &(entity[i].pos));

		// Run the timer. Works with actual frames.
		// Todo: use delta here. But needs float type and more memory
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
			}
			else if (entity[i].state == S_HIT)
			{
				if (entity[i].timer == 0)
				{
					// Back to alert state
					entity[i].state = S_ALERT;
					entity[i].timer = 40; // delay next fireball thrown
				}
			}
			else if (entity[i].state == S_FIRING)
			{
				if (entity[i].timer == 0)
				{
					// Back to alert state
					entity[i].state = S_ALERT;
					entity[i].timer = 40; // delay next fireball throwm
				}
			}
			else
			{
				// ALERT STATE
				if (entity[i].distance > ENEMY_MELEE_DIST &&
					entity[i].distance < MAX_ENEMY_VIEW)
				{
					if (entity[i].state != S_ALERT)
					{
						entity[i].state = S_ALERT;
						entity[i].timer = 20; // used to throw fireballs
					}
					else
					{
						if (entity[i].timer == 0)
						{
							// Throw a fireball
							game_spawn_fireball(
								entity[i].pos.x,
								entity[i].pos.y);
							entity[i].state = S_FIRING;
							entity[i].timer = 6;
						}
						else
						{
							// move towards to the player.
							game_update_position(
								level, &(entity[i].pos),
								SIGN(player.pos.x, entity[i].pos.x) *
									ENEMY_SPEED * delta,
								SIGN(player.pos.y, entity[i].pos.y) *
									ENEMY_SPEED * delta,
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
						player.health = MAX(
							0, player.health - ENEMY_MELEE_DAMAGE);
						entity[i].timer = 14;
						flash_screen = 1;
					}
				}
				else
				{
					// stand
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
				player.health = MAX(0, player.health - ENEMY_FIREBALL_DAMAGE);
				flash_screen = 1;
				game_remove_entity(entity[i].uid);
				continue; // continue in the loop
			}
			else
			{
				// Move. Only collide with walls.
				// Note: using health to store the angle of the movement
				UID collided = game_update_position(
					level, &(entity[i].pos),
					cosf((float)entity[i].health / FIREBALL_ANGLES * PI) *
						FIREBALL_SPEED,
					sinf((float)entity[i].health / FIREBALL_ANGLES * PI) *
						FIREBALL_SPEED,
					true);

				if (collided)
				{
					game_remove_entity(entity[i].uid);
					continue; // continue in the entity check loop
				}
			}
			break;
		}

		case E_MEDKIT:
		{
			if (entity[i].distance < ITEM_COLLIDER_DIST)
			{
				// pickup
				sound_play(medkit_snd, MEDKIT_SND_LEN);
				entity[i].state = S_HIDDEN;
				player.health = MIN(100, player.health + 50);
				flash_screen = 1;
			}
			break;
		}

		case E_KEY:
		{
			if (entity[i].distance < ITEM_COLLIDER_DIST)
			{
				// pickup
				sound_play(get_key_snd, GET_KEY_SND_LEN);
				entity[i].state = S_HIDDEN;
				player.keys++;
				flash_screen = 1;
			}
			break;
		}
		}

		i++;
	}
}

// The map raycaster. Based on https://lodev.org/cgtutor/raycasting.html
void game_render_map(const uint8_t level[], float view_height)
{
	UID last_uid;

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

			uint8_t block = game_level_get_block(level, map_x, map_y);

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
						UID uid = entities_get_uid(block, map_x, map_y);
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
				distance = MAX(
					1, (map_x - player.pos.x + (1 - step_x) / 2) / ray_x);
			else
				distance = MAX(
					1, (map_y - player.pos.y + (1 - step_y) / 2) / ray_y);

			// store zbuffer value for the column
			zbuffer[x / Z_RES_DIVIDER] = MIN(
				distance * DISTANCE_MULTIPLIER, 0xff);

			// rendered line height
			uint8_t line_height = RENDER_HEIGHT / distance;

			display_draw_vline(
				x,
				view_height / distance - line_height / 2 + RENDER_HEIGHT / 2,
				view_height / distance + line_height / 2 + RENDER_HEIGHT / 2,
				GRADIENT_COUNT -
					(distance / MAX_RENDER_DEPTH * GRADIENT_COUNT) - side * 2);
		}
	}
}

// Sort entities from far to close
uint8_t game_sort_entities(void)
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

Coords game_translate_into_view(Coords *pos)
{
	// translate sprite position to relative to camera
	float sprite_x = pos->x - player.pos.x;
	float sprite_y = pos->y - player.pos.y;

	// required for correct matrix multiplication
	float inv_det =
		1.0 / (player.plane.x * player.dir.y - player.dir.x * player.plane.y);
	float transform_x =
		inv_det * (player.dir.y * sprite_x - player.dir.x * sprite_y);
	float transform_y = inv_det * (-player.plane.y * sprite_x +
								   player.plane.x * sprite_y); // Z in screen

	return (Coords){transform_x, transform_y};
}

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

		int16_t sprite_screen_x = HALF_WIDTH *
								  (1.0 + transform.x / transform.y);
		int8_t sprite_screen_y = RENDER_HEIGHT / 2 + view_height / transform.y;
		uint8_t type = entities_get_type(entity[i].uid);

		// don´t try to render if outside of screen
		// doing this pre-shortcut due int16 -> int8 conversion
		// makes out-of-screen values fit into the screen space
		if ((sprite_screen_x < -HALF_WIDTH) ||
			(sprite_screen_x > SCREEN_WIDTH + HALF_WIDTH))
			continue;

		switch (type)
		{
		case E_ENEMY:
		{
			uint8_t sprite;
			if (entity[i].state == S_ALERT)
				sprite = (millis() / 500) % 2; // walking
			else if (entity[i].state == S_FIRING)
				sprite = 2; // fireball
			else if (entity[i].state == S_HIT)
				sprite = 3; // hit
			else if (entity[i].state == S_MELEE)
				sprite = entity[i].timer > 10 ? 2 : 1; // melee atack
			else if (entity[i].state == S_DEAD)
				sprite = entity[i].timer > 0 ? 3 : 4; // dying
			else
				sprite = 0; // stand

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

void game_render_gun(uint8_t gun_pos, float amount_jogging)
{
	// jogging
	uint8_t x = 48 + sinf(millis() * JOGGING_SPEED) * 10 * amount_jogging;
	uint8_t y = RENDER_HEIGHT - gun_pos +
				fabsf(cosf(millis() * JOGGING_SPEED)) * 8 * amount_jogging;

	if (gun_pos > GUN_SHOT_POS - 2)
	{
		// Gun fire
		display_draw_bitmap(
			x + 6,
			y - 11,
			bmp_fire_bits,
			BMP_FIRE_WIDTH,
			BMP_FIRE_HEIGHT,
			true);
	}

	// Don't draw over the hud!
	uint8_t clip_height = MAX(0, MIN(y + BMP_GUN_HEIGHT, RENDER_HEIGHT) - y);

	// Draw the gun (black mask + actual sprite).
	display_draw_bitmap(x, y, bmp_gun_mask, BMP_GUN_WIDTH, clip_height, false);
	display_draw_bitmap(x, y, bmp_gun_bits, BMP_GUN_WIDTH, clip_height, true);
}

// Only needed first time
void game_render_hud_symbols(void)
{
	display_draw_text(2, 58, "{}", false);	// Health symbol
	display_draw_text(40, 58, "[]", false); // Keys symbol
	game_render_hud();
}

// Render values for the HUD
void game_render_hud(void)
{
	display_draw_rect(12, 58, 15, 6, false);
	display_draw_rect(50, 58, 5, 6, false);

	display_draw_int(12, 58, player.health);
	display_draw_int(50, 58, player.keys);
}

// Debug stats
void game_render_stats(void)
{
	display_draw_rect(58, 58, 70, 6, false);
	display_draw_int(114, 58, (uint8_t)(getActualFps()));
	display_draw_int(82, 58, num_entities);
}

// Intro screen
void game_run_intro(void)
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
		game_jump_to_scene(GAME_PLAY);
}

void game_run_level(void)
{
	display_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, false);

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
			player.velocity += (-MOV_SPEED - player.velocity) * 0.4f;
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
			rot_speed = ROT_SPEED * delta;
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
			rot_speed = ROT_SPEED * delta;
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

		view_height = fabsf(sinf(millis() * JOGGING_SPEED)) * 6.0f * jogging;

		if (view_height > 5.9f)
		{
			if (sound == false)
			{
				if (walkSoundToggle)
				{
					sound_play(walk1_snd, WALK1_SND_LEN);
					walkSoundToggle = false;
				}
				else
				{
					sound_play(walk2_snd, WALK2_SND_LEN);
					walkSoundToggle = true;
				}
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
			// ready to fire and fire pressed
			gun_pos = GUN_SHOT_POS;
			gun_fired = true;
			game_fire_shootgun();
		}
		else if (gun_fired && !input_fire())
		{
			// just fired and restored position
			gun_fired = false;
		}
	}
	else
	{
		// The player is dead
		if (view_height > -10.0f)
			view_height--;
		else if (input_fire())
			game_jump_to_scene(GAME_EXIT);

		if (gun_pos > 1)
			gun_pos -= 2;
	}

	// Player movement
	if (fabsf(player.velocity) > 0.003f)
	{
		game_update_position(
			level_1,
			&(player.pos),
			player.dir.x * player.velocity * delta,
			player.dir.y * player.velocity * delta,
			false);
	}
	else
		player.velocity = 0.0f;

	// Update things
	game_update_entities(level_1);

	// Render stuff
	game_render_map(level_1, view_height);
	game_render_entities(view_height);
	game_render_gun(gun_pos, jogging);

	// Fade in effect
	if (fade > 0)
	{
		display_fade(fade, false);
		fade--;
	}
	else
	{
		// Only draw the hud after fade in effect
		game_render_hud_symbols();
		game_render_hud();
		game_render_stats();
	}

	// flash screen
	if (flash_screen > 0)
	{
		invert_screen = ~invert_screen;
		flash_screen--;
	}
	else if (invert_screen)
	{
		display_invert();
		invert_screen = false;
	}

	// Exit routine
	if (input_select())
		game_jump_to_scene(GAME_EXIT);
}

void game_run_exit(void)
{
	// fade out effect
	for (uint8_t i = 0; i < GRADIENT_COUNT; i++)
	{
		display_fade(i, false);
		delay(40);
	}

	game_jump_to_scene(GAME_INTRO);
}

void main(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Doom Pico");
	SetTargetFPS(30);

	game_run_scene = game_run_intro;
	init_clock = clock();
	display_init();
	input_setup();
	sound_init();

	while (!WindowShouldClose())
	{
		BeginDrawing();

		input_update();
		game_run_scene();
		display_update();

		EndDrawing();
	}
}

/* -------------------------------------------------------------------------- */