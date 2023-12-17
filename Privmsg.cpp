#include "Server.hpp"

int Server::check_channel_if_exist(std::string channel_name)
{
	int flag;

	flag = 0;
	for (int i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].get_name() == channel_name)
			flag = 1337;
	}
	return flag;
}

void Server::privmsg_command(Client &client, std::string command, int &clientSocket)
{
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
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (tokens.size() < 2)
	{
		response = ":" + client.get_nickname() + " 411 " + client.get_nickname() + " :No recipient given (PRIVMSG)\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	if (tokens.size() < 3)
	{
		response = ":" + client.get_nickname() + " 412 " + client.get_nickname() + " :No text to send\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		return;
	}
	else if (tokens[1] == "bot")
		bot_commad(client, temp_command, clientSocket);
	else
	{
		if (tokens[1][0] == '#')
		{
			if (check_channel_if_exist(tokens[1]))
				flag = 42;
			else
			{
				response = ":" + client.get_nickname() + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
		}
		else
		{
			if (check_if_client_exist(tokens[1]))
				flag = 1337;
			else
			{
				response = ":" + client.get_nickname() + " 401 " + client.get_nickname() + " " + tokens[1] + " :No such nick\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
		}
		std::string clientIP(client.getClientIP());
		if (flag == 1337)
		{
			for (iter = _clients.begin(); iter != _clients.end(); iter++)
			{
				if ((*iter).second.get_nickname() == tokens[1])
				{
					int pos = temp_command.find(tokens[2]);
					if (tokens[2][0] == ':')
						message = temp_command.substr(pos + 1);
					else
						message = temp_command.substr(pos);
					for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++)
					{
						if ((*iter2).second.get_nickname() == tokens[1])
						{
							break;
						}
					}
					response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
					bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
					(*iter).second.set_private_message((*iter).second.get_nickname(), message);
					break;
				}
			}
		}
		else if (flag == 42)
		{
			if (check_if_client_already_joined(client, tokens[1]))
			{
				for (int i = 0; i < _channels.size(); i++)
				{
					if (_channels[i].get_name() == tokens[1])
					{
						int pos = temp_command.find(tokens[2]);
						if (tokens[2][0] == ':')
							message = temp_command.substr(pos + 1);
						else
							message = temp_command.substr(pos);
						for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++)
						{
							if (check_if_client_already_joined((*iter2).second, tokens[1]) && (*iter2).first != clientSocket)
							{
								response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " PRIVMSG " + tokens[1] + " :" + message + "\r\n";
								bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
							}
						}
						_channels[i].add_message(client.get_nickname(), message);
						break;
					}
				}
			}
			else
			{
				response = ":" + client.get_nickname() + " 403 " + client.get_nickname() + " " + tokens[1] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
		}
	}
}
