#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

<<<<<<< HEAD
#define a 2147483647

int main() {
    std::cout << a + 1 << std::endl;
    // int serverSocket;
    // struct sockaddr_in serverAddr;
    // socklen_t addrLen = sizeof(serverAddr);

    // // Create a socket
    // serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // if (serverSocket == -1) {
    //     perror("Socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // // Bind the socket to an address and port
    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = INADDR_ANY;
    // serverAddr.sin_port = htons(8080);

    // if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    //     perror("Bind failed");
    //     close(serverSocket);
    //     exit(EXIT_FAILURE);
    // }

    // // Get the bound IP address
    // if (getsockname(serverSocket, (struct sockaddr *)&serverAddr, &addrLen) == -1) {
    //     perror("Getsockname failed");
    //     close(serverSocket);
    //     exit(EXIT_FAILURE);
    // }

    // char ipAddress[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &(serverAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);

    // printf("Server IP address: %s\n", ipAddress);

    // // Close the socket
    // close(serverSocket);

    // return 0;
}
//part
//part #chan
//part #chan,#chan2
//part #chan,chan2
//part #chan,#chan2 reason
//part #chan,chan2 reason
// JOIN #a,#b,#c,#d,#e

//invite
//invite rida
//invite rida chan
//invite rida #chan
//invite and

//+l or l is same
// ila tbeddel l khass i3awed itsifet msg
//  unknown mode char to me

//mode
//mode chan
//mode #chan i
//mode #chan +i
//mode #chan -i
//mode #chan +l
//mode #chan l
//mode #chan -l
//mode #chan -l param
//mode #chan +l num
//mode #chan +l 145
//mode #chan +l 1338
//mode #chan +o
//mode #chan +o makaynch,makaynch2
//mode #chan +o kayn
//mode #chan -o
//mode #chan -o kayn
//mode #chan +k
//mode #chan +k pass khte2
//mode #chan +k pass s7eeee7
//mode #chan -k pass  khate2
//mode #chan -k pass s7e7
//mode #chan + pass s7e7


// TOPIC
// TOPIC chan
// TOPIC #chan
// TOPIC chan msg
// TOPIC #chan msg
// TOPIC #chan - >> not op
=======
void sigint_handler(int signo) {
    printf("Caught SIGINT (Ctrl+C)\n");
    // Custom handling code can be added here
    exit(0);
}

int main() {
    // Set up the sigaction struct
    struct sigaction new_action, old_action;

    new_action.sa_handler = sigint_handler; // Specify the handler function
    sigemptyset(&new_action.sa_mask);      // Clear the signal mask
    new_action.sa_flags = 0;                // No special flags

    // Register the new action for SIGINT
    if (sigaction(SIGINT, &new_action, &old_action) < 0) {
        perror("sigaction");
        return 1;
    }

    // Infinite loop to keep the program running
    while (1) {
        // Your program logic here
    }

    return 0;
}
>>>>>>> rida_branch
