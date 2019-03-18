struct player_dat
{
	int x;
	int y;
	float thrust;
	bool firing;
};

union player_net_dat
{
	struct player_dat p;
	char s[sizeof (struct player_dat)];

};
