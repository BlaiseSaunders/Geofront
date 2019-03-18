#include "theheader.h"

extern int maxx, maxy, wall_size;

extern float max_velocity;
extern float max_thrust;

extern int player_health;
extern int ai_health;

float angle_to_entity(entity *e1, entity *e2)
{
	int distx = e1->x - e2->x;
	int disty = e1->y - e2->y;
	return -(atan2(distx, disty) + M_PI_2 * 3);
}

void update_velocity(entity *e)
{
	e->xv += cos(e->angle)*e->thrust;
	e->yv += sin(e->angle)*e->thrust;
}

void update_aura_v(entity *entities, entity *e)
{
	int x1 = e->x + (e->size / 2),
		y1 = e->y + (e->size / 2),
		x2, y2;
	float dist, fx, fy, angle;
	int sign;
	entity *c;
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			break;
		if (c->z != e->z)
			continue;
		x2 = c->x + (c->size / 2);
		y2 = c->y + (c->size / 2);
		switch (c->type)
		{
			case V_WALL:
				sign = -(x2 - x1) / abs(x2 - x1);
				dist = pow(x2 - x1, 2);
				if (dist < pow(c->size, 2) * 2)
				{
					fx = pow(1 / dist, 2)*50000000.0 * sign;
					e->xv = e->xv + fx < max_velocity ? e->xv + fx : max_velocity;
				}
				break;
			case H_WALL:
				sign = -(y2 - y1) / abs(y2 - y1);
				dist = pow(y2 - y1, 2);
				if (dist < pow(c->size, 2) * 2)
				{
					fy = pow(1 / dist, 2)*50000000.0 * sign;
					e->yv = e->yv + fy < max_velocity ? e->yv + fy : max_velocity / 2;
				}
				break;

			default:
				angle = atan2(y2 - y1, x2 - x1);
				dist = pow(x2 - x1, 2) + pow(y2 - y1, 2);
				if (dist < pow(c->size + e->size, 2)*1.5)
				{
					fx = -(cos(angle)*pow(1 / dist, 2))*10000000.0;
					fy = -(sin(angle)*pow(1 / dist, 2))*10000000.0;
					e->xv = e->xv + fx < max_velocity ? e->xv + fx : max_velocity / 2;
					e->yv = e->yv + fy < max_velocity ? e->yv + fy : max_velocity / 2;
				}
				break;
		}
	}
}

/* Pushes people away from the walls */
void update_wall_aura_v(entity *e)
{
	float x = e->x + e->size / 2,
		y = e->y + e->size / 2;
	float fx, fy;

	/*if (x < wall_size)
	e->x = wall_size + 1;
	if (y < wall_size)
	e->y = wall_size + 1;*/


	if (e->x > maxx)
		e->x = maxx-wall_size;
	if (e->x > maxy)
		e->y = maxy-wall_size;
	if (e->x < wall_size)
		e->x = wall_size;
	if (e->x < wall_size)
		e->y = wall_size;


	/* Calculate how they should be pushed away */
	fx = pow(1 / (x - wall_size), 2) * 10000 - pow(1 / (maxx - x), 2) * 10000;
	fy = pow(1 / (y - wall_size), 2) * 10000 - pow(1 / (maxy - y), 2) * 10000;

	if (x < wall_size + 200 || x > maxx - 200)
		e->xv = e->xv + fx < max_velocity ? e->xv + fx : max_velocity;
	if (y < wall_size + 200 || y > maxy - 200)
		e->yv = e->yv + fy < max_velocity ? e->yv + fy : max_velocity;
}


