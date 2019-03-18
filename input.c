#include "theheader.h"

extern SDL_Window *screen;

int get_input(entity *player)
{
	int x, y, w, h;
	extern SDL_Event event;
	extern struct keyboard_s keyboard;
	/*
	* Get Input
	*/
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_MOUSEMOTION:
				/*if (1)
					break;*/
				SDL_GetMouseState(&x, &y);
				SDL_GetWindowSize(screen, &w, &h);
				player->t_angle = -(atan2(x - (w / 2), y - (h / 2)) + M_PI / 2 * 3);
				if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
					keyboard.space = PRESSED;
				else
					keyboard.space = RELEASED;
				break;
			case SDL_QUIT:
				cleanup();
				return 1;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_e:
						keyboard.e = PRESSED;
						break;
					case SDLK_TAB:
						keyboard.tab = PRESSED;
						break;
					case SDLK_w:
						keyboard.w = PRESSED;
						break;
					case SDLK_a:
						keyboard.a = PRESSED;
						break;
					case SDLK_s:
						keyboard.s = PRESSED;
						break;
					case SDLK_d:
						keyboard.d = PRESSED;
						break;
					case SDLK_l:
						keyboard.l = PRESSED;
						break;
					case SDLK_UP:
						keyboard.up = PRESSED;
						break;
					case SDLK_DOWN:
						keyboard.down = PRESSED;
						break;
					case SDLK_LEFT:
						keyboard.left = PRESSED;
						break;
					case SDLK_RIGHT:
						keyboard.right = PRESSED;
						break;
					case SDLK_SPACE:
						keyboard.space = PRESSED;
						break;
					case SDLK_1:
						keyboard.one = PRESSED;
						break;
					case SDLK_2:
						keyboard.two = PRESSED;
						break;
					case SDLK_3:
						keyboard.three = PRESSED;
						break;
					case SDLK_4:
						keyboard.four = PRESSED;
						break;
					default:
						break;
				}
				break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case SDLK_e:
					keyboard.e = RELEASED;
					break;
				case SDLK_TAB:
					keyboard.tab = RELEASED;
					break;
				case SDLK_w:
					keyboard.w = RELEASED;
					break;
				case SDLK_a:
					keyboard.a = RELEASED;
					break;
				case SDLK_s:
					keyboard.s = RELEASED;
					break;
				case SDLK_d:
					keyboard.d = RELEASED;
					break;
				case SDLK_l:
					keyboard.l = RELEASED;
					break;
				case SDLK_UP:
					keyboard.up = RELEASED;
					break;
				case SDLK_DOWN:
					keyboard.down = RELEASED;
					break;
				case SDLK_LEFT:
					keyboard.left = RELEASED;
					break;
				case SDLK_RIGHT:
					keyboard.right = RELEASED;
					break;
				case SDLK_SPACE:
					keyboard.space = RELEASED;
					break;
				case SDLK_1:
					keyboard.one = RELEASED;
					break;
				case SDLK_2:
					keyboard.two = RELEASED;
					break;
				case SDLK_3:
					keyboard.three = RELEASED;
					break;
				case SDLK_4:
					keyboard.four = RELEASED;
					break;
				case SDLK_q:
					cleanup();
					return 1;
				default:
					break;
			}
			break;
			case SDL_CONTROLLERBUTTONDOWN:
				switch (event.cbutton.button)
				{
					case SDL_CONTROLLER_BUTTON_A:
						keyboard.down = PRESSED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
						keyboard.d = PRESSED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
						keyboard.a = PRESSED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_UP:
						keyboard.w = PRESSED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
						keyboard.s = PRESSED;
						break;
				}
				break;
			case SDL_CONTROLLERBUTTONUP:
				switch (event.cbutton.button)
				{
					case SDL_CONTROLLER_BUTTON_A:
						keyboard.down = RELEASED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
						keyboard.d = RELEASED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
						keyboard.a = RELEASED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_UP:
						keyboard.w = RELEASED;
						break;
					case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
						keyboard.s = RELEASED;
						break;
				}
				break;
			/*case SDL_JOYAXISMOTION:
				if ((event.jaxis.value < -3200) || (event.jaxis.value > 3200))
				{
					 x axis
					if (event.jaxis.axis == 0)
					{
						keyboard.w = PRESSED;
					}

					 y axis
					if (event.jaxis.axis == 1)
					{
						keyboard.d = PRESSED;
					}
				}
				break;
		*/}
	}
	/*fprintf(fp, "%d %d: %d\n", event.jaxis.axis, event.jaxis.which, event.jaxis.value);*/
	return 0;
}



int read_port(char *device)
{
    int fd = open(device, O_RDONLY | O_NOCTTY);
    if (fd == -1)
    {
        perror("open_port: Unable to open serial device - ");
        return -1;
    }

    return fd;
}

void *read_serial(void *params)
{
	int i, fd, am_read;
	char buf[2][256], *pos, *start;
	float *ypr; /* Yaw, Pitch, Roll */
	struct serial_params_s *args = params;
	bool *firing = args->firing;

	ypr = args->ypr;

	fd = read_port("/dev/ttyACM0");


	while (1)
	{
		usleep(1000);
		buf[0][0] = buf[1][0] = '\0';
		while (strstr(buf[0], "\n") == NULL)
		{
			am_read = read(fd, buf[1], 1);
			if (am_read > 0)
				strcat(buf[0], buf[1]);
		}


		start = buf[0];
		/*printf("%s", start);*/
		for (i = 0; i < 5; i++)
		{
			if (start[0] == 'p')
				*firing = true;
			else
				*firing = false;
			if ((pos = strstr(start, "\t")) == NULL)
				break;
			pos[0] = '\0';
			ypr[i] = atof(start);
			start = &pos[1];
		}

		/*printf("Yaw: %.2f; Pitch: %.2f; Roll: %.2f;\n", ypr[0],
		                                                ypr[1], ypr[2]);*/
	}


	close(fd);
	return 0;
}
