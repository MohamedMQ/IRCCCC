#include "Server.hpp"

int Server::pars_nickname(std::string nickname)
{
	int i = 0;
	if (!((nickname[i] >= 'a' && nickname[i] <= 'z') || (nickname[i] >= 'A' && nickname[i] <= 'Z') || nickname[i] == '_'))
	{
		return 0;
	}
	if (nickname.size() > 14 || nickname.size() < 7)
		return 0;
	for (int i = 0; i < nickname.size(); i++)
	{
		if ((nickname[i] >= 'a' && nickname[i] <= 'z') || (nickname[i] >= 'A' && nickname[i] <= 'Z') || (nickname[i] >= '1' && nickname[i] <= '9') || nickname[i] == '_' || nickname[i] == '[' || nickname[i] == ']' || nickname[i] == '}' || nickname[i] == '{' || nickname[i] == '\\' || nickname[i] == '|')
			continue;
		else
			return 0;
	}
	return 1;
}

void Server::nickname_command(std::string buffer, Client &client, int &clientSocket)
{
	char *str;
	std::string response;
	int bytes_sent;
	std::map<int, Client>::iterator ptr;
	std::vector<std::string> tokens;
	std::string clientIP(client.getClientIP());

	str = strtok((char *)(buffer.c_str()), " ");
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (tokens.size() < 2 || (buffer.find(":") == 5 && !client.get_is_nickF()))
	{
		response = ":" + client.get_nickname() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		close(clientSocket);
		_clients.erase(clientSocket);
		clientSocket = -1;
	}
	else if (!client.get_is_nickF())
	{
		std::string s(tokens[1]);
		if (!pars_nickname(s))
		{
			response = ":" + client.get_nickname() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			close(clientSocket);
			_clients.erase(clientSocket);
			clientSocket = -1;
			return;
		}
		if (check_if_client_exist(s))
		{
			response = ":" + client.get_nickname() + " 433 " + client.get_nickname() + " " + s + " :Nickname is already in use\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			close(clientSocket);
			_clients.erase(clientSocket);
			clientSocket = -1;
			return;
		}
		response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " NICK :" + s + "\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		client.set_nickname(s);
		client.set_is_nickF(1);
	}
	else
	{
		str = strtok((char *)(buffer.c_str()), " ");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		if (tokens.size() >= 2)
		{
			if (!std::strcmp(tokens[1].c_str(), ":"))
			{
				response = ":" + client.get_nickname() + " 431 " + client.get_nickname() + " :No nickname given\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			for (ptr = _clients.begin(); ptr != _clients.end(); ptr++)
			{
				if ((*ptr).second.get_nickname() == tokens[1])
				{
					response = ":" + client.get_nickname() + " 433 " + client.get_nickname() + " " + tokens[1] + " :Nickname is already in use\r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
					return;
				}
			}
			if (!pars_nickname(tokens[1]))
			{
				response = ":" + client.get_nickname() + " 432 " + client.get_nickname() + " :Erroneous Nickname\r\n";
				bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				return;
			}
			response = ":" + client.get_nickname() + "!" + client.get_username() + "@" + clientIP + " NICK :" + tokens[1] + "\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			client.set_nickname(tokens[1]);
			client.set_is_nickF(1);
		}
	}
}