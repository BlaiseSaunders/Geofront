#include "theheader.h"
#include "proto.h"

#define BUF_SIZE 1024



char *get_value(char *string, double *value)
{
	int i;
	char buf[BUF_SIZE];

	for (i = 0; string[i] != ','; i++)
		buf[i] = string[i];
	buf[i] = '\0';
	*value = atof(buf);
	while (string++[0] != ',');
	return string;
}


void *get_in_addr(struct sockaddr *sa)
{
	return &(((struct sockaddr_in*)sa)->sin_addr);
}


void *update_players(void *params)
{
	int sock/*, sent*/, got, rv;
	char port[] = "2424";
	char s[INET6_ADDRSTRLEN];
	struct thread_params_s *args = params;
	struct addrinfo hints, *servinfo, *p;

	union player_net_dat player_dat;

	printf("Connecting to %s:%s.\n", args->ip, port);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	if ((rv = getaddrinfo(args->ip, port, &hints, &servinfo)) != 0)
	{
		printf("getaddrinfo: %s\n", gai_strerror(rv));
		return NULL;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sock = socket(p->ai_family, p->ai_socktype,
		                     p->ai_protocol)) == -1)
		{
			perror("Error: socket");
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock);
			perror("Error: connect");
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		printf("Failed to connect to host at %s:%s\n", args->ip, port);
		return NULL;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	freeaddrinfo(servinfo);

	printf("Connection established.\n");


	while (1) 
	{
		got = 0;
		while (1)
		{
			got = recv(sock, &player_dat.s+got, (sizeof player_dat)-1-got, 0);
			if (got >= (int)(sizeof player_dat)-1)
				break;
		}
		printf("Got: %d bytes\n", got);
		args->players->x = player_dat.p.x;
	}

}
