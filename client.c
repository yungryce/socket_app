#define BUF_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char **argv);
void sock_error(const char *msg);

void sock_error(const char *msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char **argv)
{
	struct addrinfo hints, *res, *p;
	int sock_fd;
	char buffer[BUF_SIZE];

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <server_ip> <port_number>\n", argv[0]);
		exit(-1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(argv[1], argv[2], &hints, &res) != 0)
		sock_error("Error resolving host. Terminating...\n");

	for (p = res; p != NULL; p = p->ai_next)
	{
		sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_fd == -1)
			continue;

		if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock_fd);
			continue;
		}

		break;
	}

	if (p == NULL)
		sock_error("Failed to connect to server. Terminating...\n");

	freeaddrinfo(res);
	printf("Usage:\n");
	printf("<exit> to quit\n");
	printf("Or any other message to chat\n");

	while (1)
	{
		memset(buffer, 0, BUF_SIZE);
		fgets(buffer, BUF_SIZE, stdin);

		if (write(sock_fd, buffer, strlen(buffer)) == -1)
			sock_error("Error sending data. Terminating...\n");

		if (strcmp("exit\n", buffer) == 0)
			break;

		memset(buffer, 0, BUF_SIZE);
		if (read(sock_fd, buffer, BUF_SIZE) == -1)
			sock_error("Error receiving data. Terminating...\n");

		printf("Server: %s", buffer);
	}

	close(sock_fd);
	return (0);
}
