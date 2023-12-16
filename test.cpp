// <<<<<<< HEAD
// // <<<<<<< HEAD

// // // #include <iostream>
// // // #include <vector>


// // // int main()
// // // {
// // // 	std::vector<int> vec;
// // // 	vec.push_back(1);
// // // 	vec.push_back(2);
// // // 	vec.push_back(3);
// // // 	vec.push_back(4);
// // // 	for (int i = 0; i < vec.size(); i++)
// // // 	{
// // // 		if (vec[i] == 2)
// // // 			vec.erase(vec.begin() + i);
// // // 	}
// // // 	for (int i = 0; i < vec.size(); i++)
// // // 	{
// // // 		std::cout << 
// // // 	}

// // // }

// // =======
// // >>>>>>> 8ce2f2e4478ff64fce39d3d860f7ae87c1041859
// =======
// >>>>>>> rida_branch
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
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
// <<<<<<< HEAD
// #include <vector>
// // <<<<<<< HEAD
// // #include <arpa/inet.h>

// // int main() {
// //     // Create a socket
// //     int serverSock = socket(AF_INET, SOCK_STREAM, 0);
// //     if (serverSock == -1) {
// //         perror("socket");
// //         return 1;
// //     }

// //     // Set up the server address
// //     sockaddr_in serverAddr;
// //     std::memset(&serverAddr, 0, sizeof(serverAddr));
// //     serverAddr.sin_family = AF_INET;
// //     serverAddr.sin_addr.s_addr = INADDR_ANY;
// //     serverAddr.sin_port = htons(12345);

// //     // Bind the socket
// //     if (bind(serverSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
// //         perror("bind");
// //         close(serverSock);
// //         return 1;
// //     }

// //     // Listen for incoming connections
// //     if (listen(serverSock, 5) == -1) {
// //         perror("listen");
// //         close(serverSock);
// //         return 1;
// //     }

// //     std::cout << "Server listening on port 12345\n";

// //     // Set the socket to non-blocking mode
// //     // int flags = fcntl(serverSock, F_GETFL, 0);
// //     // flags |= O_NONBLOCK; // Add O_NONBLOCK to the flags
// //     // fcntl(serverSock, F_SETFL, flags);

// //     // Set up the pollfd structure
// //     pollfd fds[1];
// //     fds[0].fd = serverSock;
// //     fds[0].events = POLLIN;

// //     // Blocking mode
// //     // std::cout << "Blocking mode:\n";
// //     // poll(fds, 1, -1);
// //     // std::cout << "Event occurred!\n\n";

// //     // Non-blocking mode
// //     std::cout << "Non-blocking mode:\n";
// //     poll(fds, 1, 0);
// //     std::cout << "If no event occurred, this message will be printed immediately.\n";

// //     // Clean up
// //     close(serverSock);

// //     return 0;
// // }
// // =======
// // using namespace std;
// // #include <ctime>

// // void count_age(int actual_day, int actual_mounth, int actual_year,int birthday, int birth_mounth, int birth_year)
// // {
// // 	int day;
// // 	int mounth;
// // 	int year;
// // 	int md[] = {31,28,31,30,31,30,31,31,30,31,30,31};
// // 	year = actual_year - birth_year;
// // 	if (actual_mounth < birth_mounth)
// // 	{
// // 		year--;
// // 		mounth = 12 - (birth_mounth - actual_mounth);
// // 	}
// // 	else
// // 		mounth = actual_mounth - birth_mounth;
// // 	if (actual_day < birthday)
// // 	{
// // 		mounth--;
// // 		day = md[actual_mounth - 1] - (birthday - actual_day);
// // 	}
// // 	else
// // 	day = actual_day - birthday;
// // 	std::cout << "your age is : ";
// // 	std::cout << year << " years " << mounth << " months " << day << " days. ";
// // }

// // int get_mounth(std::string mounths[], std::string mounth)
// // {
// // 	for (int i = 0; i < 12; i++)
// // 	{
// // 		if (mounths[i] == mounth)
// // 			return i + 1;
// // 	}
// // 	return 0;
// // }

// // int main(int argc, char **argv)
// // {
// // 		time_t now = time(0);
// // 		char* dt = ctime(&now);
// // 	std::cout << dt;
// //   return 0;
// // }
// // >>>>>>> 8ce2f2e4478ff64fce39d3d860f7ae87c1041859

// int main() {
//     long i = std::atol("922337203685477580009");
//     std::cout << i << std::endl;
// }
// =======
// #include <vector>z
// using namespace std;
// #include <ctime>

// int main(int argc, char **argv)
// {
// 	std::string a = ""
//   return 0;
// }

// >>>>>>> rida_branch


#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Get the bound IP address
    if (getsockname(serverSocket, (struct sockaddr *)&serverAddr, &addrLen) == -1) {
        perror("Getsockname failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);

    printf("Server IP address: %s\n", ipAddress);

    // Close the socket
    close(serverSocket);

    return 0;
}
