#include "theheader.h"

extern SDL_Window *screen;
extern TTF_Font *font;

extern int wall_size;


void draw_text(int x, int y, char *string, int r, int g, int b, int a)
{
	extern SDL_Renderer *renderer;
	SDL_Color color;
	SDL_Surface *surface;
	SDL_Texture *texture;
	SDL_Rect rect;

	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;

	TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
	surface = TTF_RenderText_Blended(font, string, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	rect.x = x;
	rect.y = y;
	rect.h = 48;
	rect.w = 24*strlen(string);

	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}


void draw_buy_menu(int x, int y, int size)
{
	SDL_Rect rect;
	extern SDL_Renderer *renderer;

	/* Top */
	rect.x = x;
	rect.y = y+size;
	rect.w = size;
	rect.h = size;
	SDL_RenderDrawRect(renderer, &rect);
	/* Left */
	rect.x = x-size;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderDrawRect(renderer, &rect);
	/* Bottom */
	rect.x = x;
	rect.y = y-size;
	rect.w = size;
	rect.h = size;
	SDL_RenderDrawRect(renderer, &rect);
	/* Right */
	rect.x = x+size;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderDrawRect(renderer, &rect);
	draw_l_turret(x+size, y, 0, size, 0);
}



void draw_l_turret(int x, int y, float t_angle, int size, int health)
{
	extern SDL_Renderer *renderer;


	SDL_RenderDrawLine(renderer,
		x + (size / 2), y,
		x, y + (size / 2));
	SDL_RenderDrawLine(renderer,
		x + (size / 2), y,
		x + size, y + (size / 2));
	SDL_RenderDrawLine(renderer,
		x, y + (size / 2),
		x + (size / 2), y + size);
	SDL_RenderDrawLine(renderer,
		x + size, y + (size / 2),
		x + (size / 2), y + size);


	SDL_RenderDrawLine(renderer,
		x + (size / 2), y + (size / 2),
		x + (size / 2) + cos(t_angle)*size / 2, y + (size / 2) + sin(t_angle)*size / 2);


	draw_hp(x + (size / 2), y - (size / 2), health);
}



void draw_ship(int x, int y, float angle, int size, float t_angle, int health, int weapon)
{
	switch (weapon)
	{
		default:
		case BASE_1:
		case BASE_2:
			draw_ship_tri(x, y, angle, size, t_angle, health);
			break;
		case SHOTGUN_1:
		case SHOTGUN_2:
			draw_ship_w(x, y, angle, size, health);
			break;
		case CRESCENT_1:
		case CRESCENT_2:
			draw_ship_cres(x, y, angle, size, health);
			break;
	}
}

void draw_ship_cres(int x, int y, float angle, int size, int health)
{
	float init;
	extern SDL_Renderer *renderer;


	x = x + (size / 2);
	y = y + (size / 2);

	init = angle;
	angle -= M_PI_2;

	for (; angle < init + M_PI_2; angle += 1.0f / size)
		SDL_RenderDrawPoint(renderer,
		x + (cos(angle)*size),
		y + (sin(angle)*size));

	draw_hp(x + (size / 2), y - (size / 2), health);
}

void draw_ship_tri(int x, int y, float angle, int size, float t_angle, int health)
{
	extern SDL_Renderer *renderer;
	/*angle += M_PI;*/

	SDL_RenderDrawLine(renderer,
		x + (size / 2) + cos(angle)*size / 2, y + (size / 2) + sin(angle)*size / 2,
		x + (size / 2) - cos(angle + M_PI_4)*size / 2, y + (size / 2) - sin(angle + M_PI_4)*size / 2);
	SDL_RenderDrawLine(renderer,
		x + (size / 2) + cos(angle)*size / 2, y + (size / 2) + sin(angle)*size / 2,
		x + (size / 2) - cos(angle - M_PI_4)*size / 2, y + (size / 2) - sin(angle - M_PI_4)*size / 2);
	SDL_RenderDrawLine(renderer,
		x + (size / 2), y + (size / 2),
		x + (size / 2) + cos(t_angle)*size / 2, y + (size / 2) + sin(t_angle)*size / 2);

	draw_hp(x + (size / 2), y - (size / 2), health);
}

void draw_ship_w(int x, int y, float angle, int size, int health)
{
	extern SDL_Renderer *renderer;
	SDL_RenderDrawLine(renderer,
		x + (size / 2) + cos(angle)*size / 2, y + (size / 2) + sin(angle)*size / 2,
		x + (size / 2) + cos(angle + M_PI_4)*size / 2, y + (size / 2) + sin(angle + M_PI_4)*size / 2);
	SDL_RenderDrawLine(renderer,
		x + (size / 2) + cos(angle)*size / 2, y + (size / 2) + sin(angle)*size / 2,
		x + (size / 2) + cos(angle - M_PI_4)*size / 2, y + (size / 2) + sin(angle - M_PI_4)*size / 2);


	draw_hp(x + (size / 2), y - (size / 2), health);
}

void draw_h_wall(int y, int size)
{
	int w, h;
	SDL_Rect rect;
	extern SDL_Renderer *renderer;

	SDL_GetWindowSize(screen, &w, &h);

	rect.x = 0;
	rect.y = y;
	rect.w = w;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);
}


