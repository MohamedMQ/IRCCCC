#include "irc.hpp"

class Server
{
private:
	std::map<int, std::string> _saveSemiCommands;
	std::string _command;
	std::string _password;
	int _portNumber;
	int _serverSock;
	int _maxClientsNumber;
	std::string _serverName;
	struct pollfd *_pollFds;
	std::map<int, Client> _clients;
	std::vector<Channel> _channels;

public:
	Server(std::string password, int port)
	{
		_password = password;
		_portNumber = port;
		_maxClientsNumber = 50;
		_serverName = "TIGERS";
		_pollFds = new struct pollfd[_maxClientsNumber];
	}

	int getMaxClientsNumber()
	{
		return _maxClientsNumber;
	}

	void setPass(std::string pass)
	{
		_password = pass;
	}

	void setPort(int port)
	{
		_portNumber = port;
	}

	void setServerSock(int socket)
	{
		_serverSock = socket;
	}

	std::string getServerName()
	{
		return _serverName;
	}

	int requiredParams(Client &client)
	{
		if (client.get_is_userF() && client.get_is_nickF() && client.get_is_passF())
			return (1);
		else
			return (0);
	}

	void params_requirements(Client &client, int &clientSocket)
	{
		std::string response;
		int bytes_read;

		if (!client.get_is_passF()) {
			std::string response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " PASS :Password required\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		} else if (!client.get_is_nickF()) {
			std::string response = ":" + this->getServerName() + " 431 " + client.get_nickname() + " :No nickname given\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		} else {
			std::string response = ":" + this->getServerName() + " 431 * :No username given\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}

	void executeAll(Client &client, std::string buffer, int &clientSocket, std::string _password)
	{
		if (!std::strcmp((buffer.substr(0, 4)).c_str(), "USER") && client.get_is_passF())
			user_command(buffer, client, clientSocket);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "NICK") && client.get_is_passF())
			nickname_command(buffer, client, clientSocket);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "PASS"))
			pass_command(client, buffer, clientSocket);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "QUIT") && client.get_is_passF())
		{
			if (quit_command(clientSocket, buffer) == -1)
				return;
		}
		else if (!std::strcmp((buffer.substr(0, 7)).c_str(), "PRIVMSG") && requiredParams(client))
			privmsg_command(client, buffer);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "JOIN") && requiredParams(client))
			join_command(client, buffer);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "PART") && requiredParams(client))
			part_command(client, buffer);
		else if (!std::strcmp((buffer.substr(0, 6)).c_str(), "INVITE") && requiredParams(client))
			invite_command(client, buffer);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "MODE") && requiredParams(client))
			mode_command(client, buffer);
		else if (!std::strcmp((buffer.substr(0, 5)).c_str(), "TOPIC") && requiredParams(client))
			topic_command(buffer);
		else if (!std::strcmp((buffer.substr(0, 4)).c_str(), "KICK") && requiredParams(client))
			kick_command(client, buffer);
		else if (!requiredParams(client))
			params_requirements(client, clientSocket);
		else {
			std::string response = ":" + this->getServerName() + " 421 " + client.get_nickname() + " UNKNOWN_COMMAND :Unknown command\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}

	int CreateSocketConnection()
	{
		int yes = 1;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			std::cout << "Error/ninitializing the socket\n";
			return -1;
		}
		setServerSock(sockfd);
		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(_portNumber);
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(sockfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
		{
			std::cout << "Error/nbinding with the socket failed\n";
			close(sockfd);
			return -1;
		}
		return sockfd;
	}

	void initClient()
	{
		for (int i = 1; i < _maxClientsNumber; i++)
		{
			_pollFds[i].fd = 0;
			_pollFds[i].events = POLLIN;
			_pollFds[i].revents = 0;
		}
	}

	int indexClient()
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

	void ServerRun()
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
			if (_pollFds[0].revents && POLLIN)
			{
				int clientSocket = accept(_serverSock, NULL, NULL);
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
				// Client *client =
				// Client client;
				_clients.insert(std::make_pair(_pollFds[i].fd, Client()));
				// _clients.push_back(clientSocket);
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
						std::cout << "Socket fd: " << _pollFds[j].fd << " J is " << j << std::endl;
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
								std::cout << _command << std::endl;
								if (_command.find('\r') != std::string::npos)
									executeAll(_clients[_pollFds[j].fd], _command.substr(0, _command.size() - 1), _pollFds[j].fd, _password);
								else
									executeAll(_clients[_pollFds[j].fd], _command, _pollFds[j].fd, _password);
								partialCommand = partialCommand.substr(newlinePos + 1);
								newlinePos = partialCommand.find('\n');
							}
						}
					}
				}
			}
		}
	}

	void fill_client(std::string command, Client &client)
	{
		char *str;
		int i = 3;
		std::vector<std::string> tokens;
		std::string realname;
		str = strtok((char *)(command.c_str() + 4), " ");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		client.set_username(tokens[0]);
		while (i < tokens.size())
		{
			realname += tokens[i];
			if (i < tokens.size() - 1)
				realname += " ";
			i++;
		}
		client.set_real_name(realname);
	}

	int pars_user_command(std::string command)
	{
		char *str;
		int count = 0;
		str = strtok((char *)(command.c_str() + 4), " ");
		while (str != NULL)
		{
			count++;
			str = strtok(NULL, " ");
		}
		if (count < 4)
		{
			std::cerr << "Error(461): USER Not enough parameters" << std::endl;
			return 0;
		}
		return 1;
	}

	void user_command(std::string _command, Client &client, int &socket)
	{
		if (client.get_is_userF() == 0)
		{
			if (!pars_user_command(_command))
				return;
			fill_client(_command, client);
			client.set_is_userF(1);
		}
	}

	////////////////////////////////////////pass_command////////////////////////////////////////////////////////////////

	void pass_command(Client &client, std::string _command, int &socket)
	{
		std::string response;
		int bytes_sent;

		if (_command.substr(0, 4) == "PASS")
		{
			if (client.get_is_passF() == 0)
			{
				if (!std::strcmp(_command.c_str() + 5, _password.c_str()))
				{
					response = ":" + this->getServerName() + " 001 " + _clients[socket].get_nickname() + " :Welcome to the IRC Network " + _clients[socket].get_nickname() + "!" + _clients[socket].get_username() + "@" + this->getServerName() + "\r\n";
					bytes_sent = send(socket, response.c_str(), response.size(), 0);
					client.set_is_passF(1);
					// std::cout << "You are now authentified" << std::endl;
					return;
				}
				else
				{
					response = ":" + this->getServerName() + " 464 " + client.get_nickname() + " :Password incorrect\r\n";
					bytes_sent = send(socket, response.c_str(), response.size(), 0);
					client.set__retry_pass();
					// std::cout << "Wrong Password" << std::endl;
				}
			}
			else
			{
				response = ":" + this->getServerName() + " 462 " + client.get_nickname() + " :You may not reregister\r\n";
				bytes_sent = send(socket, response.c_str(), response.size(), 0);
				// std::cerr << "safi ghiyerha " << std::endl;
			}
			if (client.get__retry_pass() == 3)
			{
				response = ":" + this->getServerName() + " 465 " + client.get_nickname() + " :Too many incorrect password attempts. You are being kicked out.\r\n";
				bytes_sent = send(socket, response.c_str(), response.size(), 0);
				close(socket);
				_clients.erase(socket);
				socket = -1;
				// std::cout << "Client " << socket << " disconnected from the server\n";
				return;
			}
		}
	}

	/////////////////////////////////////////quit_command///////////////////////////////////////////////////////////////

	int quit_command(int &socket, std::string command)
	{
		if (!std::strcmp((command.substr(0, 4)).c_str(), "QUIT"))
		{
			std::cout << "Client disconnected 2" << std::endl;
			close(socket);
			_clients.erase(socket);
			socket = -1;
			return -1;
		}
		return 0;
	}

	//////////////////////////////////////nickname_command//////////////////////////////////////////////////////////////////

	int pars_nickname(std::string nickname)
	{
		int i = 0;
		if (!((nickname[i] >= 'a' && nickname[i] <= 'z') || (nickname[i] >= 'A' && nickname[i] <= 'Z') || nickname[i] == '_'))
		{
			return 0;
		}
		for (int i = 0; i < nickname.size(); i++)
		{
			if ((nickname[i] >= 'a' && nickname[i] <= 'z') || (nickname[i] >= 'A' && nickname[i] <= 'Z') || (nickname[i] >= '1' && nickname[i] <= '9') || nickname[i] == '_')
				continue;
			else
				return 0;
		}
		return 1;
	}

	// void nickname_command(std::string buffer, Client &client, int &clientSocket) {
	// 	char *str;
	// 	std::map<int, Client>::iterator ptr;
	// 	// std::cout << "9beeel :" << client.get_nickname() << std::endl;
	// 	if(!std::strcmp((buffer.substr(0,4)).c_str(), "NICK")) {
	// 		std::vector<std::string> tokens;
	// 		str = strtok((char *)(buffer.c_str()), " \r");
	// 		while (str != NULL) {
	// 			tokens.push_back(str);
	// 			str = strtok (NULL, " \r");
	// 		}
	// 		if (tokens.size() == 1)
	// 			std::cerr << "Error(431): No nickname given" << std::endl;
	// 		else if (tokens.size() >= 2) {
	// 			for (ptr = _clients.begin() ; ptr != _clients.end() ; ptr++) {
	// 				if((*ptr).second.get_nickname() == tokens[1]) {
	// 					std::string response = ":" + this->getServerName() + " 433 " + client.get_nickname() + " " + tokens[1] + " :Nickname is already in use\r\n";
	// 					int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	// 					// std::cerr << "Error(433): "<< (*ptr).second.get_nickname() << " Nickname is already in use" << std::endl;
	// 					return;
	// 				}
	// 			}
	// 			if (!pars_nickname(tokens[1])) {
	// 				std::string response = ":" + this->getServerName() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
	// 				int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	// 				return ;
	// 			}
	// 			std::string response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " NICK :" + tokens[1] + "\r\n";
	// 			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	// 			client.set_nickname(tokens[1]);
	// 			client.set_is_nickF(1);
	// 		}
	// 	}
	// }

	void nickname_command(std::string buffer, Client &client, int &clientSocket)
	{
		char *str;
		std::map<int, Client>::iterator ptr;
		std::vector<std::string> tokens;
		// std::cout << "NICK 1\n";
		if (buffer.find(":") == 5 && !client.get_is_nickF())
		{
			// std::cout << "NICK 2\n";
			std::string response = ":" + this->getServerName() + " 432 " + "*" + " :Erroneous Nickname\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			close(clientSocket);
			_clients.erase(clientSocket);
			clientSocket = -1;
		}
		else if (!client.get_is_nickF())
		{
			// std::cout << "NICK 3\n";
			str = strtok((char *)(buffer.c_str()), " \r");
			while (str != NULL)
			{
				tokens.push_back(str);
				str = strtok(NULL, " \r");
			}
			std::string s(tokens[1]);
			// std::cout << "NICK 4\n";
			if (!pars_nickname(s))
			{
				std::string response = ":" + this->getServerName() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
				int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				close(clientSocket);
				_clients.erase(clientSocket);
				clientSocket = -1;
				return;
			}
			if (check_if_client_exist(s)) {
				std::string response = ":" + this->getServerName() + " 433 " + client.get_nickname() + " " + s + " :Nickname is already in use\r\n";
				int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				close(clientSocket);
				_clients.erase(clientSocket);
				clientSocket = -1;
				return;
				// client.set_nickname(s);
			}
			client.set_nickname(s);
			std::string response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " NICK :" + s + "\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			client.set_is_nickF(1);
			// std::cout << "NICK 5\n";
		}
		else
		{
			str = strtok((char *)(buffer.c_str()), " \r");
			while (str != NULL)
			{
				tokens.push_back(str);
				str = strtok(NULL, " \r");
			}
			if (tokens.size() == 0)
				std::cerr << "Error(431): No nickname given" << std::endl;
			else if (tokens.size() >= 2)
			{
				for (ptr = _clients.begin(); ptr != _clients.end(); ptr++)
				{
					if ((*ptr).second.get_nickname() == tokens[1])
					{
						std::string response = ":" + this->getServerName() + " 433 " + client.get_nickname() + " " + tokens[1] + " :Nickname is already in use\r\n";
						int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						return;
					}
				}
				if (!pars_nickname(tokens[1]))
				{
					std::string response = ":" + this->getServerName() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
					int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
				std::string response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " NICK :" + tokens[1] + "\r\n";
				int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				client.set_nickname(tokens[1]);
				client.set_is_nickF(1);
				return;
			}
		}
	}

	///////////////////////////////////////privmsg_command/////////////////////////////////////////////////////////////////

	int check_channel_if_exist(std::string to_check)
	{
		int flag = 0;
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == to_check)
				flag = 1337;
		}
		return flag;
	}

	void privmsg_command(Client &client, std::string command)
	{
		char *str;
		int flag = 0;
		int j = 2;
		std::string message;
		std::vector<std::string> tokens;
		std::map<int, Client>::iterator iter;
		str = strtok((char *)(command.c_str()), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (tokens.size() < 3)
			std::cerr << "Not enough paramters" << std::endl;
		else
		{
			for (iter = _clients.begin(); iter != _clients.end(); iter++)
			{
				if ((*iter).second.get_nickname() == tokens[1])
					flag = 1337;
				else if (check_channel_if_exist(tokens[1]))
					flag = 42;
			}
			if (flag == 1337)
			{
				for (iter = _clients.begin(); iter != _clients.end(); iter++)
				{
					if ((*iter).second.get_nickname() == tokens[1])
					{
						int pos = command.find(tokens[2]);
						message = command.substr(pos);
						(*iter).second.set_private_message((*iter).second.get_nickname(), message);
					}
				}
			}
			else if (flag == 42)
			{
				for (int i = 0; i < _channels.size(); i++)
				{
					if (_channels[i].get_name() == tokens[1])
					{
						int pos = command.find(tokens[2]);
						message = command.substr(pos);
						_channels[i].add_message(client.get_nickname(), message);
						break;
					}
				}
			}
			else
				std::cerr << "No such user with this name" << std::endl;
		}
	}

	//////////////////////////join_command//////////////////////////////////////////////////////////////////////////////

	void add_channel_to_client(Client &client, std::string to_check)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == to_check)
			{
				client.add_channel(_channels[i], 0);
				break;
			}
		}
	}

	void add_client_to_channel(Client &client, std::string to_check)
	{
		int a;
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == to_check)
			{
				_channels[i].add_client(client);
				break;
			}
		}
	}

	int check_if_invite_only_channel(std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (token == _channels[i].get_name())
			{
				ch_modes ch;
				ch = _channels[i].get_modes();
				if (ch.i == 1)
					return 1;
				break;
			}
		}
		return 0;
	}

	int check_if_client_already_joined(Client &client, std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (token == _channels[i].get_name())
			{
				if (client.if_element_exist(_channels[i]))
					return 1;
				break;
			}
		}
		return 0;
	}

	void join_command(Client &client, std::string command)
	{
		char *str;
		char *str2;
		int i = 0;
		int flag = 0;
		std::vector<char *> tokens;
		std::vector<char *> tokens2;
		std::cout << "size 9bel :" << _channels.size() << std::endl;
		str = strtok((char *)(command.c_str() + 5), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (tokens.size() != 1)
		{
			// split it to take the first one
			str2 = strtok((char *)(command.c_str() + 5), " \r");
			while (str2 != NULL)
			{
				tokens2.push_back(str2);
				str2 = strtok(NULL, " \r");
			}
			// check if this channel is already exist
			if (check_channel_if_exist(tokens2[0]))
			{
				// channel already exist
				if (check_if_client_already_joined(client, tokens2[0]))
					return;
				if (!check_if_invite_only_channel(tokens2[0]) || (check_if_invite_only_channel(tokens2[0]) && client.get_is_invited(tokens2[0])))
				{
					add_client_to_channel(client, tokens2[0]);
					add_channel_to_client(client, tokens2[0]);
				}
				else
					std::cerr << "channel is invite only" << std::endl;
			}
			else
			{
				// channel does not exist
				Channel channel;
				channel.set_name(tokens2[0]);
				_channels.push_back(channel);
				client.add_channel(channel, 1);
				add_client_to_channel(client, channel.get_name());
			}
		}
		else
		{
			str2 = strtok((char *)(command.c_str() + 5), ",\r\n");
			while (str2 != NULL)
			{
				tokens2.push_back(str2);
				str2 = strtok(NULL, ",\r\n");
			}
			while (i < tokens2.size())
			{
				if (tokens2[i][0] == '#')
				{
					if (check_channel_if_exist(tokens2[i]))
					{
						// channel already exist
						if (check_if_client_already_joined(client, tokens2[0]))
							continue;
						if (!check_if_invite_only_channel(tokens2[i]) || (check_if_invite_only_channel(tokens2[0]) && client.get_is_invited(tokens2[0])))
						{
							add_client_to_channel(client, tokens2[i]);
							add_channel_to_client(client, tokens2[i]);
						}
						else
							std::cerr << "channel is invite only" << std::endl;
					}
					else
					{
						Channel channel;
						channel.set_name(tokens2[i]);
						_channels.push_back(channel);
						client.add_channel(channel, 1);
						add_client_to_channel(client, channel.get_name());
					}
				}
				else
					std::cerr << tokens2[i] << " :Bad Channel Mask" << std::endl;
				i++;
			}
		}
		std::cout << "size be3d :" << _channels.size() << std::endl;
	}

	/////////////////////////////////////part_command///////////////////////////////////////////////////////////////////

	void part_from_channel(Client &client, std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (token == _channels[i].get_name())
			{
				client.leave_channel(_channels[i]);
				_channels[i].remove_client(client);
				break;
			}
		}
	}

	int check_if_client_inside_channel(Client &client, std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token)
			{
				if (client.if_element_exist(_channels[i]))
					return 1;
			}
		}
		return 0;
	}

	void part_command(Client &client, std::string buffer)
	{
		char *str;
		char *str2;
		int i = 0;
		int flag = 0;
		std::vector<char *> tokens;
		std::vector<char *> tokens2;
		str = strtok((char *)(buffer.c_str() + 5), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (tokens.size() != 1)
		{
			// split it to take the first one
			str2 = strtok((char *)(buffer.c_str() + 5), " \r");
			while (str2 != NULL)
			{
				tokens2.push_back(str2);
				str2 = strtok(NULL, " \r\n");
			}
			// check if this channel is exist in this client
			if (check_if_client_inside_channel(client, tokens2[0]))
				part_from_channel(client, tokens2[0]);
			else
				std::cerr << client.get_username() << " " << tokens2[0] << " :No such channel" << std::endl;
		}
		else
		{
			str2 = strtok((char *)(buffer.c_str() + 5), ",\r\n");
			while (str2 != NULL)
			{
				tokens2.push_back(str2);
				str2 = strtok(NULL, ",\r\n");
			}
			while (i < tokens2.size())
			{
				if (tokens2[i][0] == '#')
				{
					if (check_if_client_inside_channel(client, tokens2[i]))
						part_from_channel(client, tokens2[i]);
					else
						std::cerr << client.get_username() << " " << tokens2[0] << " :No such channel" << std::endl;
				}
				else
					std::cerr << client.get_username() << " " << tokens2[0] << " :No such channel" << std::endl;
				i++;
			}
		}
	}

	//////////////////////////////////invite_command//////////////////////////////////////////////////////////////////////

	void add_channel_to_invited(std::string token0, std::string token1)
	{
		int i = 0;
		int j = 0;
		std::map<int, Client>::iterator iter;
		for (i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token1)
				break;
		}
		for (iter = _clients.begin(); iter != _clients.end(); iter++)
		{
			if ((*iter).second.get_nickname() == token0)
				break;
		}
		(*iter).second.set_is_invited(_channels[i].get_name());
	}

	int check_client_is_op(Client &client, std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token)
			{
				if (client.get_channel(_channels[i]))
					return 1;
			}
		}
		return 0;
	}

	int check_invite_only_channel(std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token)
			{
				ch_modes ch = _channels[i].get_modes();
				if (ch.i == 1)
					return 1;
			}
		}
		return 0;
	}

	int check_if_client_exist(std::string token)
	{
		std::map<int, Client>::iterator iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++)
		{
			if ((*iter).second.get_nickname() == token)
				return 1;
		}
		return 0;
	}

	void invite_command(Client &client, std::string buffer)
	{
		char *str;
		std::vector<char *> tokens;
		str = strtok((char *)(buffer.c_str() + 7), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (!check_if_client_exist(tokens[0]))
		{
			std::cerr << "no such client with this name" << std::endl;
			return;
		}
		if (!check_channel_if_exist(tokens[1]))
		{
			std::cerr << "no such channel with this name" << std::endl;
			return;
		}
		if (check_invite_only_channel(tokens[1]))
		{
			if (check_client_is_op(client, tokens[1]))
			{
				add_channel_to_invited(tokens[0], tokens[1]);
				std::cout << "youv been invited" << std::endl;
			}
			else
			{
				std::cerr << "client need to be an opp" << std::endl;
				return;
			}
		}
		else
		{
			add_channel_to_invited(tokens[0], tokens[1]);
			std::cout << "youv been invited" << std::endl;
		}
	}

	//////////////////////////////mode_command//////////////////////////////////////////////////////////////////////////

	void set_channel_psw_and_mode(std::string token, std::string password, int flag)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (token == _channels[i].get_name())
			{
				_channels[i].set_channel_psw(password);
				if (flag == 1)
					_channels[i].set_mode('k', 1);
				else
					_channels[i].set_mode('k', 0);
				break;
			}
		}
	}

	int check_new_already_join(std::string token, std::string channel_name)
	{
		std::map<int, Client>::iterator iter;
		if (check_if_client_exist(token))
		{
			for (iter = _clients.begin(); iter != _clients.end(); iter++)
			{
				if (token == (*iter).second.get_nickname())
				{
					if (check_if_client_already_joined((*iter).second, channel_name))
						return 1;
				}
			}
		}
		else
			std::cerr << "clients does not exist" << std::endl;
		return 0;
	}

	void change_client_mode_o(std::string client_name, std::string channel_name, int flag)
	{
		std::map<int, Client>::iterator iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++)
		{
			if (client_name == (*iter).second.get_nickname())
			{
				for (int j = 0; j < _channels.size(); j++)
				{
					if (_channels[j].get_name() == channel_name)
					{
						(*iter).second.modify_channel_bool(_channels[j], flag);
						break;
					}
				}
				break;
			}
		}
	}

	void set_channel_mode(std::string token, char mode, int flag)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token)
			{
				_channels[i].set_mode(mode, 1);
				break;
			}
		}
	}

	void mode_command(Client &client, std::string buffer)
	{
		char *str;
		std::string password;
		const char *modes = buffer.c_str();
		std::vector<char *> tokens;
		str = strtok((char *)(buffer.c_str() + 5), " \r\n");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r\n");
		}
		if (check_channel_if_exist(tokens[0]))
		{
			if (!std::strcmp(tokens[1], "+k") || !std::strcmp(tokens[1], "+o"))
			{
				if (!std::strcmp(tokens[1], "+k"))
				{
					int pos = buffer.find(tokens[2]);
					password = buffer.substr(pos);
					set_channel_psw_and_mode(tokens[0], password, 1);
				}
				else if (!std::strcmp(tokens[1], "+o"))
				{
					if (check_if_client_exist(tokens[2]) && check_if_client_already_joined(client, tokens[0]) && check_new_already_join(tokens[2], tokens[0]) && check_client_is_op(client, tokens[0]))
						change_client_mode_o(tokens[2], tokens[0], 1);
				}
			}
			else if (!std::strcmp(tokens[1], "-k") || !std::strcmp(tokens[1], "-o"))
			{
				if (!std::strcmp(tokens[1], "-k"))
					set_channel_psw_and_mode(tokens[0], "", 0);
				else if (!std::strcmp(tokens[1], "-o"))
				{
					if (check_if_client_exist(tokens[2]) && check_if_client_already_joined(client, tokens[0]) && check_new_already_join(tokens[2], tokens[0]) && check_client_is_op(client, tokens[0]))
					{
						change_client_mode_o(tokens[2], tokens[0], 0);
					}
				}
			}
			else if (!std::strcmp(tokens[1], "+i") || !std::strcmp(tokens[1], "+l") || !std::strcmp(tokens[1], "+t"))
			{
				if (!std::strcmp(tokens[1], "+i"))
					set_channel_mode(tokens[0], 'i', 1);
				else if (!std::strcmp(tokens[1], "+l"))
					set_channel_mode(tokens[0], 'l', 1);
				else if (!std::strcmp(tokens[1], "+t"))
					set_channel_mode(tokens[0], 't', 1);
			}
			else if (!std::strcmp(tokens[1], "-i") || !std::strcmp(tokens[1], "-l") || !std::strcmp(tokens[1], "-t"))
			{
				if (!std::strcmp(tokens[1], "-i"))
					set_channel_mode(tokens[0], 'i', 0);
				else if (!std::strcmp(tokens[1], "-l"))
					set_channel_mode(tokens[0], 'l', 0);
				else if (!std::strcmp(tokens[1], "-t"))
					set_channel_mode(tokens[0], 't', 0);
			}
		}
		else
		{
			std::cerr << "no such channel" << std::endl;
			return;
		}
	}
	/////////////////////////////////topic_command////////////////////////////////////////////////////////////

	void set_topic(std::string channel_name, std::string topic)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == channel_name)
			{
				_channels[i].set_topic(topic);
				break;
			}
		}
	}

	void print_topic(std::string token)
	{
		for (int i = 0; i < _channels.size(); i++)
		{
			if (_channels[i].get_name() == token)
			{
				std::cout << "Topic of " << token << " : " << _channels[i].get_topic() << std::endl;
				break;
			}
		}
	}

	void topic_command(std::string command)
	{
		char *str;
		std::vector<char *> tokens;
		std::string topic;
		str = strtok((char *)(command.c_str() + 6), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (tokens.size() == 0)
			std::cout << "syntax errrooor " << std::endl;
		if (tokens.size() > 1)
		{
			if (check_channel_if_exist(tokens[0]))
			{
				int pos = command.find(tokens[1]);
				topic = command.substr(pos);
				set_topic(tokens[0], topic);
			}
			else
				std::cerr << "no such channel" << std::endl;
		}
		else
			print_topic(tokens[0]);
	}

	////////////////////////////////////kick_command////////////////////////////////////////////////////////////////////

	int check_if_kicked_client_joined(std::string client_name, std::string channel_name)
	{
		std::map<int, Client>::iterator iter;
		for (int i = 0; i < _channels.size(); i++)
		{
			if (channel_name == _channels[i].get_name())
			{
				for (iter = _clients.begin(); iter != _clients.end(); iter++)
				{
					if ((*iter).second.get_nickname() == client_name)
					{
						if ((*iter).second.if_element_exist(_channels[i]))
							return 1;
						break;
					}
				}
				break;
			}
		}
		return 0;
	}

	void remove_channel_from_client(std::string client_name, std::string channel_name)
	{
		std::map<int, Client>::iterator iter;
		for (iter = _clients.begin(); iter != _clients.end(); iter++)
		{
			if ((*iter).second.get_nickname() == client_name)
			{
				for (int i = 0; i < _channels.size(); i++)
				{
					if (channel_name == _channels[i].get_name())
					{
						(*iter).second.leave_channel(_channels[i]);
						_channels[i].remove_client((*iter).second);
						break;
					}
				}
				break;
			}
		}
	}

	void kick_command(Client &client, std::string command)
	{
		char *str;
		std::vector<char *> tokens;
		str = strtok((char *)(command.c_str() + 5), " \r");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " \r");
		}
		if (tokens.size() < 2)
		{
			std::cerr << "Not enough parameters" << std::endl;
			return;
		}
		if (!check_channel_if_exist(tokens[0]) || check_if_client_exist(tokens[1]) || !check_if_client_already_joined(client, tokens[0]) || !check_if_kicked_client_joined(tokens[1], tokens[0]) || !check_client_is_op(client, tokens[0]))
		{
			std::cerr << "eroooooooooooooooooooooooor" << std::endl;
			return;
		}
		remove_channel_from_client(tokens[1], tokens[0]);
	}

	// kick #chann rida reason
	////////////////////////////////////////////////////////////////////////////////////////////////////////
};

int main(int ac, char **av)
{
	int socket;

	Server server("", 0);
	if (ac != 3)
	{
		std::cout << "Wrong parametres !!!\nUsage: ./ft_irc (port) (password)\n";
		return (0);
	}
	server.setPass(av[2]);
	server.setPort(std::stoi(av[1]));
	socket = server.CreateSocketConnection();
	if (listen(socket, server.getMaxClientsNumber()) == -1)
	{
		std::cerr << "Error/nlisten failed\n";
		return (0);
	}
	server.setServerSock(socket);
	server.ServerRun();
	return (0);
}
