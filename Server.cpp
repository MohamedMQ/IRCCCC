#include "irc.hpp"

class Server {
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
	std::vector <std::string> _clients_oper;
	std::string oper_username;
	std::string oper_password;

public:
	Server(std::string password, int port) {
		oper_username = "mmaqbour";
		oper_password = "rennacir";
		_password = password;
		_portNumber = port;
		_maxClientsNumber = 50;
		_serverName = "TIGERS";
		_pollFds = new struct pollfd[_maxClientsNumber];
	}

	int getMaxClientsNumber() {
		return _maxClientsNumber;
	}

	void setPass(std::string pass) {
		_password = pass;
	}

	void setPort(int port) {
		_portNumber = port;
	}

	void setServerSock(int socket) {
		_serverSock = socket;
	}

	std::string getServerName() {
		return _serverName;
	}

	// void add_client_as_oper(std::string client_name)
	// {
	// 	this->_clients_oper.push_back(client_name);
	// }

	// void remove_client_from_op(std::string)
	// {

	// }

	int requiredParams(Client &client) {
		if (client.get_is_userF() && client.get_is_nickF() && client.get_is_passF())
			return (1);
		else
			return (0);
	}

	void params_requirements(Client &client, int &clientSocket) {
		std::string response;
		int bytes_read;

		if (!client.get_is_passF()) {
			std::string response = ":" + this->getServerName() + " 464 " + client.get_nickname() + " :You must identify with a password before running commands\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		} else if (!client.get_is_nickF()) {
			std::string response = ":" + this->getServerName() + " 431 " + client.get_nickname() + " :No nickname given. Use NICK command to set your nickname\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		} else {
			std::string response = ":" + this->getServerName() + " 451 * :No username given. Use USER command to set your username\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}

	void executeAll(Client &client, std::string buffer, int &clientSocket, std::string _password) {
		std::string buffer_temp = buffer;
		char *str;
		std::vector<char *> tokens;
		str = strtok((char *)(buffer.c_str()), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok (NULL, " ");
		}
		if (!std::strcmp(tokens[0], "USER") && client.get_is_passF())
			user_command(buffer_temp, client, clientSocket);
		else if (!std::strcmp(tokens[0], "NICK") && client.get_is_passF())
			nickname_command(buffer_temp, client, clientSocket);
		else if (!std::strcmp(tokens[0], "PASS"))
			pass_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "QUIT") && client.get_is_passF()) {
			if (quit_command(clientSocket, buffer_temp) == -1)
				return;
		} else if (!std::strcmp(tokens[0], "PRIVMSG") && requiredParams(client))
			privmsg_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "JOIN") && requiredParams(client))
			join_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "PART") && requiredParams(client))
			part_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "INVITE") && requiredParams(client))
			invite_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "MODE") && requiredParams(client))
			mode_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "TOPIC") && requiredParams(client))
			topic_command(client, buffer_temp);
		else if (!std::strcmp(tokens[0], "KICK") && requiredParams(client))
			kick_command(client, buffer_temp, clientSocket);
		else if (!std::strcmp(tokens[0], "OPER") && requiredParams(client))
			oper_command(client, buffer_temp, clientSocket);
		else if (!requiredParams(client))
			params_requirements(client, clientSocket);
		else {
			std::string response = ":" + this->getServerName() + " 421 " + client.get_nickname() + tokens[0] + " :Unknown command\r\n";
			int bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}

	int CreateSocketConnection() {
		int yes = 1;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			std::cout << "Error/ninitializing the socket\n";
			return -1;
		}
		setServerSock(sockfd);
		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(_portNumber);
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(sockfd, (sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
			std::cout << "Error/nbinding with the socket failed\n";
			close(sockfd);
			return -1;
		}
		return sockfd;
	}

	void initClient() {
		for (int i = 1; i < _maxClientsNumber; i++) {
			_pollFds[i].fd = 0;
			_pollFds[i].events = POLLIN;
			_pollFds[i].revents = 0;
		}
	}

	int indexClient() {
		int i;
		for (i = 1; i < _maxClientsNumber; i++) {
			if (_pollFds[i].fd == -1)
				return i;
		}
		for (i = 1; i < _maxClientsNumber; i++) {
			if (_pollFds[i].fd == 0)
				break;
		}
		return i;
	}

	void ServerRun() {
		initClient();
		std::cout << "Server is running\n";
		int flag = fcntl(_serverSock, F_GETFL, 0);

		fcntl(_serverSock, F_SETFL, flag | O_NONBLOCK);

		_pollFds[0].fd = _serverSock;
		_pollFds[0].events = POLLIN;
		_pollFds[0].revents = 0;
		int response = 0;
		int i = 1;

		while (1) {
			response = poll(_pollFds, _maxClientsNumber, -1);
			if (response == -1) {
				std::cout << "Error\npoll failed\n";
				return;
			}
			if (_pollFds[0].revents && POLLIN) {
				int clientSocket = accept(_serverSock, NULL, NULL);
				if (clientSocket < 0) {
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
			}
			for (long unsigned int j = 1; j <= _clients.size(); j++) {
				if (_pollFds[j].fd == -1)
					continue;
				if (_pollFds[j].revents & POLLIN) {
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					response = recv(_pollFds[j].fd, buffer, sizeof(buffer), 0);
					if (response == -1) {
						std::cout << "Socket fd: " << _pollFds[j].fd << " J is " << j << std::endl;
						std::cout << "Error\nrecv failed: " << response << std::endl;
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							continue;
						else
							return;
					} else if (response == 0) {
						std::cout << "Client " << _pollFds[j].fd << " disconnected from the server\n";
						_saveSemiCommands.erase(_pollFds[j].fd);
						_clients.erase(_pollFds[j].fd);
						close(_pollFds[j].fd);
						_pollFds[j].fd = -1;
						continue;
					} else {
						std::string receivedData(buffer, response);
						std::string &partialCommand = _saveSemiCommands[_pollFds[j].fd];
						partialCommand += receivedData;

						if (!partialCommand.empty()) {
							size_t newlinePos = partialCommand.find('\n');
							while (newlinePos != std::string::npos) {
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

	void fill_client(std::string command, Client &client, int flag) {
		char *str;
		std::string temp_command = command;
		std::vector<std::string> tokens;
		std::string realname;
		str = strtok((char *)(command.c_str()), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (flag == 5) {
			client.set_username(client.get_nickname());
			int pos = temp_command.find(tokens[4]);
			realname = temp_command.substr(pos);
			client.set_real_name(realname);
		} else if (flag == 10) {
			client.set_username(client.get_nickname());
			client.set_real_name(client.get_nickname());
		} else {
			client.set_username(tokens[1]);
			int pos = temp_command.find(tokens[4]);
			realname = temp_command.substr(pos);
			client.set_real_name(realname);
		}
	}

	int pars_nickname(std::string nickname)
	{
		int i = 0;
		if (!(nickname[i] >= 'a' && nickname[i] <= 'z')
			|| (nickname[i] >= 'A' && nickname[i] <= 'Z')
			|| nickname[i] == '_') {
			return 0;
		}
		for (int i = 0; i < nickname.size(); i++) {
			if ((nickname[i] >= 'a' && nickname[i] <= 'z')
				|| (nickname[i] >= 'A' && nickname[i] <= 'Z')
				|| (nickname[i] >= '1' && nickname[i] <= '9')
				|| nickname[i] == '_' || nickname[i] == '['
				|| nickname[i] == ']' || nickname[i] == '}'
				|| nickname[i] == '{' || nickname[i] == '\\'
				|| nickname[i] == '|')
				continue;
			else
				return 0;
		}
		return 1;
	}

	int pars_user_command(std::string command, int &flag, Client &client, int &clientSocket) {
		std::string response;
		std::string real_name;
		int bytes_sent;
		int pos;
		int i;

		std::vector<std::string> tokens;
		char *str;
		std::string temp_command = command;
		str = strtok((char *)(command.c_str()), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() < 5) {
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " USER :Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return 0;
		}
		if (!pars_nickname(tokens[1])
			&& std::strcmp(tokens[2].c_str(), "0")
			&& std::strcmp(tokens[3].c_str(), "*")) {
			response = ":" + this->getServerName() + " 468 " + client.get_nickname() + " :Your username is not valid\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			_clients.erase(clientSocket);
			close(clientSocket);
			clientSocket = -1;
			return 0;
		}
		pos = temp_command.find(tokens[4]);
		real_name = temp_command.substr(pos);
		if (real_name == ":") {
			flag = 10;
			return 1;
		}
		i = 4;
		while (i < tokens.size()) {
			if (!pars_nickname(tokens[4])) {
				flag = 10;
				break;
			}
			i++;
		}
		return 1;
	}

	void user_command(std::string _command, Client &client, int &socket) {
		std::string response;
		int bytes_sent;

		if (client.get_is_userF() == 0) {
			int flag = 0;
			if (!pars_user_command(_command, flag, client, socket))
				return;
			fill_client(_command, client, flag);
			std::string response = ":" + this->getServerName() + " 001 " + _clients[socket].get_nickname() + " :Welcome to the IRC Network " + _clients[socket].get_nickname() + "!" + _clients[socket].get_username() + "@" + this->getServerName() + "\r\n";
			int bytes_sent = send(socket, response.c_str(), response.size(), 0);
			client.set_is_userF(1);
		} else {
			response = ":" + this->getServerName() + " 462 " + client.get_nickname() + " :You may not reregister\r\n";
			bytes_sent = send(socket, response.c_str(), response.size(), 0);
		}
	}

	////////////////////////////////////////pass_command////////////////////////////////////////////////////////////////

	void pass_command(Client &client, std::string _command, int &socket) {
		std::string response;
		int bytes_sent;

		if (_command.substr(0, 4) == "PASS") {
			if (client.get_is_passF() == 0) {
				if (!std::strcmp(_command.c_str() + 5, _password.c_str())) {
					client.set_is_passF(1);
					return;
				} else {
					response = ":" + this->getServerName() + " 464 " + client.get_nickname() + " :Password incorrect\r\n";
					bytes_sent = send(socket, response.c_str(), response.size(), 0);
					client.set__retry_pass();
				}
			} else {
				response = ":" + this->getServerName() + " 462 " + client.get_nickname() + " :You may not reregister\r\n";
				bytes_sent = send(socket, response.c_str(), response.size(), 0);
			}
			if (client.get__retry_pass() == 3) {
				response = ":" + this->getServerName() + " 465 " + client.get_nickname() + " :Too many incorrect password attempts. You are being kicked out.\r\n";
				bytes_sent = send(socket, response.c_str(), response.size(), 0);
				close(socket);
				_clients.erase(socket);
				socket = -1;
				return;
			}
		}
	}

	/////////////////////////////////////////quit_command///////////////////////////////////////////////////////////////

	int quit_command(int &socket, std::string command) {
		if (!std::strcmp((command.substr(0, 4)).c_str(), "QUIT")) {
			close(socket);
			_clients.erase(socket);

			socket = -1;
			return -1;
		}
		return 0;
	}

	//////////////////////////////////////nickname_command//////////////////////////////////////////////////////////////////


	void nickname_command(std::string buffer, Client &client, int &clientSocket) {
		char *str;
		std::string response;
		int bytes_sent;
		std::map<int, Client>::iterator ptr;
		std::vector<std::string> tokens;

		if (buffer.find(":") == 5 && !client.get_is_nickF()) {
			response = ":" + this->getServerName() + " 432 " + "*" + " :Erroneous Nickname\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			close(clientSocket);
			_clients.erase(clientSocket);
			clientSocket = -1;
		} else if (!client.get_is_nickF())
		{
			str = strtok((char *)(buffer.c_str()), " ");
			while (str != NULL) {
				tokens.push_back(str);
				str = strtok(NULL, " ");
			}
			std::string s(tokens[1]);
			if (!pars_nickname(s)) {
				response = ":" + this->getServerName() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				close(clientSocket);
				_clients.erase(clientSocket);
				clientSocket = -1;
				return;
			}
			if (check_if_client_exist(s)) {
				response = ":" + this->getServerName() + " 433 " + client.get_nickname() + " " + s + " :Nickname is already in use\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				close(clientSocket);
				_clients.erase(clientSocket);
				clientSocket = -1;
				return;
			}
			response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " NICK :" + s + "\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			client.set_nickname(s);
			client.set_is_nickF(1);
		} else {
			str = strtok((char *)(buffer.c_str()), " ");
			while (str != NULL) {
				tokens.push_back(str);
				str = strtok(NULL, " ");
			}
			if (tokens.size() >= 2) {
				if (!std::strcmp(tokens[1].c_str(), ":")) {
					std::cout << "HERE [" << tokens[1] << "] NOW\n";
					response = ":" + this->getServerName() + " 431 " + client.get_nickname() + " :No nickname given\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
				for (ptr = _clients.begin(); ptr != _clients.end(); ptr++) {
					if ((*ptr).second.get_nickname() == tokens[1]) {
						response = ":" + this->getServerName() + " 433 " + client.get_nickname() + " " + tokens[1] + " :Nickname is already in use\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						return;
					}
				}
				if (!pars_nickname(tokens[1])) {
					response = ":" + this->getServerName() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
				response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " NICK :" + tokens[1] + "\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				client.set_nickname(tokens[1]);
				client.set_is_nickF(1);
			}
		}
	}

	///////////////////////////////////////privmsg_command/////////////////////////////////////////////////////////////////

	int check_channel_if_exist(std::string channel_name) {
		int flag;

		flag = 0;
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == channel_name)
				flag = 1337;
		}
		return flag;
	}

	void privmsg_command(Client &client, std::string command, int &clientSocket) {
		char *str;
		int flag = 0;
		std::string response;
		int bytes_sent;
		std::string message;
		std::string temp_command = command;
		std::vector<std::string> tokens;
		std::map<int, Client>::iterator iter;
		std::map<int, Client>::iterator iter2;

		str = strtok((char *)(command.c_str()), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() < 2) {
			response = ":" + this->getServerName() + " 411 " + client.get_nickname() + " :No recipient given (PRIVMSG)\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (tokens.size() < 3) {
			response = ":" + this->getServerName() + " 412 " + client.get_nickname() + " :No text to send\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		} else {
			if (tokens[1][0] == '#') {
				if (check_channel_if_exist(tokens[1]))
					flag = 42;
				else {
					response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
			} else {
				if (check_if_client_exist(tokens[1]))
					flag = 1337;
				else {
					response = ":" + this->getServerName() + " 401 " + client.get_nickname() + " " + tokens[1] + " :No such nick\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
			}
			if (flag == 1337) {
				for (iter = _clients.begin(); iter != _clients.end(); iter++) {
					if ((*iter).second.get_nickname() == tokens[1]) {
						int pos = temp_command.find(tokens[2]);
						message = temp_command.substr(pos);
						for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
							if ((*iter2).second.get_nickname() == tokens[1]) {
								break;
							}
						}
						response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
						bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
						(*iter).second.set_private_message((*iter).second.get_nickname(), message);
						break;
					}
				}
			} else if (flag == 42) {
				if (check_if_client_already_joined(client, tokens[1]))
				{
					for (int i = 0; i < _channels.size(); i++) {
						if (_channels[i].get_name() == tokens[1]) {
							int pos = temp_command.find(tokens[2]);
							message = temp_command.substr(pos);
							for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[1])
									&& (*iter2).first != clientSocket) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
							_channels[i].add_message(client.get_nickname(), message);
							break;
						}
					}
				} else {
					response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			}
		}
	}

	//////////////////////////join_command//////////////////////////////////////////////////////////////////////////////

	void add_channel_to_client(Client &client, std::string to_check) {
		for (int i = 0; i < _channels.size() ; i++) {
			if(_channels[i].get_name() == to_check) {
				client.add_channel(_channels[i], 0);
				break;
			}
		}
	}

	void add_client_to_channel(Client &client, std::string to_check) {
		int a ;
		for (int i = 0; i < _channels.size() ; i++) {
			if(_channels[i].get_name() == to_check) {
				_channels[i].add_client(client);
				break;
			}
		}
	}

	int check_if_client_already_joined(Client &client, std::string token) {
		for (int i = 0; i < _channels.size(); i++) {
			if (token == _channels[i].get_name()) {
				if (client.if_element_exist(_channels[i]))
					return 1;
				break;
			}
		}
		return 0;
	}

	ch_modes get_modes(std::string channel_name) {
		ch_modes ch;
		for (int i = 0; i < _channels.size(); i++) {
			if (channel_name == _channels[i].get_name()) {
				ch = _channels[i].get_modes();
				break;
			}
		}
		return ch;
	}

	int not_reach_limit(std::string channel_name) {
		for (int i = 0; i < _channels.size(); i++) {
			if (channel_name == _channels[i].get_name()) {
				if (_channels[i].get_limit_num_of_clients() > _channels[i].get_num_of_clients())
					return 1;
				break;
			}
		}
		return 0;
	}

	int check_password(std::string channel_name, std::string channel_password) {
		for (int i = 0; i < _channels.size(); i++) {
			if (channel_name == _channels[i].get_name()) {
				if (_channels[i].get_channel_psw() == channel_password)
					return 1;
				break;
			}
		}
		return 0;
	}

	void join_command(Client &client, std::string command, int &clientSocket) {
		char *str;
		char *str2;
		int i = 0;
		int flag = 0;
		std::vector<char *> tokens;
		std::vector<char *> tokens2;
		std::map<int, Client>::iterator iter1;
		std::map<int, Client>::iterator iter2;
		std::map<int, Client>::iterator iter3;
		std::string response;
		int bytes_sent;

		str = strtok((char *)(command.c_str() + 5), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok (NULL, " ");
		}
		if (tokens.size() >= 1) {
			str2 = strtok(tokens[0], ",");
			while (str2 != NULL) {
				tokens2.push_back(str2);
				str2 = strtok (NULL, ",");

			}
			while (i < tokens2.size()) {
				if (tokens2[i][0] == '#') {
					if(check_channel_if_exist(tokens2[i])) {
						ch_modes ch;
						ch = get_modes(tokens2[i]);
						// print_modes(tokens2[i]);
						// client.print_channels();
						if (check_if_client_already_joined(client, tokens2[i])) {
							i++;
							continue;
						}
						if (ch.i == 1 && !client.get_is_invited(tokens2[i])) {
							response = ":" + this->getServerName() + " 473 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+i)\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							i++;
							continue;
						}
						if (ch.l == 1 && !not_reach_limit(tokens2[i])) {
							std::cout << "SEG HERE JOIN 1\n";
							response = ":" + this->getServerName() + " 471 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+l)\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							i++;
							continue;
						}
						if (i == 0 && tokens.size() >= 2 && ch.k == 1 && !check_password(tokens2[0], tokens[1])) {
							std::cout << "SEG HERE JOIN 2\n";
							response = ":" + this->getServerName() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							i++;
							continue;
						} else if (i ==0 && ch.k == 1 && tokens.size() < 2){
							std::cout << "SEG HERE JOIN 3\n";
							response = ":" + this->getServerName() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							i++;
							continue;
						} else if (i != 0 && ch.k == 1) {
							std::cout << "SEG HERE JOIN 4\n";
							response = ":" + this->getServerName() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							i++;
							continue;
						}
						add_client_to_channel(client, tokens2[i]);
						add_channel_to_client(client, tokens2[i]);
						response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " JOIN " + tokens2[i] + "\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
							if (check_if_client_already_joined((*iter2).second, tokens2[i])
								&& (*iter2).first != clientSocket) {
								response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " JOIN " + tokens2[i] + "\r\n";
								bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								response = ":" + (*iter2).second.get_nickname() + "!" + (*iter2).second.get_username() + "@" + this->getServerName() + " JOIN " + tokens2[i] + "\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							}
						}
						for (iter3 = _clients.begin(); iter3 != _clients.end(); iter3++) {
							if (check_if_client_already_joined((*iter3).second, tokens2[i])
								&& check_client_is_op((*iter3).second, tokens2[i])) {
									response = ":" + this->getServerName() + " MODE " + tokens2[i] + " +o " + (*iter3).second.get_nickname() + "\r\n";
									bytes_sent = send((*iter3).first, response.c_str(), response.size(), 0);
									for (iter1 = _clients.begin(); iter1 != _clients.end(); iter1++) {
										if (check_if_client_already_joined((*iter1).second, tokens2[i])
											&& (*iter3).first != (*iter1).first)
											bytes_sent = send((*iter1).first, response.c_str(), response.size(), 0);
									}
							}
						}
					} else {
						Channel channel;
						channel.set_name(tokens2[i]);
						_channels.push_back(channel);
						client.add_channel(channel, 1);
						add_client_to_channel(client, channel.get_name());
						response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " JOIN " + tokens2[i] + "\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						response = ":" + this->getServerName() + " MODE " + tokens2[i] + " +o " + client.get_nickname() + "\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					}
				} else {
					response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
				i++;
			}
		} else {
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " JOIN : Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}
	/////////////////////////////////////part_command///////////////////////////////////////////////////////////////////

	void part_from_channel(Client &client, std::string token) {
		for (int i = 0; i < _channels.size() ; i++) {
			if (token == _channels[i].get_name()) {
				client.leave_channel(_channels[i]);
				_channels[i].remove_client(client);
				if (_channels[i].get_num_of_clients() == 0)
				{
					set_channel_mode(token, 'k', 0);
					set_channel_mode(token, 'o', 0);
					set_channel_mode(token, 't', 0);
					set_channel_mode(token, 'l', 0);
					set_channel_mode(token, 'i', 0);
				}
				break;
			}
		}
	}

	int check_if_client_inside_channel(Client &client, std::string token) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == token) {
				if (client.if_element_exist(_channels[i]))
					return 1;
			}
		}
		return 0;
	}

	void part_command(Client &client, std::string buffer, int &clientSocket) {
		char *str;
		char *str2;
		int i = 0;
		int flag = 0;
		std::string reason;
		std::string response;
		int bytes_sent;
		std::string buffer_temp = buffer;
        int pos;
		std::vector<char *> tokens;
		std::vector<char *> tokens2;
		std::map<int, Client>::iterator iter;

		str = strtok((char *)(buffer.c_str() + 5), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok (NULL, " ");
		}
		if (tokens.size() >= 1) {
			if (tokens.size() >= 2) {
				pos = buffer_temp.find(tokens[1]);
				reason = buffer_temp.substr(pos + 1);
			} else
				reason = "";
			str2 = strtok(tokens[0], ",");
			while (str2 != NULL) {
				tokens2.push_back(str2);
				str2 = strtok (NULL, ",");
			}
			while (i < tokens2.size()) {
				if (tokens2[i][0] == '#') {
					if(check_channel_if_exist(tokens2[i]) && check_if_client_inside_channel(client, tokens2[i])) {
						for (iter = _clients.begin(); iter != _clients.end(); iter++) {
							if (check_if_client_already_joined((*iter).second, tokens2[i])) {
								response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " PART " + tokens2[i] + " :" + reason + "\r\n";
								bytes_sent = send((*iter).first, response.c_str(), response.size(), 0);
							}
						}
						part_from_channel(client ,tokens2[i]);
					} else if (!check_if_client_inside_channel(client, tokens2[i]) && check_channel_if_exist(tokens2[i])) {
						response = ":" + this->getServerName() + " 442 " + client.get_nickname() + " " + tokens2[i] + " :You're not on that channel\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					} else {
						response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					}
				} else {
					response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
				i++;
			}
		} else {
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " PART :Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
	}
	//////////////////////////////////invite_command//////////////////////////////////////////////////////////////////////

	void add_channel_to_invited(std::string client_name, std::string channel_name) {
		int i = 0;
		int j = 0;
		std::map<int, Client>::iterator iter;

		for (i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == channel_name)
				break;
		}
		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			if ((*iter).second.get_nickname() == client_name)
				break;
		}
		(*iter).second.set_is_invited(_channels[i].get_name());
	}

	int check_client_is_op(Client &client, std::string channel_name) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == channel_name) {
				if (client.get_channel(_channels[i]))
					return 1;
			}
		}
		return 0;
	}

	int check_invite_only_channel(std::string token) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == token) {
				ch_modes ch = _channels[i].get_modes();
				if (ch.i == 1)
					return 1;
			}
		}
		return 0;
	}

	int check_if_client_exist(std::string client_name) {
		std::map<int, Client>::iterator iter;

		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			if ((*iter).second.get_nickname() == client_name)
				return 1;
		}
		return 0;
	}

	void invite_command(Client &client, std::string buffer, int clientSocket) {
		char *str;
		std::vector<char *> tokens;
		std::string temp_buffer = buffer;
		std::string response;
		int bytes_sent;

		str = strtok((char *)(buffer.c_str() + 7), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() >=2) {
			int pos = temp_buffer.find(tokens[1]);
			std::string chann_name = temp_buffer.substr(pos);
			if (!check_if_client_exist(tokens[0])) {
				// std::cerr << "no such client with this name" << std::endl;
				response = ":" + this->getServerName() + " 401 " + client.get_nickname() + " " + tokens[0] + " :No such nick\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			if (!check_channel_if_exist(chann_name)) {
				response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + chann_name + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				// std::cerr << "no such channel with this name" << std::endl;
				return;
			}
			if (check_if_kicked_client_joined(tokens[0], chann_name)) {
				response = ":" + this->getServerName() + " 443 " + client.get_nickname() + " " + tokens[0] + " " + chann_name + " :is already on channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			if (check_client_is_op(client, chann_name)) {
				std::map<int, Client>::iterator iter2;
				for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
					if ((*iter2).second.get_nickname() == tokens[0]) {
						break;
					}
				}
				response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " INVITE " + tokens[0] + " " + chann_name + "\r\n";
				bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
				response = ":" + this->getServerName() + " 001 " + client.get_nickname() + " :Inviting " + tokens[0] + " to " + chann_name + "\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				add_channel_to_invited(tokens[0], chann_name);
				// std::cout << "youv been invited" << std::endl;
			} else {
				response = ":" + this->getServerName() + " 481 " + client.get_nickname() + " :Permission Denied- You're not an IRC operator\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				// std::cerr << "client need to be an opp" << std::endl;
			}
		} else {
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " INVITE :Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			// std::cout << "no enough parameters" << std::endl;
		}
	}

	//////////////////////////////mode_command//////////////////////////////////////////////////////////////////////////

	void set_channel_psw_and_mode(std::string token, std::string password,  int flag) {
		for (int i = 0; i < _channels.size() ; i++) {
			if (token == _channels[i].get_name()) {
				_channels[i].set_channel_psw(password);
				if (flag == 1)
					_channels[i].set_mode('k', 1);
				else
					_channels[i].set_mode('k', 0);
				break;
			}
		}
	}

	int get_limit_num(std::string channel_name) {
		for (int i = 0; i < _channels.size(); i++) {
			if (channel_name == _channels[i].get_name()) {
				return(_channels[i].get_limit_num_of_clients());
			}
		}
		return 0;
	}

	int check_new_already_join(std::string token, std::string channel_name) {
		std::map<int, Client>::iterator iter;

		if (check_if_client_exist(token)) {
			for (iter = _clients.begin() ; iter != _clients.end() ; iter++) {
				if (token == (*iter).second.get_nickname()) {
					if (check_if_client_already_joined((*iter).second, channel_name))
						return 1;
				}
			}
		} else
			std::cerr << "clients does not exist" << std::endl;
		return 0;
	}

	void change_client_mode_o(std::string client_name, std::string channel_name, int flag) {
		std::map<int, Client>::iterator iter;

		for (iter = _clients.begin() ; iter != _clients.end() ; iter++) {
			if (client_name == (*iter).second.get_nickname()) {
				for (int j = 0; j < _channels.size() ; j++) {
					if (_channels[j].get_name() == channel_name) {
						(*iter).second.modify_channel_bool(_channels[j], flag);
						break;
					}
				}
				break;
			}
		}
	}

	void set_channel_mode(std::string token, char mode, int flag) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == token) {
				_channels[i].set_mode(mode, flag);
				break;
			}
		}
	}

	void print_modes(std::string channel_name) {
		ch_modes ch;

		for (int i = 0; i < _channels.size() ; i++) {
			if (_channels[i].get_name() == channel_name) {
				ch = _channels[i].get_modes();
				std::cout << "i :" << ch.i << " t :" << ch.t << " k :" << ch.k << " l :" << ch.l << " o :" << ch.o << std::endl;
			}
		}
	}

	int check_is_int(char *limit) {
		int i;

		i = 0;
		while (limit[i]) {
			if (limit[i] < '0' || limit[i] > '9')
				return 0;
			i++;
		}
		return 1;
	}

	int set_limit(std::string channel_name, std::string sett) {
		long long a;

		for (int i = 0; i < _channels.size() ; i++) {
			if (_channels[i].get_name() == channel_name) {
				a = atoll(sett.c_str());
				if (a == 0 || a < 0 || a > 1337)
					return 0;
				_channels[i].set_limit_num_of_clients(atoi(sett.c_str()));
				break;
			}
		}
		return 1;
	}

	int check_channel_pass(std::string channel_name, std::string password) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == channel_name) {
				if (_channels[i].get_channel_psw() == password || _channels[i].get_channel_psw() == "")
					return 1;
			}
		}
		return 0;
	}
	int check_if_name_client_is_op(std::string client_name, std::string channel_name) {
		std::map <int , Client>::iterator iter;
		for (iter = _clients.begin() ; iter != _clients.end() ; iter++) {
			if ((*iter).second.get_nickname() == client_name) {
				if (check_client_is_op((*iter).second, channel_name))
					return 1;
			}
		}
		return 0;
	}

	std::vector<std::string> split_options(std::string to_split)
	{
		std::vector<std::string> options;
		std::string tok;
		char sign;

		sign = to_split[0];
		for (int i = 1; i < to_split.size(); i++) {
			tok = "";
			tok += sign;
			tok+= to_split[i];
			options.push_back(tok);
		}
		return options;
	}

	std::vector<std::string> get_arguments(std::vector<char *>  tokens) {
		std::vector<std::string> vec;

		for (int i = 2 ; i < tokens.size() ;i++)
			vec.push_back(tokens[i]);
		return vec;
	}

	void mode_command(Client &client, std::string buffer, int &clientSocket) {
		char *str;
		std::string response;
		int bytes_sent;
		std::string password;
		int args_count = 0;
		std::vector<std::string> options;
		std::vector<std::string> arguments;
		std::string limit;
		const char *modes = buffer.c_str();
		std::vector<char *> tokens;

		str = strtok((char *)(buffer.c_str() + 5), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok (NULL, " ");
		}
		if (tokens.size() >= 2) {
			if (!check_channel_if_exist(tokens[0])) {
				response = ":" + this->getServerName() + " 403 " + client.get_nickname() + tokens[0] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return ;
			}
			if (!check_client_is_op(client, tokens[0])) {
				response = ":" + this->getServerName() + " 482 " + client.get_nickname() + " :You're not channel operator\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return ;
			}
			ch_modes ch = get_modes(tokens[0]);
			if(check_channel_if_exist(tokens[0]) && check_client_is_op(client, tokens[0])
				 && check_if_client_already_joined(client, tokens[0])) {
				options = split_options(tokens[1]);
				arguments = get_arguments(tokens);
				for (int p = 0; p < options.size() ; p++) {
					if (!std::strcmp(options[p].c_str(), "+k") || !std::strcmp(options[p].c_str(), "+o") || !std::strcmp(options[p].c_str(), "+l")) {
						if (!std::strcmp(options[p].c_str(), "+k")) {
							if (ch.k == 1) {
								response = ":" + this->getServerName() + " 467 " + client.get_nickname() + " " +tokens[0] + " :Channel key already set\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (args_count >= arguments.size()) {
								response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								continue;
							}
							set_channel_psw_and_mode(tokens[0], arguments[args_count], 1);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " +k " + arguments[args_count] + "\r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
							args_count++;
						} else if (!std::strcmp(options[p].c_str(), "+o")) {
							if (args_count >= arguments.size()) {
								response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								continue;
							}
							if (!check_if_client_exist(arguments[args_count])) {
								response = ":" + this->getServerName() + " 401 " + client.get_nickname() + " " + tokens[3] + " :No such nick\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (!check_new_already_join(arguments[args_count], tokens[0])) {
								response = ":" + this->getServerName() + " 441 " + client.get_nickname() + " " + tokens[0] + " :He is not on that channel\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (check_if_client_exist(arguments[args_count]) && check_new_already_join(arguments[args_count], tokens[0])) {
								if (!check_if_name_client_is_op(arguments[args_count], tokens[0])) {
									change_client_mode_o(arguments[args_count], tokens[0], 1);
									for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
										if (check_if_client_already_joined((*iter2).second, tokens[0])) {
											response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " +o " + arguments[args_count] + "\r\n";
											bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
										}
									}
									args_count++;
								} else {
									args_count++;
									continue;
								}
							}
						} else if (!std::strcmp(options[p].c_str(), "+l")) {
							if (args_count >= arguments.size()) {
								response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								continue;
							}
							if (!set_limit(tokens[0], arguments[args_count])) {
								response = ":" + this->getServerName() + " 468 " + client.get_nickname() + " " + tokens[0] + " :Invalid channel user limit\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (ch.l == 1 && atoll(arguments[args_count].c_str()) == get_limit_num(tokens[0])) {
								args_count++;
								continue;
							}
							set_channel_mode(tokens[0], 'l', 1);
							for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
								if (check_if_client_already_joined((*iter2).second, tokens[0])) {
									response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " +l " + arguments[args_count] + "\r\n";
									bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
								}
							}
							args_count++;
						}
					} else if (!std::strcmp(options[p].c_str(), "-k") || !std::strcmp(options[p].c_str(), "-o") || !std::strcmp(options[p].c_str(), "-l")) {
						if (!std::strcmp(options[p].c_str(), "-k")) {
								if (args_count >= arguments.size()) {
									response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
									bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
									continue;
								}
								if (ch.k == 0) {
									args_count++;
									continue;
								}
								if (check_channel_pass(tokens[0],arguments[args_count])) {
									set_channel_psw_and_mode(tokens[0], "", 0);
									args_count++;
									for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
										if (check_if_client_already_joined((*iter2).second, tokens[0])) {
											response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " -k\r\n";
											bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
										}
									}
								} else {
									response = ":" + this->getServerName() + " 475 " + client.get_nickname() + " " + tokens[0] + " :Cannot remove channel key -bad key\r\n";
									bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
									args_count++;
								}
						} else if (!std::strcmp(options[p].c_str(), "-o")) {
							if (args_count >= arguments.size()) {
								response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								continue;
							}
							if (!check_if_client_exist(arguments[args_count])) {
								response = ":" + this->getServerName() + " 401 " + client.get_nickname() + " " + tokens[3] + " :No such nick\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (!check_new_already_join(arguments[args_count], tokens[0])) {
								response = ":" + this->getServerName() + " 441 " + client.get_nickname() + " " + tokens[0] + " :He is not on that channel\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
								args_count++;
								continue;
							}
							if (check_if_client_exist(arguments[args_count]) && check_new_already_join(arguments[args_count], tokens[0])) {
								if (check_if_name_client_is_op(arguments[args_count], tokens[0])) {
									change_client_mode_o(arguments[args_count], tokens[0], 0);
									for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
										if (check_if_client_already_joined((*iter2).second, tokens[0])) {
											response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " -o " + arguments[args_count] + "\r\n";
											bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
										}
									}
									args_count++;
								} else {
									args_count++;
									continue;
								}
							}
						} else if (!std::strcmp(options[p].c_str(), "-l")) {
							if (tokens.size() >= 2) {
								if (ch.l == 1) {
									set_channel_mode(tokens[0], 'l', 0);
									for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
										if (check_if_client_already_joined((*iter2).second, tokens[0])) {
											response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " -l \r\n";
											bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
										}
									}
								}
							} else {
								response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " MODE :Not enough parameters\r\n";
								bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
							}
						}
					} else if (!std::strcmp(options[p].c_str(), "+i") || !std::strcmp(options[p].c_str(), "+t")) {
						if (tokens.size() >= 2) {
							if (!std::strcmp(options[p].c_str(), "+i")
								&& !ch.i) {
								set_channel_mode(tokens[0], 'i', 1);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " +i \r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
							} else if (!std::strcmp(options[p].c_str(), "+t")
									&& !ch.t) {
								set_channel_mode(tokens[0], 't', 1);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " +t \r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
							}
						}
					} else if (!std::strcmp(options[p].c_str(), "-i") || !std::strcmp(options[p].c_str(), "-t")) {
						if (tokens.size() >= 2) {
							if (!std::strcmp(options[p].c_str(), "-i")
								&& ch.i) {
								set_channel_mode(tokens[0], 'i', 0);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " -i \r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
							} else if (!std::strcmp(options[p].c_str(), "-t")
									&& ch.t) {
								set_channel_mode(tokens[0], 't', 0);
								for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
									if (check_if_client_already_joined((*iter2).second, tokens[0])) {
										response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " MODE " + tokens[0] + " -t \r\n";
										bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
									}
								}
							}
						}
					} else {
						response = ":" + this->getServerName() + " 472 " + client.get_nickname() + tokens[0] + " :is unknown mode char to me\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					}
				}
			}
		}
	}
	/////////////////////////////////topic_command////////////////////////////////////////////////////////////

	void set_topic(std::string channel_name, std::string topic) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == channel_name) {
				_channels[i].set_topic(topic);
				break;
			}
		}
	}

	void print_topic(std::string token) {
		for (int i = 0; i < _channels.size(); i++) {
			if (_channels[i].get_name() == token) {
				std::cout << "Topic of " << token << " : " << _channels[i].get_topic() << std::endl;
				break;
			}
		}
	}

	void topic_command(Client &client, std::string command) {
		char *str;
		std::vector<char *> tokens;
		std::string topic;
		ch_modes ch;

		str = strtok((char *)(command.c_str() + 6), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() == 0)
		{
			std::cout << "no enogh parameters" << std::endl;
			return;
		}
		if (tokens.size() > 1) {
			if (check_channel_if_exist(tokens[0])) {
				ch = get_modes(tokens[0]);
				if (ch.t == 1) {
					if (check_client_is_op(client, tokens[0])) {
						int pos = command.find(tokens[1]);
						topic = command.substr(pos);
						set_topic(tokens[0], topic);
					}
					else
						std::cerr << "Error(482): #hmed You're not channel operator" << std::endl;
				} else {
					int pos = command.find(tokens[1]);
					topic = command.substr(pos);
					set_topic(tokens[0], topic);
				}
			} else
				std::cerr << "no such channel" << std::endl;
		} else
			print_topic(tokens[0]);
	}

	////////////////////////////////////kick_command////////////////////////////////////////////////////////////////////

	int check_if_kicked_client_joined(std::string client_name, std::string channel_name) {
		std::map<int, Client>::iterator iter;

		for (int i = 0; i < _channels.size(); i++) {
			if (channel_name == _channels[i].get_name()) {
				for (iter = _clients.begin(); iter != _clients.end(); iter++) {
					if ((*iter).second.get_nickname() == client_name) {
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

	void remove_channel_from_client(std::string client_name, std::string channel_name) {
		std::map<int, Client>::iterator iter;

		for (iter = _clients.begin(); iter != _clients.end(); iter++) {
			if ((*iter).second.get_nickname() == client_name) {
				for (int i = 0; i < _channels.size(); i++) {
					if (channel_name == _channels[i].get_name()) {
						(*iter).second.leave_channel(_channels[i]);
						_channels[i].remove_client((*iter).second);
						break;
					}
				}
				break;
			}
		}
	}

	void kick_command(Client &client, std::string command, int &clientSocket) {
		char *str;
		std::vector<char *> tokens;
		std::string response;
		std::string reason;
		std::string temp_command = command;
		int pos;
		int bytes_sent;

		str = strtok((char *)(command.c_str() + 5), " ");
		while (str != NULL) {
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() <= 2) {
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " KICK :Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_channel_if_exist(tokens[0]) || !check_if_client_already_joined(client, tokens[0])) {
			response = ":" + this->getServerName() + " 403 " + client.get_nickname() + " " + tokens[0] + " :No such channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_if_client_exist(tokens[1])) {
			response = ":" + this->getServerName() + " 401 " + client.get_nickname() + " " + tokens[1] + " :No such nick\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_if_kicked_client_joined(tokens[1], tokens[0])) {
			response = ":" + this->getServerName() + " 441 " + client.get_nickname() + " " + tokens[1] + " " + tokens[0] + " :They aren't on that channel\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		if (!check_client_is_op(client, tokens[0])) {
			response = ":" + this->getServerName() + " 482 " + client.get_nickname() + " :You're not channel operator\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return ;
		}
		if (tokens.size() == 3 && !std::strcmp(tokens[2], ":"))	 {
			reason = client.get_nickname();
		} else {
			pos = temp_command.find(tokens[2]);
			reason = temp_command.substr(pos + 1);
		}
		response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " KICK " + tokens[0] + " " + tokens[1] + " :" + reason + "\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		for (std::map<int, Client>::iterator iter2 = _clients.begin(); iter2 != _clients.end(); iter2++) {
			if (check_if_client_already_joined((*iter2).second, tokens[0])) {
				response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + this->getServerName() + " KICK " + tokens[0] + " " + tokens[1] + " :" + reason + "\r\n";
				bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
			}
		}
		remove_channel_from_client(tokens[1], tokens[0]);
	}

	////////////////////////////////////oper_command////////////////////////////////////////////////////////////////////



	void oper_command(Client &client, std::string command, int &clientSocket)
	{
		char *str;
		std::vector<char *> tokens;
		std::string response;
		int bytes_sent;
		str = strtok((char *)(command.c_str()), " ");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() >= 3)
		{
			if (tokens[1] != this->oper_username)
			{
				response = ":" + this->getServerName() + " 464 " + tokens[1] + " OPER :Username Incorrect\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			if (tokens[2] != this->oper_password)
			{
				response = ":" + this->getServerName() + " 464 " + tokens[1] + " OPER :Password Incorrect\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			_clients_oper.push_back(client.get_nickname());
		}
		else
		{
			response = ":" + this->getServerName() + " 461 " + client.get_nickname() + " OPER :Not enough parameters\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return ;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void eraseAllClients() {
		_channels.clear();
		for (long unsigned int j = 1; j <= _clients.size(); j++) {
			if (_pollFds[j].fd != -1)
				close(_pollFds[j].fd);
		}
		_clients.clear();
		close(_pollFds[0].fd);
		exit(0);
	}
};

Server server("", 0);

void signal_handler(int sig) {
	server.eraseAllClients();
	exit (0);
}

int main(int ac, char **av)
{
	int socket;
	signal(SIGINT, signal_handler);

	if (ac != 3) {
		std::cout << "Wrong parametres !!!\nUsage: ./ft_irc (port) (password)\n";
		return (0);
	}
	server.setPass(av[2]);
	server.setPort(std::stoi(av[1]));
	socket = server.CreateSocketConnection();
	if (listen(socket, server.getMaxClientsNumber()) == -1) {
		std::cerr << "Error/nlisten failed\n";
		return (0);
	}
	server.setServerSock(socket);
	server.ServerRun();
	return (0);
}