void fire_weapon(entity *bullet, entity *e)
{
	int i;

	if (e->cool_pos > 0)
		return;

	/*c->x = e->x+e->size/2;
	c->y = e->y+e->size/2;*/


	if (e->type == PLAYER || e->type == P_AI)
	{
		switch (e->weapon)
		{
		default:
		case BASE_1:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle, 10);
			e->cool_pos = 21;
			break;
		case BASE_2:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle, 14);
			e->cool_pos = 18;
			break;
		case SHOTGUN_1:
			for (i = 0; i < 5; i++)
				fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle + (((rand() % 2) ? 1 : -1)*(
				(float)rand() / ((float)(RAND_MAX)) * 0.5)), 15);
			e->cool_pos = 60;
			break;
		case SHOTGUN_2:
			for (i = 0; i < 5; i++)
				fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle + (((rand() % 2) ? 1 : -1)*(
				(float)rand() / ((float)(RAND_MAX)) * 0.5)), 20);
			if (e->shot_pos)
			{
				e->shot_pos = 0;
				e->cool_pos = 60;
			}
			else
			{
				e->cool_pos = 10;
				e->shot_pos = 1;
			}
			break;
		case CRESCENT_1:
			fire_bullet(bullet, e, e->x + e->size / 2 + cos(e->angle)*e->size,
				e->y + e->size / 2 + sin(e->angle)*e->size,
				e->angle - M_PI_4, 20);
			fire_bullet(bullet, e, e->x + e->size / 2 + cos(e->angle)*e->size,
				e->y + e->size / 2 + sin(e->angle)*e->size,
				e->angle + M_PI_4, 20);
			fire_bullet(bullet, e, e->x + e->size / 2 + cos(e->angle)*e->size,
				e->y + e->size / 2 + sin(e->angle)*e->size,
				e->angle, 20);
			e->cool_pos = 10;
			break;
		case CRESCENT_2:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle - M_PI_4, 35);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle + M_PI_4, 35);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle, 35);
			e->cool_pos = 5;
			break;
		}
		return;
	}
	else
	{
		switch (e->weapon)
		{
		default:
		case BASE_1:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle, 7);
			e->cool_pos = 30;
			break;
		case BASE_2:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle, 9);
			e->cool_pos = 30;
			break;
		case SHOTGUN_1:
			for (i = 0; i < 5; i++)
				fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle + (((rand() % 2) ? 1 : -1)*(
				(float)rand() / ((float)(RAND_MAX)) * 0.5)), 15);
			e->cool_pos = 90;

			break;
		case SHOTGUN_2:
			for (i = 0; i < 5; i++)
				fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->t_angle + (((rand() % 2) ? 1 : -1)*(
				(float)rand() / ((float)(RAND_MAX)) * 0.5)), 15);
			if (e->shot_pos)
			{
				e->shot_pos = 0;
				e->cool_pos = 90;
			}
			else
			{
				e->cool_pos = 10;
				e->shot_pos = 1;
			}

			break;
		case CRESCENT_1:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle - M_PI_4, 18);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle + M_PI_4, 18);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle, 18);
			e->cool_pos = 18;
			break;
		case CRESCENT_2:
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle - M_PI_4, 25);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle + M_PI_4, 25);
			fire_bullet(bullet, e, e->x + e->size / 2, e->y + e->size / 2,
				e->angle, 25);
			e->cool_pos = 10;
			break;

		}
		return;

	}

}


void fire_bullet(entity *bullet, entity *e, int x, int y, float angle, int damage)
{
	entity *c;

	for (c = bullet;; c = c->next)
		if (c->type == TRASH)
			break;
	c->type = BUL;
	c->x = x;
	c->y = y;
	c->z = e->z;
	c->thrust = max_thrust;
	c->xv = (max_velocity * 3)*cos(angle);
	c->yv = (max_velocity * 3)*sin(angle);
	c->size = 20;
	c->angle = angle;
	c->team = e->team;
	c->age = 0;
	c->damage = damage;

	if (c->next == NULL)
	{
		c->next = (entity *)malloc(sizeof(entity));
		memset(c->next, 0, sizeof(entity));
		c->next->type = TRASH;
		c->next->next = NULL;
	}
}

entity *get_entity_by_num(entity *e, int num)
{
	int i;
	entity *c;
	for (c = e, i = 0; i < num; i++, c = e->next)
		if (c == NULL || c->next == NULL)
			return NULL;
	return c;
}

