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

int main(int ac, char **av) {
	std::string _saveSemiCommands;
	struct sockaddr_in serverAddr;
	std::string _command;
	struct pollfd *_pollFd;
	int clientSocket;
	char buffer[1024];
	int flag;
	int a = 0;

	std::memset(&serverAddr, 0, sizeof(serverAddr));
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "Error\nfailed connecting to the server\n";
		exit (-1);
	} else
		std::cout << "Success\nsuccessed connecting to the server\n";
	flag = fcntl(clientSocket, F_GETFL, 0);
	fcntl(clientSocket, F_SETFL, flag | O_NONBLOCK);
	_pollFd = new struct pollfd;
	_pollFd->fd = clientSocket;
	_pollFd->events = POLLIN;
	_pollFd->revents = 0;
	int response = 0;
	while (1) {
		response = poll(_pollFd, 1, -1);
		if (response == -1) {
			std::cout << "Error\npoll failed\n";
			return -1;
		}
		if (_pollFd->revents & POLLIN) {
			memset(buffer, 0, sizeof(buffer));
			response = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (response == -1) {
				std::cout << "Error\nrecv failed: " << response << std::endl;
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					return -1;
				} else if (response == 0) {
					std::cout << "the server is closed for now, try later\n";
					free(_pollFd);
					close(clientSocket);
					return -1;
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
	return 0;
}