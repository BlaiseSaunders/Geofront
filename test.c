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
