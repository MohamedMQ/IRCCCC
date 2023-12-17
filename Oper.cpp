#include "Server.hpp"

void Server::oper_command(Client &client, std::string command, int &clientSocket)
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
		return;
	}
}