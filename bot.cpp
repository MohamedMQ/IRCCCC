#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <map>
#include <vector>
#include <arpa/inet.h>

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

	std::vector<int> count_age(int actual_day, int actual_mounth, int actual_year,int birthday, int birth_mounth, int birth_year)
	{
		int day;
		int mounth;
		int year;
		int mounts[] = {31,28,31,30,31,30,31,31,30,31,30,31};
		std::vector<int> ints;
		if (birthday > mounts[birth_mounth - 1])
		{
			ints.push_back(-1337);
			return ints;
		}
		year = actual_year - birth_year;
		if (actual_mounth < birth_mounth)
		{
			mounth = 12 - (birth_mounth - actual_mounth);
			year--;
		}
		else
			mounth = actual_mounth - birth_mounth;
		if (actual_day < birthday)
		{
			day = mounts[actual_mounth - 1] - (birthday - actual_day);
			mounth--;
		}
		else
			day = actual_day - birthday;
		ints.push_back(day);
		ints.push_back(mounth);
		ints.push_back(year);
		return ints;
	}

	int get_mounth(std::string mounths[], std::string mounth)
	{
		for (int i = 0; i < 12; i++)
		{
			if (mounths[i] == mounth)
				return i + 1;
		}
		return 0;
	}

	void age_bot(char *birth_date, std::string client_name, int &clientSocket)
	{
		std::vector<int> ints;
		std::string response;
		int bytes_sent;
		char *str;
		char *str2;
		int actual_mounth;
		int actual_day;
		int actual_year;
		std::string mounts[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		std::vector<char *> tokens;
		time_t now = time(0);
		char* dt = ctime(&now);
		std::vector<std::string> tokens2;
		str2 = strtok(dt, " ");
		while (str2 != NULL)
		{
			tokens2.push_back(str2);
			str2 = strtok(NULL, " ");
		}
		actual_mounth = get_mounth(mounts, tokens2[1]);
		actual_day = atoi(tokens2[2].c_str());
		actual_year = atoi(tokens2[4].c_str());
		str = strtok(birth_date, "-");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, "-");
		}
		if (tokens.size() < 3 || !check_is_int(tokens[0]) || !check_is_int(tokens[1]) || !check_is_int(tokens[2])
			|| atol(tokens[0]) < 1 || atol(tokens[0]) > 31 || atol(tokens[1]) < 1
			|| atol(tokens[1]) > 12 || atol(tokens[2]) > actual_year)
		{
			response = "PRIVMSG " + client_name + " " + "Invalid format: dd-mm-yy" +  " \r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
		ints = count_age(actual_day, actual_mounth, actual_year, atoi(tokens[0]), atoi(tokens[1]), atoi(tokens[2]));
		if (ints[0] > 0)
		{
			response = "PRIVMSG " + client_name + " :your age is : " + std::to_string(ints[2]) + " years " + std::to_string(ints[1]) + " months " + std::to_string(ints[0]) + " days\r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		}
		else
		{
			response = "PRIVMSG " + client_name + " " + "Invalid day" +  " \r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
			return;
		}
	}

	void nickname_bot(std::string client_name, int &clientSocket)
	{
		std::string nicknames;
		std::string response;
		std::string names[] = {"ali", "amir", "hamza", "rida", "mohamed", "hassan", "karim", "malik", "omar", "samir", "zayn", "akram", "bilal", "daniyal", "farid", "marwan", "qasim", "suhail", "ahmed", "aziz", "hicham", "makram", "soultan", "nabil", "adil", "anas", "badr", "habib", "hadi", "jebril", "naji", "nizar", "tarik", "zaki", "", "assem", "fouad", "haroun", "jalal", "khalid", "mustapha", "saif", "oussama", "said", "ayoub", "fakhr", "issam", "laith"};
		int bytes_sent;
		int ran1;
		int ran2;
		int ran3;
		int ran4;
		response = "PRIVMSG " + client_name + " " + "Choose a unique nickname from the ones below :" +  " \r\n";
		bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
		for (int i = 0; i < 10; i++)
		{
			ran3 = std::rand() % 2;
			if (ran3 == 0)
			{
				ran1 = std::rand() % 49;
				ran2 = std::rand() % 49;
				nicknames += names[ran1] + "_" + names[ran2];
			}
			else
			{
				ran1 = std::rand() % 49;
				ran4 = std::rand() % 2;
				if (ran4 == 0)
					nicknames += "_" + names[ran1];
				else
					nicknames += names[ran1] + "_";
			}
			if (i < 9)
				nicknames += " , ";
		}
			response = "PRIVMSG " + client_name + " " + nicknames +  " \r\n";
			bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
	}

	std::vector<int> pars_bot_command(std::string command, int &clientSocket)
	{
		char *str;
		std::vector<std::string> tokens;
		std::string response;
		int bytes_sent;
		std::vector<int> ints;
		str = strtok((char *)(command.c_str()), " ");
		while (str != NULL)
		{
			tokens.push_back(str);
			str = strtok(NULL, " ");
		}
		std::cout << "size : " << tokens.size() << std::endl;


		if (tokens.size() >= 1)
		{
			if (tokens[0] == "my_age")
			{
				if (tokens.size() >= 3)
					age_bot((char *)tokens[1].c_str(), tokens[2], clientSocket);
				else
				{
					response = "PRIVMSG " + tokens[2] + " " + "Invalid format: dd-mm-yy" +  " \r\n";
					bytes_sent = send(clientSocket, response.c_str(), response.size(), 0);
				}
			}
			else if (tokens[0] == "nickname")
			{
				nickname_bot(tokens[1], clientSocket);
			}
		}
		else
			std::cout << "Please enter a flaag" << std::endl;
		return ints;
	}

int main(int ac, char **av) {
	std::string _saveSemiCommands;
	struct sockaddr_in serverAddr;
	std::string _command;
	std::vector<int> ints;
	struct pollfd *_pollFd;
	int clientSocket;
	char buffer[1024];
	int flag;

	std::memset(&serverAddr, 0, sizeof(serverAddr));
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cout << "Error\nfailed connecting to the server\n";
		exit (-1);
	} else
	{
		std::cout << "Success\nsuccessed connecting to the server\n";
		send(clientSocket, "PASS popo\r\n", strlen("PASS popo\r\n"), 0);
		send(clientSocket, "NICK BOT\r\n", strlen("NICK BOT\r\n"), 0);
		send(clientSocket, "USER BOT 0 * BOT\r\n", strlen("USER BOT 0 * BOT\r\n"), 0);
	}
	flag = fcntl(clientSocket, F_GETFL, 0);
	fcntl(clientSocket, F_SETFL, flag | O_NONBLOCK);
	_pollFd = new struct pollfd;
	_pollFd->fd = clientSocket;
	_pollFd->events = POLLIN;
	_pollFd->revents = 0;
	int response = 0;
	while (1) {
		response = poll(_pollFd, 1, -1);
		if (response == -1) {
			std::cout << "Error\npoll failed\n";
			return -1;
		}
		if (_pollFd->revents & POLLIN) {
			memset(buffer, 0, sizeof(buffer));
			response = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (response == -1) {
				std::cout << "Error\nrecv failed: " << response << std::endl;
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					return -1;
				} else if (response == 0) {
					std::cout << "the server is closed for now, try later\n";
					free(_pollFd);
					close(clientSocket);
					return -1;
				} else {
					std::string receivedData(buffer, response);
					std::string &partialCommand = _saveSemiCommands;
					partialCommand += receivedData;

					if (!partialCommand.empty()) {
						size_t newlinePos = partialCommand.find('\n');
						while (newlinePos != std::string::npos) {
							_command = partialCommand.substr(0, newlinePos);
							partialCommand = partialCommand.substr(newlinePos + 1);
							std::cout << _command << std::endl;
							ints = pars_bot_command(_command, clientSocket);
							newlinePos = partialCommand.find('\n');
						}
					}
				}
		}
	}
	return 0;
}
