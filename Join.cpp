#include "Server.hpp"

void Server::add_channel_to_client(Client &client, std::string to_check)
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

void Server::add_client_to_channel(Client &client, std::string to_check)
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

int Server::check_if_client_already_joined(Client &client, std::string token)
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

ch_modes Server::get_modes(std::string channel_name)
{
	ch_modes ch;
	for (int i = 0; i < _channels.size(); i++)
	{
		if (channel_name == _channels[i].get_name())
		{
			ch = _channels[i].get_modes();
			break;
		}
	}
	return ch;
}

int Server::not_reach_limit(std::string channel_name)
{
	for (int i = 0; i < _channels.size(); i++)
	{
		if (channel_name == _channels[i].get_name())
		{
			if (_channels[i].get_limit_num_of_clients() > _channels[i].get_num_of_clients())
				return 1;
			break;
		}
	}
	return 0;
}

int Server::check_password(std::string channel_name, std::string channel_password)
{
	for (int i = 0; i < _channels.size(); i++)
	{
		if (channel_name == _channels[i].get_name())
		{
			if (_channels[i].get_channel_psw() == channel_password)
				return 1;
			break;
		}
	}
	return 0;
}

void Server::join_command(Client &client, std::string command, int &clientSocket)
{
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
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (tokens.size() >= 1)
	{
		str2 = strtok(tokens[0], ",");
		while (str2 != NULL)
		{
			tokens2.push_back(str2);
			str2 = strtok(NULL, ",");
		}
		std::string clientIP(client.getClientIP());
		while (i < tokens2.size())
		{
			if (tokens2[i][0] == '#')
			{
				if (check_channel_if_exist(tokens2[i]))
				{
					ch_modes ch;
					ch = get_modes(tokens2[i]);
					if (check_if_client_already_joined(client, tokens2[i]))
					{
						i++;
						continue;
					}
					if (ch.i == 1 && !client.get_is_invited(tokens2[i]))
					{
						response = ":" + client.get_nickname() + " 473 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+i)\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						i++;
						continue;
					}
					if (ch.l == 1 && !not_reach_limit(tokens2[i]))
					{
						response = ":" + client.get_nickname() + " 471 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+l)\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						i++;
						continue;
					}
					if (i == 0 && tokens.size() >= 2 && ch.k == 1 && !check_password(tokens2[0], tokens[1]))
					{
						response = ":" + client.get_nickname() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						i++;
						continue;
					}
					else if (i == 0 && ch.k == 1 && tokens.size() < 2)
					{
						response = ":" + client.get_nickname() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						i++;
						continue;
					}
					else if (i != 0 && ch.k == 1)
					{
						response = ":" + client.get_nickname() + " 457 " + client.get_nickname() + " " + tokens2[i] + " :Cannot join channel (+k)\r\n";
						bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						i++;
						continue;
					}
					add_client_to_channel(client, tokens2[i]);
					add_channel_to_client(client, tokens2[i]);
					response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " JOIN " + tokens2[i] + "\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					for (iter2 = _clients.begin(); iter2 != _clients.end(); iter2++)
					{
						if (check_if_client_already_joined((*iter2).second, tokens2[i]) && (*iter2).first != clientSocket)
						{
							response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " JOIN " + tokens2[i] + "\r\n";
							bytes_sent = send((*iter2).first, response.c_str(), response.size(), 0);
							response = ":" + (*iter2).second.get_nickname() + "!" + (*iter2).second.get_username() + "@" + clientIP + " JOIN " + tokens2[i] + "\r\n";
							bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
						}
					}
					for (iter3 = _clients.begin(); iter3 != _clients.end(); iter3++)
					{
						if (check_if_client_already_joined((*iter3).second, tokens2[i]) && check_client_is_op((*iter3).second, tokens2[i]))
						{
							response = ":" + client.get_nickname() + " MODE " + tokens2[i] + " +o " + (*iter3).second.get_nickname() + "\r\n";
							bytes_sent = send((*iter3).first, response.c_str(), response.size(), 0);
							for (iter1 = _clients.begin(); iter1 != _clients.end(); iter1++)
							{
								if (check_if_client_already_joined((*iter1).second, tokens2[i]) && (*iter3).first != (*iter1).first)
									bytes_sent = send((*iter1).first, response.c_str(), response.size(), 0);
							}
						}
					}
				}
				else
				{
					Channel channel;
					channel.set_name(tokens2[i]);
					_channels.push_back(channel);
					client.add_channel(channel, 1);
					add_client_to_channel(client, channel.get_name());
					response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " JOIN " + tokens2[i] + "\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					response = ":" + client.get_nickname() + " MODE " + tokens2[i] + " +o " + client.get_nickname() + "\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			}
			else
			{
				response = ":" + client.get_nickname() + " 403 " + client.get_nickname() + " " + tokens2[i] + " :No such channel\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			}
			i++;
		}
	}
	else
	{
		response = ":" + client.get_nickname() + " 461 " + client.get_nickname() + " JOIN : Not enough parameters\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	}
}