/**
 ****    HOST   ****
 * argv[0] - filename
 * argv[1] - port number
 ****    CLient   ****
 * argv[0] - filename
 * argv[1] - server ip address
 * argv[2] - port number
 */


#define BUF_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv);
void sock_error(const char *msg);

void sock_error(const char *msg)
{
	perror(msg);
	exit(-1);
}

int main(int argc, char **argv)
{
	struct sockaddr_in serve_addr, end_addr;
	socklen_t end_len;
	int sock_fd, new_sock_fd, port_no;
	char buffer[BUF_SIZE];

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
		exit(1);
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
		sock_error("Error opening socket\n");

	memset(&serve_addr, 0, sizeof(serve_addr));
	port_no = atoi(argv[1]);

	serve_addr.sin_family = AF_INET;
	serve_addr.sin_addr.s_addr = INADDR_ANY; /* IPv4 address */
	serve_addr.sin_port = htons(port_no); /* Port number */

	if (bind(sock_fd, (struct sockaddr *) &serve_addr, sizeof(serve_addr)) == -1)
		sock_error("Error binding. Terminating ...\n");

	listen(sock_fd, 5);
	end_len = sizeof(end_addr);

	new_sock_fd = accept(sock_fd, (struct sockaddr *) &end_addr, &end_len);
	if (new_sock_fd == -1)
		sock_error("Error accept connection. Terminating...\n");

	while (1)
	{
		memset(buffer, 0, BUF_SIZE);
		if (read(new_sock_fd, buffer, BUF_SIZE) == -1)
			sock_error("Error reading data. Terminating...\n");

		printf("Client: %s\n", buffer);



		memset(buffer, 0, BUF_SIZE);
		fgets(buffer, BUF_SIZE, stdin);

		if (write(new_sock_fd, buffer, strlen(buffer)) == -1)
			sock_error("Error printing data. Terminating...\n");

		if ((strcmp("exit", buffer)) == 0)
			break;
	}

	close(new_sock_fd);
	close(sock_fd);
	return (0);
}
