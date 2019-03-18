#include "theheader.h"

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

	/*
	* Declarations
	*/
	int w, h, i, woff = 0, hoff = 0;
	unsigned int frame_limit, ticks;
	char buf[4096], *map;
	/*float ypr[3], ypr_offset[3] = {0.0, 0.0, 0.0}; Yaw, Pitch, Roll */
	bool pressed = false, firing = false;
	struct cam_s camera;


	entity *players = NULL;
	entity *player;
	entity *ai = NULL;
	entity *entities, *c, *c2;
	entity *bullet;
	SDL_Rect rect;

	if (argc > 1)
		printf("Usage: %s\n", argv[0]);

	/*
	 * Initialisation
	 */


	/* Setup SDL */
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER|
	             SDL_INIT_AUDIO) < 0)
	{
		printf("Failed to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (TTF_Init() < 0)
		printf("TTF Fudged up\n");

	font = TTF_OpenFont("font/DroidSansMono.ttf", 48);
	if (font == NULL)
		printf("Managed to fudge up opening the font: %s\n", TTF_GetError());
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
	TTF_SetFontKerning(font, 0);

	if ((screen = SDL_CreateWindow("Game",
		                       30,
		                       50,
		                       SCREEN_WIDTH, SCREEN_HEIGHT,
		                       SDL_WINDOW_OPENGL/*|SDL_WINDOW_FULLSCREEN_DESKTOP*/)) == NULL)
	{
		printf("Failed to create SDL window: %s\n", SDL_GetError());
		return 1;
	}
	if ((renderer = SDL_CreateRenderer(screen, -1, 0)) == NULL)
	{
		printf("Failed to create SDL screen: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_SetSwapInterval(1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	atexit(cleanup);




	/*
	 * Show the main menu which
	 * gets the map to load.
	 */
	map = show_main_menu();


	/* Init data */
	memset(&(keyboard), 0, sizeof(struct keyboard_s));
	memset(&(camera), 0, sizeof(struct cam_s));

	printf("Loading map: %s\n", map);
	fflush(stdout);
	entities = load_entities(map);

	players = (entity *)malloc(sizeof(entity));
	memset(players, 0, sizeof(entity));
	players->next = NULL;

	bullet = (entity *)malloc(sizeof(entity));
	memset(bullet, 0, sizeof(entity));
	bullet->type = TRASH;
	bullet->next = NULL;


	/* Add players */
	for (c = entities;; c = c->next)
	{
		if (c == NULL || c->next == NULL)
			break;
		if (c->type == BASE)
			players = add_player(entities, players, 1);

	}

	/* Setup the main player */
	player = players->next;



	/* Init Misc */
	srand(time(NULL));




	/*
	 * Initialise the multiplayer breads
	 *
	params.players = player;
	player->type = NET;
	strcpy(params.ip, "192.168.88.233");
	pthread_create(&thread[0], NULL, update_players, &params);


	*
	 * Serial bread
	 *
	s_params.ypr = ypr;
	s_params.firing = &firing;
	pthread_create(&serial_t, NULL, read_serial, &s_params);
	*/

	/*
	* Loop
	*/
	frame_limit = SDL_GetTicks() + 16;
	while (1)
	{
		/* TODO: Check for player death */


		/*
		* Check for a change in screen size
		*/
		SDL_GetWindowSize(screen, &w, &h);


		/*
		* Get Input
		*/
		if (get_input(player))
		{
			/*pthread_cancel(serial_t);*/
			return 0;
		}

		/*
		* Handle Input
		*/
		switch (input_type)
		{
			case MANUAL:
				if (keyboard.w && player->thrust < max_thrust)
					player->thrust += thrust;
				if (keyboard.s && player->thrust > -max_thrust)
					player->thrust -= thrust;
				if (keyboard.d)
					player->angle += rotation_speed;
				if (keyboard.a)
					player->angle -= rotation_speed;
				break;
			case AUTO:
				if (keyboard.w)
				{
					player->angle = M_PI / 2 * 3;
					player->thrust = max_thrust;
				}
				if (keyboard.a)
				{
					player->angle = M_PI;
					player->thrust = max_thrust;
				}
				if (keyboard.s)
				{
					player->angle = M_PI / 2;
					player->thrust = max_thrust;
				}
				if (keyboard.d)
				{
					player->angle = 0;
					player->thrust = max_thrust;
				}
				if (keyboard.w && keyboard.d)
				{
					player->angle = (M_PI * 2) - M_PI / 4;
					player->thrust = max_thrust;
				}
				if (keyboard.w && keyboard.a)
				{
					player->angle = M_PI + M_PI / 4;
					player->thrust = max_thrust;
				}
				if (keyboard.s && keyboard.a)
				{
					player->angle = M_PI - M_PI / 4;
					player->thrust = max_thrust;
				}
				if (keyboard.s && keyboard.d)
				{
					player->angle = M_PI / 4;
					player->thrust = max_thrust;
				}
				break;
		}

		if (keyboard.m || keyboard.tab)
		{
			minimap_scale = 20;
			minimap_size = 10;
		}
		else
		{
			minimap_scale = 40;
			minimap_size = 5;
		}


		if (!keyboard.e)
			pressed = false;


		if (close_to_outpost(entities, player) && !pressed &&
		    keyboard.right && keyboard.e && player->scrap >= 500)
		{
			player->scrap -= 500;
			add_turret(entities, player, L_TURRET);
			pressed = true;
		}



		if (keyboard.one)
			set_ai_mode(ai, BEHIND, player->team);
		if (keyboard.three)
			set_ai_mode(ai, TARGET, player->team);
		if (keyboard.four)
			set_ai_mode(ai, DEPLOY, player->team);


		if (keyboard.right && !keyboard.e)
			player->t_angle += rotation_speed;
		if (keyboard.left && !keyboard.e)
			player->t_angle -= rotation_speed;

		if (keyboard.down || AUTO_AIM)
		{
			player->t_angle = angle_to_entity((c = find_closest_enemy(player, player, 1000,
				entities)), player);
			if (c == player)
				player->t_angle = angle_to_entity((c = find_closest_enemy(player, ai,
				1000, entities)), player);
			if (c == player)
				player->t_angle = angle_to_entity((c = find_closest_enemy(player,
				entities, 1000, entities)), player);
		}


		player->cool_pos--;
		if (keyboard.space || keyboard.up || keyboard.down || firing)
			fire_weapon(bullet, player);



		/*if (keyboard.l)
		{
			ypr_offset[0] = ypr[0];
			ypr_offset[1] = ypr[1];
			ypr_offset[2] = ypr[2];
		}*/



		/* Check if they've done a loop */
		while (player->angle > M_PI * 2)
			player->angle -= M_PI * 2;
		while (player->angle < 0)
			player->angle += M_PI * 2;
		while (player->t_angle > M_PI * 2)
			player->t_angle -= M_PI * 2;
		while (player->angle < 0)
			player->t_angle += M_PI * 2;



		/*
		* Physics
		*/


		/* Move the other players */
		for (c = players;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == TRASH)
				continue;

			if ((c2 = check_bullet_collision(bullet, c)) != NULL)
				c->health -= c2->damage;

			if (c->health <= 0)
			{
				if (INFINITE_HEALTH && c == player)
					c->health = 1000;
				else
					respawn(entities, c, players);
				get_entity_by_num(players, c2->team)->xp += 50;
			}

			if ((c->type != NET && c != player) || AIMBOT)
			{
				c->angle = angle_to_entity((c2 = find_closest_enemy(c, players, 800,
					                   entities)), c);

				if (c->weapon < SHOTGUN_2)
				{
					if (c2 == c)
						c->angle = angle_to_entity((c2 = find_closest_enemy(c, entities, maxx+maxy,
						                           entities)), c);
					if (c2 == c)
						c->angle = angle_to_entity(find_nearest_type(entities, c, LIFT,
						                                             DROP), c);
				}
				else
				{
					c->angle = angle_to_entity((c2 = find_nearest_type(entities, c, BASE, BASE)), c);
					if (c2 == c)
						c->angle = angle_to_entity(find_nearest_type(entities, c, LIFT,
						DROP), c);
				}


				/*c->thrust = (sqrt(pow((float)c->x - c2->x, 2) + pow((float)c->y - c2->y, 2))) / ((rand() % 10) + 10);
				if (c->thrust > max_thrust || c->thrust < 0)
				c->thrust = max_thrust;*/


				c->thrust = max_thrust; /* Maybe put this in init lol */


				c->t_angle = angle_to_entity((c2 = find_closest_enemy(c, players, 1000, entities)),
					c);

				if (c2 == c)
					c->t_angle = angle_to_entity((c2 = find_closest_enemy(c, ai, 1000,
					entities)), c);

				if (c2 == c)
					c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
					entities, 1000, entities)), c);

				c->cool_pos--;
				if (c2 != c)
					fire_weapon(bullet, c);
			}


			if (c->type == NET && c->firing)
				fire_weapon(bullet, c);

			update_entity_collision(entities, c);

			update_velocity(c);
			update_aura_v(entities, c);
			update_wall_aura_v(c);

			c->x = get_next_x(c);
			c->y = get_next_y(c);

			c->thrust *= slow;
			c->xv *= slow;
			c->yv *= slow;

			if (c->xp >= 100)
				level_up(c);
		}
		/* Move the ai lackeys */
		for (c = ai;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == TRASH)
				continue;



			if ((c2 = check_bullet_collision(bullet, c)) != NULL)
				c->health -= c2->damage;

			if (c->health <= 0)
			{
				respawn(entities, c, players);
				find_player(players, c2->team)->xp += 10;
			}


			set_squad_position(entities, c, find_player(players, c->team), ai);
			switch (c->mode)
			{
				default:
				case BEHIND:
					if (c->mode != TARGET)
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						ai, 1000, entities)), c);
					if (c2 == c && c->mode != TARGET)
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						players, 1000, entities)), c);
					if (c2 == c && c->mode != TARGET)
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						entities, 5000, entities)), c);

					c->thrust = (sqrt(pow((float)c->x - c2->x, 2) +
						pow((float)c->y - c2->y, 2))) / ((rand() % 10) + 10);
					if (c->thrust > max_thrust - 2)
						c->thrust = max_thrust;
					if (c2 != c && c->mode != TARGET)
						fire_weapon(bullet, c);
					break;
				case DEPLOY:
					c->t_angle = angle_to_entity((c2 = find_closest_enemy(c, ai,
						1000, entities)), c);
					if (c2 == c)
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						players, 1000, entities)), c);
					if (c2 == c)
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						entities, 1000, entities)), c);
					if (keyboard.space || keyboard.up)
						fire_weapon(bullet, c);
					c->thrust = 0;
					break;

			}
			c->cool_pos--;


			update_entity_collision(entities, c);


			update_velocity(c);
			update_aura_v(players, c);
			update_wall_aura_v(c);
			c->xv *= slow - 0.001;
			c->yv *= slow - 0.001;
			c->x = get_next_x(c);
			c->y = get_next_y(c);
		}
		/* Move the bulllets */
		for (c = bullet;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == TRASH)
				continue;


			bullet_damage = c->damage;
			for (c2 = entities;; c2 = c2->next)
			{
				if (c2 == NULL || c2->next == NULL)
					break;
				if (test_collision_square(c, c2) == OTHER)
				{
					if (c2->type == L_TURRET &&
						c->team == c2->team)
						continue;
					c->type = TRASH;
					if (c2->type == OUTPOST && c2->team != c->team &&
					    owns_outpost_level(entities, c->team, c2->id - 1))
					{
						c2->health -= bullet_damage;
						if (c2->health <= 0)
						{
							c2->health = outpost_health;
							c2->team = c->team;
							if (get_lackey_count(ai, c->team) <
							    find_player(players, c->team)->lackey_cap)
							{
								ai = add_ai(entities, ai, 2, c->team,
								c2->weapon, c2->id2);
								find_player(players,
								c->team)->lackey_count += 2;
							}
							find_player(players, c->team)->xp += 30;
							find_player(players, c->team)->scrap += 100;
						}
					}
					if (c2->type == BASE && c2->team != c->team)
					{
						c2->health -= bullet_damage;
						if (c2->health <= 0)
						{
							/*c2->type = TRASH;*/
							c2->team = c->team;
							c2->health = base_health;;
						}
					}
					if (c2->type == L_TURRET)
					{
						c2->health -= bullet_damage;
						if (c2->health <= 0)
						{
							c2->type = DEAD_OUTPOST;
							c2->team = -9;
						}
					}
					break;
				}
			}
			c->age++;
			if (c->age > 50)
				c->type = TRASH;

			update_velocity(c);
			c->x = get_next_x(c);
			c->y = get_next_y(c);
		}

		/* Loop through the entities looking for turrets */
		for (c = entities;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			switch (c->type)
			{
				case L_TURRET:
					c->cool_pos--;

					c->angle = angle_to_entity((c2 = find_closest_enemy(c, players,
						1000, entities)), c);
					c->t_angle = angle_to_entity((c2 = find_closest_enemy(c, players,
						1000, entities)), c);
					if (c2 == c)
					{
						c->angle = angle_to_entity((c2 = find_closest_enemy(c,
						ai, 1000, entities)), c);
						c->t_angle = angle_to_entity((c2 = find_closest_enemy(c,
						ai, 1000, entities)), c);
					}
					if (c2 != c)
						fire_weapon(bullet, c);
					break;
				default:
					break;
			}
		}


		/*
		* Draw
		*/

		/* Move the camera */
		camera.x = player->x - w / 2;
		camera.y = player->y - h / 2;

		SDL_SetRenderDrawColor(renderer, 23, 24, 26, 255);
		SDL_RenderClear(renderer);


		woff += player->xv;
		hoff += player->yv;


		while (woff > BOX_SPACE)
			woff -= BOX_SPACE;
		while (woff < 0)
			woff += BOX_SPACE;
		while (hoff > BOX_SPACE)
			hoff -= BOX_SPACE;
		while (hoff < 0)
			hoff += BOX_SPACE;


		set_colour(-5);
		for (i = 0; i < (w/BOX_SPACE)*4; i++)
			SDL_RenderDrawLine(renderer, i*BOX_SPACE-woff, 0, i*BOX_SPACE-woff, h);
		for (i = 0; i < (h/BOX_SPACE)*4; i++)
			SDL_RenderDrawLine(renderer, 0, i*BOX_SPACE-hoff, w, i*BOX_SPACE-hoff);





		for (c = players;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == TRASH || c->z != player->z)
				continue;
			/*draw_ship(c->x-camera.x, c->y-camera.y, c->angle, c->size,
			c->t_angle, c->health, c->weapon);*/
			if (c->x - camera.x > 0 && c->x - camera.x < w &&
				c->y - camera.y > 0 && c->y - camera.y < h)
			{
				set_colour(c->team);
				draw_ship(c->x - camera.x, c->y - camera.y, c->angle, c->size,
					c->t_angle, c->health, c->weapon);
			}
		}

		for (c = ai;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == TRASH || c->z != player->z)
				continue;
			set_colour(c->team);
			if (c->x - camera.x > 0 && c->x - camera.x < w &&
				c->y - camera.y > 0 && c->y - camera.y < h)
				draw_ship(c->x - camera.x, c->y - camera.y, c->angle, c->size,
				c->t_angle, c->health, c->weapon);
		}
		for (c = bullet;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->type == BUL && c->z == player->z)
			{
				if (c->x - camera.x > 0 && c->x - camera.x < w &&
					c->y - camera.y > 0 && c->y - camera.y < h)
				{
					set_colour(c->team);
					draw_bullet(c->x - camera.x, c->y - camera.y, c->angle, c->size);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 42, 200, 140, 255);
		draw_borders(camera.x, camera.y, maxx, maxy, wall_size);

		/* Loop through all the entities */
		for (c = entities;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if (c->z != player->z)
				continue;
			if (c->type == V_WALL)
			{
				SDL_SetRenderDrawColor(renderer, 42, 200, 140, 255);
				draw_v_wall(c->x - camera.x, c->size);
				continue;
			}
			if (c->type == H_WALL)
			{
				SDL_SetRenderDrawColor(renderer, 42, 200, 140, 255);
				draw_h_wall(c->y - camera.y, c->size);
				continue;
			}
			if (c->x + c->size > camera.x - w && c->x - c->size < camera.x + w &&
				c->y + c->size > camera.y - h && c->y - c->size < camera.y + h)
				switch (c->type)
			{
				case BASE:
					set_colour(c->team);
					draw_base(c->x - camera.x, c->y - camera.y, c->size,
						c->health);
					break;
				case OUTPOST:
					set_colour(c->team);
					draw_outpost(c->x - camera.x, c->y - camera.y, c->size,
						c->health);
					break;
				case LIFT:
				case DROP:
				case WALL:
					SDL_SetRenderDrawColor(renderer, 42, 200, 140, 255);
					if (c->type == LIFT || c->type == DROP)
						set_colour(c->team);
					draw_wall(c->x - camera.x, c->y - camera.y, c->size);
					break;
				case L_TURRET:
					set_colour(c->team);
					draw_l_turret(c->x - camera.x, c->y - camera.y,
						c->t_angle, c->size, c->health);
					break;
				default:
					break;
			}
		}

		/* Draw the XP */
		set_colour(-5);
		draw_hp(w / 2, h - 15, player->xp);

		/* Draw the baselines */
		if (keyboard.l)
		{
			for (c = entities;; c = c->next)
			{
				if (c == NULL || c->next == NULL)
					break;
				if (c->team == -1)
					continue;
				set_colour(c->team);
				switch (c->type)
				{
				default:
					c2 = find_nearest_ally_outpost(entities, c);
					SDL_RenderDrawLine(renderer,
						c->x + (c->size / 2) - camera.x,
						c->y + (c->size / 2) - camera.y,
						c2->x + (c2->size / 2) - camera.x,
						c2->y + (c2->size / 2) - camera.y);
				}
			}
		}
		/* Draw the buy menu */
		if (keyboard.e)
		{
			set_colour(-5);
			draw_buy_menu(player->x - camera.x, player->y - camera.y, player->size);
		}


		/* Draw the minimap */
		for (c = entities;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if ((!c->x && !c->y) ||
				c->type == TRASH ||
				c->x > maxx || c->y > maxy ||
				c->z != player->z)
				continue;
			set_colour(c->team);
			switch (c->type)
			{
				default:
					draw_wall((c->x - (c->size / 2)) / minimap_scale + 10,
						(c->y - (c->size / 2)) / minimap_scale + 10,
						minimap_size*1.4);

				/* Fix later */
				case LIFT:
				case DROP:
					draw_outpost((c->x - (c->size / 2)) / minimap_scale + 10,
						(c->y - (c->size / 2)) / minimap_scale + 10,
						minimap_size, 0);
					break;
				case H_WALL:
					rect.x = 10;
					rect.y = c->y / minimap_scale + 10;
					rect.h = minimap_size;
					rect.w = maxx / minimap_scale;
					SDL_RenderFillRect(renderer, &rect);
					break;
				case V_WALL:
					rect.x = c->x / minimap_scale + 10;
					rect.y = 10;
					rect.h = maxy / minimap_scale;
					rect.w = minimap_size;
					SDL_RenderFillRect(renderer, &rect);
					break;
			}
		}
		for (c = players;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if ((!c->x && !c->y) ||
				c->type == TRASH ||
				c->x > maxx || c->y > maxy ||
				c->z != player->z)
				continue;
			set_colour(c->team);
			switch (c->type)
			{

			default:
				draw_wall(c->x / minimap_scale + 10, c->y / minimap_scale + 10,
					minimap_size*1.25);
			}
		}
		for (c = ai;; c = c->next)
		{
			if (c == NULL || c->next == NULL)
				break;
			if ((!c->x && !c->y) ||
				c->type == TRASH ||
				c->x > maxx || c->y > maxy ||
				c->z != player->z)
				continue;
			set_colour(c->team);
			switch (c->type)
			{
				default:
					draw_wall(c->x / minimap_scale + 10, c->y / minimap_scale + 10,
						minimap_size);
			}
		}


		set_colour(-5);
		draw_wall(player->x / minimap_scale + 10, player->y / minimap_scale + 10,
			minimap_size);

		SDL_SetRenderDrawColor(renderer, 42, 200, 140, 255);

		rect.x = 10;
		rect.y = 10;
		rect.w = maxx / minimap_scale;
		rect.h = maxy / minimap_scale;
		SDL_RenderDrawRect(renderer, &rect);


		/* Draw some other UI elements */
		sprintf(buf, "Floor:   %d;", player->z+1);
		draw_text(10, h-250, buf, 160, 157, 138, 255);
		sprintf(buf, "Level:   %d;", player->weapon);
		draw_text(10, h-200, buf, 160, 157, 138, 255);
		sprintf(buf, "Lackeys: %d;", player->lackey_count);
		draw_text(10, h-150, buf, 160, 157, 138, 255);
		sprintf(buf, "Scrap:   %d;", player->scrap);
		draw_text(10, h-100, buf, 160, 157, 138, 255);
		sprintf(buf, "XP:      %d;", player->xp);
		draw_text(10, h-50, buf, 160, 157, 138, 255);


		SDL_RenderPresent(renderer);

		/*
		* Sleep
		*/
		if (frame_limit > (ticks = SDL_GetTicks()) + 16)
			SDL_Delay(16);
		else if (frame_limit > ticks)
			SDL_Delay(frame_limit - ticks);
		frame_limit = SDL_GetTicks() + 16;



	}

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
