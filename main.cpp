// #include "Server.hpp"

// int main(int ac, char **av)
// {
//     int socket;

//     Server server("", 0);
//     server.setPass(av[2]);
//     server.setPort(av[1]);
//     socket = server.CreateSocketConnection();
//     if(listen(socket, server.getMaxClients()) == -1) {
//         std::cerr << "Error/nlisten failed" << std::endl;
//         return (0);
//     }
//     server.setServerSock(socket);
//     server.ServerRun();
// }