entity *add_ai(entity *entities, entity *ai, int count, int team, e_form form, int id)
{
	int i;
	entity *c, *c2;
	if (ai == NULL)
	{
		ai = (entity *)malloc(sizeof(entity));
		memset(ai, 0, sizeof(entity));
		ai->next = NULL;
		ai->type = TRASH;
		c = ai;
	}
	else
		for (c = ai;; c = c->next)
			if (c == NULL || c->next == NULL/* ||
			 c->type == TRASH*/)
				 break;

	for (i = 0; i < count; i++, c = c->next)
	{
		c->size = 30 + (rand() % 10);
		c->type = F_AI;
		c->team = team;
		c->xp = 0;
		c->health = get_level_health(form, F_AI);
		c->cool_pos = 0;
		c->mode = BEHIND;
		c->weapon = form;
		c->id2 = id;

		c2 = find_respawn_point(entities, c);
		if (c != c2)
		{
			c->x = c2->x;
			c->y = c2->y;
			c->z = c2->z;
		}
		else
		{
			c->x = 2000;
			c->y = 2000;
			c->z = 0;
		}


		c->next = (entity *)malloc(sizeof(entity));
		memset(c->next, 0, sizeof(entity));
		c->next->next = NULL;
		c->next->type = TRASH;
	}
	return ai;
}

entity *add_player(entity *entities, entity *player, int count)
{
	int i, teams = 0;
	entity *c, *c2;
	for (c = player;; c = c->next, teams++)
		if (c == NULL || c->next == NULL)
			break;

	for (i = 0; i < count; i++, c = c->next, teams++)
	{
		c->size = 60;
		c->type = P_AI;
		c->team = teams;
		c->cool_pos = 0;
		c->xp = 0;
		c->weapon = BASE_1;
		c->scrap = 500;
		c->lackey_cap = 500;
		c->shot_pos = 0;
		c->thrust = 0;
		c->xv = 0;
		c->yv = 0;

		c->health = get_level_health(c->weapon, c->type);

		c2 = find_respawn_point(entities, c);
		if (c != c2)
		{
			c->x = c2->x - (player->size * 2);
			c->y = c2->y - (player->size * 2);
			c->z = c2->z;
		}
		else
		{
			c->x = 2000;
			c->y = 2000;
			c2->z = 0;
		}

		c->next = (entity *)malloc(sizeof(entity));
		memset(c->next, 0, sizeof(entity));
		c->next->next = NULL;
		c->next->type = TRASH;
	}
	return player;
}

int get_ai_num(entity *e, entity *main)
{
	int i;
	entity *c;
	for (i = 0, c = main;; c = c->next)
	{
		if (c->team == e->team)
			i++;
		if (c == e)
			return i;
	}
}

entity *check_bullet_collision(entity *bullets, entity *e)
{
	entity *c;

	for (c = bullets;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return NULL;
		if (e->team != c->team && test_collision_square(c, e) == OTHER)
			return c;
	}
}

entity *find_respawn_point(entity *entities, entity *e)
{
	int dist, best_dist = maxx + maxy;
	entity *c, *best = e;

	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			break;
		if (c->type == BASE && c->team == e->team)
		{
			if (best == e)
				best = c;
			dist = abs(c->x - e->x) + abs(c->x - e->x);
			if (dist < best_dist)
			{
				best_dist = dist;
				best = c;

			}
		}
	}
	return best;
}
void set_ai_mode(entity *ai, e_mode mode, int team)
{
	entity *c;

	for (c = ai;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return;
		if (c->type == TRASH)
			continue;
		if (c->team == team)
			c->mode = mode;
	}
}

bool owns_outpost_level(entity *entities, int team, int id)
{
	entity *c;
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return false;
		if (c->team == team && c->id == id &&
			(c->type == OUTPOST || c->type == BASE))
			return true;
	}
}

bool owns_outpost(entity *entities, int team, int id2)
{
	entity *c;
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return false;
		if (c->team == team && c->id2 == id2)
			return true;
	}
}



entity *find_player(entity *players, int team)
{
	entity *c;

	for (c = players;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return players;
		if (c->type != TRASH && c->team == team)
			return c;
	}
}

entity *find_nearest_ally_outpost(entity *entities, entity *e)
{
	int dist, best_dist = maxx + maxy;
	entity *c, *best = e;

	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return best;
		if (c->team == e->team && c->id == e->id - 1 &&
			(c->type == OUTPOST || c->type == BASE))
		{
			dist = abs(e->x - c->x) + abs(e->y - c->y);
			if (dist < best_dist)
			{
				best_dist = dist;
				best = c;
			}

		}

	}
}

