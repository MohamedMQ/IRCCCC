#include "Server.hpp"

void Server::pass_command(Client &client, std::string _command, int &socket)
{
	std::string response;
	int bytes_sent;
	char *str;
	std::vector<std::string> tokens;
	str = strtok((char *)(_command.c_str()), " ");
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (tokens.size() < 2)
	{
		response = ":" + client.get_nickname() + " 464 " + client.get_nickname() + " :Password incorrect\r\n";
		bytes_sent = send(socket, response.c_str(), response.size(), 0);
		client.set__retry_pass();
	}
	else if (_command.substr(0, 4) == "PASS")
	{
		if (client.get_is_passF() == 0)
		{
			if (!std::strcmp(_command.c_str() + 5, _password.c_str()))
			{
				client.set_is_passF(1);
				return;
			}
			else
			{
				response = ":" + client.get_nickname() + " 464 " + client.get_nickname() + " :Password incorrect\r\n";
				bytes_sent = send(socket, response.c_str(), response.size(), 0);
				client.set__retry_pass();
			}
		}
		else
		{
			response = ":" + client.get_nickname() + " 462 " + client.get_nickname() + " :You may not reregister\r\n";
			bytes_sent = send(socket, response.c_str(), response.size(), 0);
		}
		if (client.get__retry_pass() == 3)
		{
			response = ":" + client.get_nickname() + " 465 " + client.get_nickname() + " :Too many incorrect password attempts. You are being kicked out.\r\n";
			bytes_sent = send(socket, response.c_str(), response.size(), 0);
			close(socket);
			_clients.erase(socket);
			socket = -1;
			return;
		}
	}
}
