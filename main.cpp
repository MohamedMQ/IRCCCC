#include "Server.hpp"

Server server("", 0);
void signal_handler(int sig)
{
	server.eraseAllClients();
	exit(0);
}

int passwordHandler(char *pass) {
	int Digit = 0;
	int LowerCase = 0;
	int UpperCase = 0;
	int Symbol = 0;
	std::string password(pass);

	if (password.size() < 8 || password.size() > 20)
		return 0;
	for (int i = 0; i < password.size(); i++) {
		if (!UpperCase && (password[i] >= 'A' && password[i] <= 'Z'))
			UpperCase++;
		else if (!LowerCase && (password[i] >= 'a' && password[i] <= 'z'))
			LowerCase++;
		else if (!Digit && (password[i] >= '0' && password[i] <= '9'))
			Digit++;
		else if (!Symbol && ((password[i] >= 32 && password[i] <= 47)
				|| (password[i] >= 32 && password[i] <= 47)
				|| (password[i] >= 58 && password[i] <= 64)
				|| (password[i] >= 91 && password[i] <= 96)
				|| (password[i] >= 123 && password[i] <= 126)))
			Symbol++;
	}
	if (!(Symbol && Digit && LowerCase && UpperCase))
		return 0;
	return 1;
}

int main(int ac, char **av)
{
	int socket;
	signal(SIGINT, signal_handler);

	if (ac != 3) {
		std::cerr << "\033[31mWrong parametres !!!\033[0m\nUsage: ./ft_irc port password\n";
		return (0);
	}
	if (!passwordHandler(av[2])) {
		std::cerr << "\033[31mWrong password !!!\033[0m\n\033[33mPassword should be:\033[0m\nSize greater than 7\nSize Lower than 21 characters\n\
At least an uppercase character\nAt least a lowercase character\n\
At least a special character\n";
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