void draw_v_wall(int x, int size)
{
	int w, h;
	SDL_Rect rect;
	extern SDL_Renderer *renderer;

	SDL_GetWindowSize(screen, &w, &h);

	rect.x = x;
	rect.y = 0;
	rect.w = size;
	rect.h = h;
	SDL_RenderFillRect(renderer, &rect);
}

void draw_hp(int x, int y, int hp)
{
	extern SDL_Renderer *renderer;
	SDL_Rect rect;

	rect.x = x - (hp / 4);
	rect.y = y;
	rect.w = hp / 2;
	rect.h = 5;
	SDL_RenderFillRect(renderer, &rect);
}

void draw_bullet(int x, int y, float angle, int size)
{
	extern SDL_Renderer *renderer;
	SDL_RenderDrawLine(renderer,
		x, y,
		x + cos(angle)*size, y + sin(angle)*size);
}

void draw_wall(int x, int y, int s)
{
	extern SDL_Renderer *renderer;
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;
	rect.w = s;
	rect.h = s;
	SDL_RenderFillRect(renderer, &rect);
}

void draw_borders(int x, int y, int maxx, int maxy, int size)
{
	extern SDL_Renderer *renderer;
	SDL_Rect rect;

	rect.x = 0 - x;
	rect.y = 0 - y;
	rect.w = maxx + size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);
	rect.x = 0 - x;
	rect.y = 0 - y;
	rect.w = size;
	rect.h = maxy + size;
	SDL_RenderFillRect(renderer, &rect);
	rect.x = 0 - x + maxx;
	rect.y = 0 - y;
	rect.w = size;
	rect.h = maxy + size;
	SDL_RenderFillRect(renderer, &rect);
	rect.x = 0 - x;
	rect.y = 0 - y + maxy;
	rect.w = maxx + size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);
}


void draw_outpost(int x, int y, int size, int health)
{
	extern SDL_Renderer *renderer;

	/*rect.x = x;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);*/

	SDL_RenderDrawLine(renderer,
		x + (size / 2), y,
		x, y + (size / 2));
	SDL_RenderDrawLine(renderer,
		x + (size / 2), y,
		x + size, y + (size / 2));
	SDL_RenderDrawLine(renderer,
		x, y + (size / 2),
		x + (size / 2), y + size);
	SDL_RenderDrawLine(renderer,
		x + size, y + (size / 2),
		x + (size / 2), y + size);


	draw_hp(x + (size / 2), y - (size / 2), health);
}

void draw_base(int x, int y, int size, int health)
{
	extern SDL_Renderer *renderer;
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);

	draw_hp(x + (size / 2), y - (size / 2), health);
}


