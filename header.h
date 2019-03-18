/* Header for Linux C edition */

#ifndef SCREEN_WIDTH

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	1000


#define BOX_SPACE 400

/* #define STRICT */
#endif



typedef enum
{
	false,
	true
} bool;

typedef enum
{
	RELEASED,
	PRESSED
} key_state;

struct keyboard_s
{
	key_state w, a, s, d, space,
		left, right, up, down,
		one, two, three, four,
		l, m, tab, e;
};

struct cam_s
{
	int x, y;
	float xv, yv;
};

typedef enum
{
	MANUAL,
	AUTO
} input_t;


typedef enum
{
	NONE,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	CORNER,
	OTHER
} side;


typedef enum
{
	WALL,
	BASE,
	OUTPOST,
	LIFT,
	DROP,
	H_WALL,
	V_WALL,
	R_OUTPOST,
	F_AI,
	P_AI,
	S_AI,
	BUL,
	PLAYER,
	L_TURRET,
	H_TURRET,
	SLOW_TURRET,
	HEALER,
	NAE,
	DEAD_OUTPOST,
	NET,
	TRASH
} e_type;

typedef enum
{
	BASE_1,
	BASE_2,
	SHOTGUN_1,
	SHOTGUN_2,
	MISSILE_1,
	MISSILE_2,
	CRESCENT_1,
	CRESCENT_2,
	SEMEN
} e_form;

typedef enum
{
	BEHIND,
	SIDE,
	FRONT,
	TARGET,
	DEPLOY
} e_mode;

typedef enum
{
	NO,
	LIFTING,
	DROPPING
} e_anim;


typedef enum
{
	AGGRESSIVE,
	DEFENSIVE
} persona;



typedef struct entity
{
	int x, y, z;
	int size;
	float xv, yv, thrust;
	float angle;
	float t_angle;
	int team;
	int health;
	int age;
	int cool_pos;
	int shot_pos;
	int anim_pos;
	int damage;

	int lackey_cap;
	unsigned int lackey_count;

	unsigned int xp;
	unsigned int scrap;

	e_type type;
	e_form weapon;
	e_mode mode;
	e_anim anim;
	persona personality;

	bool firing;

	int id;
	int id2;

	struct entity *next;
} entity;




struct thread_params_s
{
	entity *players;
	char ip[1024];
};


struct serial_params_s
{
	float *ypr;
	bool *firing;
};


void cleanup();

entity *load_entities(char *dat_file);


void set_colour(int colour);

void draw_image(SDL_Surface *image, int x, int y);
void draw_ship_tri(int x, int y, float angle, int size,
	float t_angle, int health);
void draw_ship_w(int x, int y, float angle, int size,
	int health);
void draw_ship_cres(int x, int y, float angle, int size,
	int health);
void draw_wall(int x, int y, int s);
void draw_outpost(int x, int y, int size, int health);
void draw_base(int x, int y, int size, int health);
void draw_borders(int x, int y, int maxx, int maxy, int size);
void draw_bullet(int x, int y, float angle, int size);
void draw_hp(int x, int y, int hp);
void draw_ship(int x, int y, float angle, int size,
	float t_angle, int health, int weapon);
void draw_l_turret(int x, int y, float t_angle, int size,
	int health);
void draw_buy_menu(int x, int y, int size);

side test_collision_square(entity *s1, entity *s2);
side test_collision_border(entity *e);

int get_input(entity *player);

void fire_weapon(entity *bullet, entity *e);
void fire_bullet(entity *bullet, entity *e, int x, int y,
	float angle, int damage);

entity *get_entity_by_num(entity *e, int num);
int get_ai_num(entity *e, entity *main);

entity *add_ai(entity *entities, entity *ai, int count,
	int team, e_form form, int id);
entity *add_player(entity *entities, entity *ai, int count);

float angle_to_entity(entity *e1, entity *e2);
entity *check_bullet_collision(entity *bullets, entity *e);
entity *find_closest_enemy(entity *e, entity *enemy,
	int range, entity *entities);
entity *find_respawn_point(entity *entities, entity *e);
entity *find_player(entity *players, int team);
entity *find_nearest_ally_outpost(entity *entities, entity *e);

int get_next_x(entity *e);
int get_next_y(entity *e);

void set_ai_mode(entity *ai, e_mode mode, int team);

bool owns_outpost_level(entity *entities, int team, int id);

void level_up(entity *player);
int get_level_health(e_form level, e_type type);

void update_velocity(entity *e);
void update_aura_v(entity *entities, entity *e);
void update_wall_aura_v(entity *e);

void respawn(entity *entities, entity *e, entity *players);

void set_squad_position(entity *entities, entity *e,
	entity*player, entity *ai);
entity *find_nearest_type(entity *entities, entity *e,
	e_type type, e_type type2);

int get_lackey_count(entity *ai, int team);

void update_entity_collision(entity *entities, entity *player);

void draw_h_wall(int y, int size);
void draw_v_wall(int x, int size);

bool wall_in_way(entity *e1, entity *e2, entity *entities);

entity *add_turret(entity *entities, entity *e, e_type type);

bool close_to_outpost(entity *entities, entity *e);

void draw_text(int x, int y, char *string, int r, int g, int b, int a);

char *show_main_menu();

bool owns_outpost(entity *entities, int team, int id2);





void *update_players(void *args);

void *read_serial(void *args);
