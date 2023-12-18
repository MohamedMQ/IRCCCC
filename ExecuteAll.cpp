#include "Server.hpp"

void Server::fill_client(std::string command, Client &client, int flag)
{
	char *str;
	std::string temp_command = command;
	std::vector<std::string> tokens;
	std::string realname;
	str = strtok((char *)(command.c_str()), " ");
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (flag == 5)
	{
		client.set_username(client.get_nickname());
		int pos = temp_command.find(tokens[4]);
		realname = temp_command.substr(pos);
		client.set_real_name(realname);
	}
	else if (flag == 10)
	{
		client.set_username(client.get_nickname());
		client.set_real_name(client.get_nickname());
	}
	else
	{
		client.set_username(tokens[1]);
		int pos = temp_command.find(tokens[4]);
		realname = temp_command.substr(pos);
		client.set_real_name(realname);
	}
}

void Server::executeAll(Client &client, std::string buffer, int &clientSocket)
{
	std::string buffer_temp = buffer;
	char *str;
	std::vector<std::string> tokens;
	int bytes_sent;
	str = strtok((char *)(buffer.c_str()), " ");
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	if (tokens[0] == "USER" && client.get_is_passF())
		user_command(buffer_temp, client, clientSocket);
	else if (tokens[0] == "NICK" && client.get_is_passF())
		nickname_command(buffer_temp, client, clientSocket);
	else if (tokens[0] == "PASS")
		pass_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "QUIT" && client.get_is_passF())
	{
		if (quit_command(clientSocket, buffer_temp) == -1)
			return;
	}
	else if (tokens[0] == "PRIVMSG" && requiredParams(client))
		privmsg_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "JOIN" && requiredParams(client))
		join_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "PART" && requiredParams(client))
		part_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "INVITE" && requiredParams(client))
		invite_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "MODE" && requiredParams(client))
		mode_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "TOPIC" && requiredParams(client))
		topic_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "KICK" && requiredParams(client))
		kick_command(client, buffer_temp, clientSocket);
	else if (tokens[0] == "PONG") {}
	else if (!requiredParams(client))
		params_requirements(client, clientSocket);
	else
	{
		std::string response = ":" + this->getServerName() + " 421 " + client.get_nickname() + tokens[0] + " :Unknown command\r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	}
}