#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

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