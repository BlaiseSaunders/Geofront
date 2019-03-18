#include "theheader.h"

extern SDL_Window *screen;
extern SDL_Renderer *renderer;


char *show_main_menu()
{
	int entries = 0;
	bool go = true;
	int w, h, i;
	int selection = 0;
	unsigned int frame_limit, ticks;
	int entry;
	DIR *dp;
	char *buf = (char *)malloc(sizeof (char)*2048);
	struct dirent *dent;
	extern SDL_Event event;
	extern int AIMBOT;

	typedef struct map_list_s
	{
		char map[1024];
		struct map_list_s *next;
	} map_list_s;

	map_list_s *map_list, *c;

	map_list = (map_list_s *)malloc(sizeof (map_list_s));
	map_list->next = NULL;
	if ((dp = opendir("maps/")) == NULL)
		printf("Couldn't open the maps folder.\n");

	c = map_list;
	while ((dent = readdir(dp)) != NULL)
		if (strstr(dent->d_name, ".dat") != NULL)
		{
			strcpy(c->map, dent->d_name);
			entries++;
			c->next = (map_list_s *)malloc(sizeof (map_list_s));
			c->next->next = NULL;
			c = c->next;
		}


	frame_limit = SDL_GetTicks() + 16;
	while (go)
	{

		SDL_GetWindowSize(screen, &w, &h);

		while (SDL_PollEvent(&event))
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_q:
						case SDL_QUIT:
							return NULL;
						case SDLK_SPACE:
						case SDLK_RETURN:
							if (selection < entries)
								for (c = map_list, entry = 0;; c = c->next, entry++)
								{
									if (c == NULL)
										break;
									if (selection == entry)
									{
										sprintf(buf, "maps/%s", c->map);
										return &buf[0];
									}
								}
							else if (selection == entries)
							{
								if (AIMBOT)
									AIMBOT = 0;
								else
									AIMBOT = 1;
							}
							break;
						case SDLK_UP:
							if (selection > 0)
								selection--;
							break;
						case SDLK_DOWN:
							if (selection < entries)
								selection++;
							break;
					}
					break;

			}


		/*
		 * Draw
		 */

		SDL_SetRenderDrawColor(renderer, 23, 24, 26, 255);
		SDL_RenderClear(renderer);

		set_colour(0);

		sprintf(buf, "Welcome");
		draw_text(w/2-(strlen(buf)*24)/2, 20, buf, 119, 190, 119, 255);

		for (c = map_list, entry = 0;; c = c->next, entry++)
		{
			if (c == NULL || c->next == NULL)
				break;

			sprintf(buf, "%s", c->map);

			for (i = 0; buf[i] != '.'; i++);
			buf[i] = '\0';

			if (entry == selection)
				draw_text(w/2-(strlen(buf)*24)/2, 50*entry+100+h/2-entries*50, buf, 160, 157, 138, 255);
			else
				draw_text(w/2-(strlen(buf)*24)/2, 50*entry+100+h/2-entries*50, buf, 207, 153, 17, 255);
		}

		if (AIMBOT)
			sprintf(buf, "Aimbot On");
		else
			sprintf(buf, "Aimbot Off");

		if (entry == selection)
			draw_text(w/2-(strlen(buf)*24)/2, 50*entry+100+h/2-entries*50+50, buf, 160, 157, 138, 255);
		else
			draw_text(w/2-(strlen(buf)*24)/2, 50*entry+100+h/2-entries*50+50, buf, 207, 153, 17, 255);


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

	return "map.dat";
}
