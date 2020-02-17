#include "theheader.h"

#include "munit.h"


int AIMBOT          = 0;
int INFINITE_HEALTH = 0;
int AUTO_FIRE       = 0;
int AUTO_AIM        = 1;
input_t input_type = MANUAL;

SDL_Window *screen;
SDL_Renderer *renderer;
TTF_Font *font;

int wall_size = 100;

float max_thrust = .9;
float max_velocity = 22;
float thrust = .06;
float slow = .975;

float rotation_speed = 0.1f;

int maxx = 0;
int maxy = 0;
int maxz = 0;

int outpost_health = 200;
int base_health = 1000;

int minimap_scale;
int minimap_size;

int bullet_damage = 1;

struct keyboard_s keyboard;
SDL_Event event;



int main(int argc, char **argv)
{
	munit_assert_int(0, ==, 0);

	return 0;
}



void cleanup()
{
	SDL_Quit();
}

int get_next_x(entity *e)
{
	return e->x + e->xv;
}
int get_next_y(entity *e)
{
	return e->y + e->yv;
}

void set_colour(int colour)
{
	switch (colour)
	{
		case -5:
			SDL_SetRenderDrawColor(renderer, 146, 172, 88,
				255); /*blue*/
			break;
		case 0:
			SDL_SetRenderDrawColor(renderer, 207, 153, 17,
				255);/*yellow*/
			break;
		case 1:
			SDL_SetRenderDrawColor(renderer, 187, 119, 164,
				255);/*magenta*/
			break;
		case 2:
		case -2:
			SDL_SetRenderDrawColor(renderer, 226, 31, 62,
				255);/*red*/
			break;
		case 3:
		case -3:
			SDL_SetRenderDrawColor(renderer, 61, 160, 129,
				255);/*blue*/
			break;
		default:
			SDL_SetRenderDrawColor(renderer, 160, 157, 138,
				255);/*off white*/
			break;
	}
}

void level_up(entity *player)
{
	switch (player->weapon)
	{
		default:
			break;
		case BASE_1:
			if (player->xp < 100)
				break;
			player->weapon = BASE_2;
			player->xp -= 100;
			break;
		case BASE_2:
			if (player->xp >= 150)
			{
				player->weapon = SHOTGUN_1;
				player->xp -= 150;
			}
			break;
		case SHOTGUN_1:
			if (player->xp >= 300)
			{
				player->weapon = SHOTGUN_2;
				player->xp -= 300;
			}
			break;
		case SHOTGUN_2:
			if (player->xp >= 400)
			{
				player->weapon = CRESCENT_1;
				player->xp -= 400;
			}
			break;
		case CRESCENT_1:
			if (player->xp >= 750)
			{
				player->weapon = CRESCENT_2;
				player->xp -= 750;
			}
			break;
	}

}

int get_level_health(e_form level, e_type type)
{
	if (type == PLAYER || type == P_AI)
		switch (level)
		{
			default:
			case BASE_1:
			case BASE_2:
					return 200;
			case SHOTGUN_1:
				return 250;
			case SHOTGUN_2:
				return 275;
			case CRESCENT_1:
				return 325;
			case CRESCENT_2:
				return 400;
		}
	else
		switch (level)
		{
			default:
			case BASE_1:
				return 15;
			case BASE_2:
				return 20;
			case SHOTGUN_1:
			case SHOTGUN_2:
				return 15;
			case CRESCENT_1:
				return 50;
			case CRESCENT_2:
				return 100;
		}
}

void respawn(entity *entities, entity *e, entity *players)
{
	entity *c;

	c = find_respawn_point(entities, e);

	if (e == c || (e->type == F_AI &&
	    !owns_outpost(entities, e->team, e->id2)))
	{
		e->type = TRASH;
		find_player(players, e->team)->lackey_count--;
		return;
	}

	e->x = c->x - (c->size);
	e->y = c->y - (c->size);
	e->z = c->z;
	e->health = get_level_health(e->weapon, e->type);
	e->xp = 0;
}



void update_entity_collision(entity *entities, entity *player)
{
	entity *c;
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			break;
		if (c->type == TRASH || c->z != player->z)
			continue;

		switch (c->type)
		{
			default:
				break;
			case DROP:
				if (test_collision_square(player, c) == OTHER)
					player->z--;
				break;
			case LIFT:
				if (test_collision_square(player, c) == OTHER)
					player->z++;
				break;
		}
	}
}
