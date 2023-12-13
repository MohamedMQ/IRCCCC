#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <map>
#include <vector>
#include <arpa/inet.h>

void bot_client() {
	std::string _saveSemiCommands;
	std::string _command;
	int a = 0;
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "failed to connect to the server\n";
		exit(-1);
	} else {
		std::cout << "the client got connected to the server correctly\n";
	}
	int response = 0;
	while (1) {
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			response = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (response == -1) {
				std::cout << "Error\nrecv failed: " << response << std::endl;
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					return;
				} else if (response == 0) {
					std::cout << "the server is closed for now, try later\n";
					close(clientSocket);
					return;
				} else {
					std::string receivedData(buffer, response);
					std::string &partialCommand = _saveSemiCommands;
					partialCommand += receivedData;

					if (!partialCommand.empty()) {
						size_t newlinePos = partialCommand.find('\n');
						while (newlinePos != std::string::npos) {
							_command = partialCommand.substr(0, newlinePos);
							std::cout << _command << std::endl;
							partialCommand = partialCommand.substr(newlinePos + 1);
							newlinePos = partialCommand.find('\n');
							if (!a) {
								a = 1;
								send(clientSocket, "PASS popo\r\n", strlen("PASS popo\r\n"), 0);
								send(clientSocket, "NICK BOT\r\n", strlen("NICK BOT\r\n"), 0);
								send(clientSocket, "USER BOT 0 * BOT\r\n", strlen("USER BOT 0 * BOT\r\n"), 0);
							}
						}
					}
				}
	}
}