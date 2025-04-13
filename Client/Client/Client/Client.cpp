#include <iostream>
#include <thread>
#include <winsock2.h>
#include <string>
#include <ws2tcpip.h>
#pragma comment(lib,"WS2_32")

using namespace std;

void receive_data(SOCKET client_socket) {
    char buffer[1024];
    while (true) {
        ZeroMemory(buffer, 1024);
        int bytesReceived = recv(client_socket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            break;
        }
        std::cout << "SERVER> " << buffer << std::endl;
    }
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        std::cerr << "Blad WSAStartup" << std::endl;
        return 0;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Nie mozna utworzyc gniazda" << std::endl;
        return 0;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &(hint.sin_addr));

    int connResult = connect(client_socket, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        std::cerr << "Nie mozna polaczyc z serwerem" << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }

    std::thread receiveThread(receive_data, client_socket);

    char buf[4096];
    std::string userInput;

    do {
        std::getline(std::cin, userInput);
        if (userInput.size() > 0) {
            if (userInput == "q") {
                break;
            }
            int sendResult = send(client_socket, userInput.c_str(), userInput.size() + 1, 0);
        }
    } while (userInput.size() > 0);

    receiveThread.detach();

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