void set_squad_position(entity *entities, entity *e, entity *player, entity *ai)
{
	int pos = get_ai_num(e, ai);
	entity player_pos;

	/* Make sure we're on the same floor */
	if (e->z == player->z)
		switch (e->mode)
		{
			default:
				player_pos.x = player->x-50*pos*cos(player->angle);
				player_pos.y = player->y-50*pos*sin(player->angle);
				e->angle = angle_to_entity(&player_pos, e);
			break;
		}
	else
	{
		if (e->z < player->z)
			e->angle = angle_to_entity(find_nearest_type(entities, e, LIFT,
			NAE), e);
		else
			e->angle = angle_to_entity(find_nearest_type(entities, e, DROP,
			NAE), e);
	}

}

int get_lackey_count(entity *ai, int team)
{
	int count = 0;
	entity *c;

	for (c = ai;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return count;
		if (c->team == team)
			count++;
	}

}

bool wall_in_way(entity *e1, entity *e2, entity *entities)
{
	entity *c;

	if (e1->z != e2->z)
		return false;

	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return false;
		if (c->z != e1->z)
			continue;
		if (c->type == V_WALL)
			if ((e1->x < c->x && e2->x > c->x) ||
				(e2->x < c->x && e1->x > c->x))
				return true;
		if (c->type == H_WALL)
			if ((e1->y < c->y && e2->y > c->y) ||
				(e2->y < c->y && e1->y > c->y))
				return true;
	}

}

entity *find_closest_enemy(entity *e, entity *enemy, int best_dist, entity *entities)
{
	int dist;
	entity *c;
	entity *best = e;

	for (c = enemy;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return best;
		if (c->team != e->team && c->type != TRASH &&
			c->type != WALL && c->z == e->z && c->team != -9)
		{
			dist = sqrt(pow(e->x - c->x, 2) + pow(e->y - c->y, 2));
			if (dist < best_dist && !wall_in_way(e, c, entities))
			{
				best_dist = dist;
				best = c;
			}
		}

	}
}


entity *find_nearest_type(entity *entities, entity *e, e_type type, e_type type2)
{
	int dist, best_dist = maxx + maxy;
	entity *c, *best = e;

	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return best;
		if ((c->type == type || c->type == type2) &&
			c->z == e->z && e->team != c->team)
		{
			dist = sqrt(pow(e->x - c->x, 2) + pow(e->y - c->y, 2));
			if (dist < best_dist && !wall_in_way(e, c, entities))
			{
				best_dist = dist;
				best = c;
			}

		}

	}
}


entity *add_turret(entity *entities, entity *e, e_type type)
{
	entity *c;
	for (c = entities;; c = c->next)
		if (c == NULL || c->next == NULL /*||
		    c->type == DEAD_OUTPOST*/)
			break;

	c->x = e->x - 100;
	c->y = e->y - 100;
	c->z = e->z;
	c->team = e->team;
	c->size = e->size;

	switch (type)
	{
		default:
		case L_TURRET:
			c->type = L_TURRET;
			c->weapon = CRESCENT_2;
			c->health = 1000;
			c->cool_pos = 10;
			break;
		case H_TURRET:
			c->type = H_TURRET;
			c->weapon = SHOTGUN_1;
			c->health = 500;
			c->cool_pos = 1800;
			break;
		case SLOW_TURRET:
			c->type = SLOW_TURRET;
			c->weapon = SEMEN;
			c->health = 250;
			c->cool_pos = 100;
			break;
	}

	if (c->next == NULL)
	{
		c->next = (entity *)malloc(sizeof(entity));
		memset(c->next, 0, sizeof(entity));
		c->next->next = NULL;
		c->next->type = TRASH;
	}

	return entities;
}

bool close_to_outpost(entity *entities, entity *e)
{
	int dist;
	entity *c;
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			return false;
		if (c->type == OUTPOST && c->team == e->team)
		{
			dist = sqrt(pow(e->x - c->x, 2) + pow(e->y - c->y, 2));
			if (dist < 300)
				return true;
		}
	}
}