entity *load_entities(char *dat_file)
{
	int i, j, count, id2;
	extern int maxx, maxy, maxz, base_health;
	char buf[1024], buf2[1024];
	FILE *fp;
	entity *entities, *c;

	srand(time(NULL));

	if ((fp = fopen(dat_file, "r")) == NULL)
	{
		printf("Failed to open %s.\n", dat_file);
		return NULL;
	}

	fgets(buf, sizeof buf, fp);
	maxx = atoi(buf);
	fgets(buf, sizeof buf, fp);
	maxy = atoi(buf);
	fgets(buf, sizeof buf, fp);
	maxz = atoi(buf);

	entities = (entity *)malloc(sizeof(entity));
	memset(entities, 0, sizeof(entity));
	entities->next = NULL;
	for (c = entities, id2 = 0; fgets(buf, sizeof buf, fp) != NULL; id2++)
	{
		if (buf[0] == '*')
			continue;
		for (i = 0, j = 0, count = 0; i < 1024 && buf[i] != '\0'; i++)
		{
			if (buf[i] == ',')
			{
				buf2[j] = '\0';
				j = 0;
				count++;
				c->id2 = id2;
				switch (count)
				{
					case 1:
						c->x = atoi(buf2);
						break;
					case 2:
						c->y = atoi(buf2);
						break;
					case 3:
						c->z = atoi(buf2);
						break;
					case 4:
						c->size = atoi(buf2);
						break;
					case 5:
						c->type = (e_type)atoi(buf2);
						c->xp = 0;
						if (c->type == R_OUTPOST)
						{
							if (rand() % 5 > 0)
							{
								c->type = OUTPOST;
								c->x += ((rand() % 2 ? -1 : 1))*(rand() % 500);
								c->y += ((rand() % 2 ? -1 : 1))*(rand() % 500);
								if (c->x + c->size > maxx)
									c->x -= c->size;
								if (c->y + c->size > maxx)
									c->y -= c->size;
								if (c->x < 0)
									c->x += wall_size;
								if (c->y < 0)
									c->y += wall_size;
							}
							else
								c->type = TRASH;
						}
						if (c->type == OUTPOST)
							c->health = 100;
						if (c->type == BASE)
							c->health = base_health;
						break;
					case 6:
						c->team = atoi(buf2);
						break;
					case 7:
						c->weapon = (e_form)atoi(buf2);
						break;
					case 8:
						c->id = atoi(buf2);
						break;
					default:
						break;

				}
				continue;
			}
			buf2[j++] = buf[i];
		}
		c->next = (entity *)malloc(sizeof(entity));
		memset(c->next, 0, sizeof(entity));
		c->next->type = TRASH;
		c->next->next = NULL;
		c = c->next;
	}
	c->next = NULL;
	return entities;

}


side test_collision_border(entity *e)
{
	int nextx, nexty;
	extern int maxx, maxy;


	/* Where are we going to be next step */
	nextx = get_next_x(e);
	nexty = get_next_y(e);

	if (nextx > maxx || nextx < wall_size ||
		nexty > maxy || nexty < wall_size)
		return OTHER;
	return NONE;
}


side test_collision_square(entity *e1, entity *e2)
{
	int nextx, nexty, time;

	/* c1----------c2
	* |           |
	* |           |
	* |           |
	* |           |
	* |           |
	* |           |
	* c3----------c4
	*/


	/* Nut out the easy one */
	if (e1->z != e2->z)
		return NONE;




	/* Where are we going to be next step */
	nextx = get_next_x(e1);
	nexty = get_next_y(e1);


	/* Are we inside the x bounds of the square*/
	if ((nextx + e1->size > e2->x && nextx + e1->size < e2->x + e2->size)
		|| (nextx > e2->x && nextx < e2->x + e2->size))
		/* Are we inside the y bounds of the square*/
		if ((nexty + e1->size > e2->y && nexty + e1->size < e2->y + e2->size)
			|| (nexty > e2->y && nexty < e2->y + e2->size))
		{
			return OTHER;
			if (e1->y > e2->y) /* We were above it */
			{
				if (e1->x + e1->size > e2->x) /* We were to its right */
				{
					/* Let's check c3 against the top and right side */
					for (time = 0;; time++)
					{
						if (e1->x + cos(e1->angle)*time == e2->x) /* We hittin' the right */
							return RIGHT;
						if (e1->y + sin(e1->angle)*time == e2->y + e2->size) /* We hittin' the top */
							return TOP;
					}
				}
				else if (e1->x < e2->x) /* We were to its left */
				{
					/* Let's check c4 against the top and left side */
					for (time = 0;; time++)
					{
						if (e1->x + e1->size + cos(e1->angle)*time == e2->x) /* We hittin' the left */
							return LEFT;
						if (e1->y + e1->size + sin(e1->angle)*time == e2->y) /* We hittin' the right */
							return TOP;
					}

				}
				else /* We were on the same x */
				{
					return TOP;
				}
			}
			else if (e1->y < e2->y + e2->size) /* We were below it */
			{
				if (e1->x + e1->size > e2->x) /* We were to its right */
				{
					/* Let's check c1 against the bottom and right side */
					for (time = 0;; time++)
					{
						if (e1->x + cos(e1->angle)*time == e2->x) /* We hittin' the right */
							return RIGHT;
						if (e1->y + sin(e1->angle)*time == e2->y + e2->size) /* We hittin' the bottom */
							return BOTTOM;
					}
				}
				else if (e1->x + e1->size < e2->x) /* We were to its left */
				{
					/* Let's check c2 against the bottom and left side */
				}
				else /* We were on the same x */
				{
					return BOTTOM;
				}

			}
			else /* We were on the same y */
			{
				if (e1->x + e1->size > e2->x) /* We were to its right */
					return RIGHT;
				else if (e1->x + e1->size < e2->x) /* We were to its left */
					return LEFT;
				else /* We were on the same x */
					return CORNER;
			}
		}

	return NONE;
}
