#include "Server.hpp"


int Server::getMaxClientsNumber()
{
	return _maxClientsNumber;
}

void Server::setPass(std::string pass)
{
	_password = pass;
}

void Server::setPort(int port)
{
	_portNumber = port;
}

void Server::setServerSock(int socket)
{
	_serverSock = socket;
}

std::string Server::getServerName()
{
	return _serverName;
}

int Server::requiredParams(Client &client)
{
	if (client.get_is_userF() && client.get_is_nickF() && client.get_is_passF())
		return (1);
	else
		return (0);
}

void Server::params_requirements(Client &client, int &clientSocket)
{
	std::string response;
	int bytes_read;

	if (!client.get_is_passF())
	{
		std::string response = ":" + this->getServerName() + " 464 " + client.get_nickname() + " :You must identify with a password before running commands\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	}
	else if (!client.get_is_nickF())
	{
		std::string response = ":" + this->getServerName() + " 431 " + client.get_nickname() + " :No nickname given. Use NICK command to set your nickname\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	}
	else
	{
		std::string response = ":" + this->getServerName() + " 451 * :No username given. Use USER command to set your username\r\n";
		bytes_read = send(clientSocket, response.c_str(), response.size(), 0);
	}
}

Server::Server(std::string password, int port)
{
	_password = password;
	_portNumber = port;
	_maxClientsNumber = 50;
	_connectedClients = 0;
	_serverName = "TIGERS";
	_pollFds = new struct pollfd[_maxClientsNumber];
}

int Server::CreateSocketConnection()
{
	int yes = 1;
	struct sockaddr_in addr;
	char serverIP[INET_ADDRSTRLEN];
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cerr << "Error/ninitializing the socket\n";
		return -1;
	}
	setServerSock(sockfd);
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_portNumber);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	inet_ntop(AF_INET, &(addr.sin_addr), serverIP, INET_ADDRSTRLEN);
	if (bind(sockfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cout << "Error/nbinding with the socket failed\n";
		close(sockfd);
		return -1;
	}
	return sockfd;
}

void Server::initClient()
{
	for (int i = 1; i < _maxClientsNumber; i++)
	{
		_pollFds[i].fd = 0;
		_pollFds[i].events = POLLIN;
		_pollFds[i].revents = 0;
	}
}

int Server::indexClient()
{
	int i;
	for (i = 1; i < _maxClientsNumber; i++)
	{
		if (_pollFds[i].fd == -1)
			return i;
	}
	for (i = 1; i < _maxClientsNumber; i++)
	{
		if (_pollFds[i].fd == 0)
			break;
	}
	return i;
}

void Server::ServerRun()
{
	initClient();
	std::cout << "Server is running\n";
	int flag = fcntl(_serverSock, F_GETFL, 0);
	fcntl(_serverSock, F_SETFL, flag | O_NONBLOCK);
	_pollFds[0].fd = _serverSock;
	_pollFds[0].events = POLLIN;
	_pollFds[0].revents = 0;
	int response = 0;
	int i = 1;
	while (1)
	{
		response = poll(_pollFds, _maxClientsNumber, -1);
		if (response == -1)
		{
			std::cout << "Error\npoll failed\n";
			return;
		}
		if (_pollFds[0].revents & POLLIN)
		{
			struct sockaddr_in addr;
			char clientIP[INET_ADDRSTRLEN];
			socklen_t len;

			len = sizeof(addr);
			int clientSocket = accept(_serverSock, (struct sockaddr *)&addr, &len);
			if (clientSocket < 0)
			{
				std::cout << "Error\naccepting the client socket failed\n";
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					return;
			}
			std::cout << "Client socket is:" << clientSocket << std::endl;
			i = indexClient();
			_pollFds[i].fd = clientSocket;
			_pollFds[i].events = POLLIN;
			_pollFds[i].revents = 0;
			_clients.insert(std::make_pair(_pollFds[i].fd, Client()));
			inet_ntop(AF_INET, &(addr.sin_addr), clientIP, INET_ADDRSTRLEN);
			_clients[_pollFds[i].fd].setClientIP(clientIP);
			int flag = fcntl(_pollFds[i].fd, F_GETFL, 0);
			fcntl(_pollFds[i].fd, F_SETFL, flag | O_NONBLOCK);
		}
		for (long unsigned int j = 1; j <= _clients.size(); j++)
		{
			if (_pollFds[j].fd == -1)
				continue;
			if (_pollFds[j].revents & POLLIN)
			{
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				response = recv(_pollFds[j].fd, buffer, sizeof(buffer), 0);
				if (response == -1)
				{
					std::cout << "Socket fd: " << _pollFds[j].fd << " is " << j << std::endl;
					std::cout << "Error\nrecv failed: " << response << std::endl;
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						continue;
					else
						return;
				}
				else if (response == 0)
				{
					std::cout << "Client " << _pollFds[j].fd << " disconnected from the server\n";
					_saveSemiCommands.erase(_pollFds[j].fd);
					_clients.erase(_pollFds[j].fd);
					close(_pollFds[j].fd);
					_pollFds[j].fd = -1;
					continue;
				}
				else
				{
					std::string receivedData(buffer, response);
					std::string &partialCommand = _saveSemiCommands[_pollFds[j].fd];
					partialCommand += receivedData;

					if (!partialCommand.empty())
					{
						size_t newlinePos = partialCommand.find('\n');
						while (newlinePos != std::string::npos)
						{
							_command = partialCommand.substr(0, newlinePos);
							partialCommand = partialCommand.substr(newlinePos + 1);
							newlinePos = partialCommand.find('\n');
							std::cout << _command << std::endl;
							if (_command == "\0")
								continue;
							if (_command.find('\r') != std::string::npos)
								executeAll(_clients[_pollFds[j].fd], _command.substr(0, _command.size() - 1), _pollFds[j].fd);
							else
								executeAll(_clients[_pollFds[j].fd], _command, _pollFds[j].fd);
						}
					}
				}
			}
		}
	}
}

void Server::eraseAllClients()
{
	_channels.clear();
	for (long unsigned int j = 1; j <= _clients.size(); j++)
	{
		if (_pollFds[j].fd != -1 && _pollFds[j].fd != 0)
			close(_pollFds[j].fd);
	}
	_clients.clear();
	close(_pollFds[0].fd);
	exit(0);
}
