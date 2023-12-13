
// #include <iostream>
// #include <vector>


// int main()
// {
// 	std::vector<int> vec;
// 	vec.push_back(1);
// 	vec.push_back(2);
// 	vec.push_back(3);
// 	vec.push_back(4);
// 	for (int i = 0; i < vec.size(); i++)
// 	{
// 		if (vec[i] == 2)
// 			vec.erase(vec.begin() + i);
// 	}
// 	for (int i = 0; i < vec.size(); i++)
// 	{
// 		std::cout << 
// 	}

// }

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
#include <vector>
#include <arpa/inet.h>

int main() {
    // Create a socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Bind the socket
    if (bind(serverSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        perror("bind");
        close(serverSock);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSock, 5) == -1) {
        perror("listen");
        close(serverSock);
        return 1;
    }

    std::cout << "Server listening on port 12345\n";

    // Set the socket to non-blocking mode
    // int flags = fcntl(serverSock, F_GETFL, 0);
    // flags |= O_NONBLOCK; // Add O_NONBLOCK to the flags
    // fcntl(serverSock, F_SETFL, flags);

    // Set up the pollfd structure
    pollfd fds[1];
    fds[0].fd = serverSock;
    fds[0].events = POLLIN;

    // Blocking mode
    // std::cout << "Blocking mode:\n";
    // poll(fds, 1, -1);
    // std::cout << "Event occurred!\n\n";

    // Non-blocking mode
    std::cout << "Non-blocking mode:\n";
    poll(fds, 1, 0);
    std::cout << "If no event occurred, this message will be printed immediately.\n";

    // Clean up
    close(serverSock);

    return 0;
}
