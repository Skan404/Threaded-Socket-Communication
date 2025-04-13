#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#pragma comment(lib,"WS2_32")

std::vector<SOCKET> client_sockets;

void handle_client(SOCKET client_socket) {
    char buffer[1024];
    while (true) {
        ZeroMemory(buffer, 1024);
        int bytesReceived = recv(client_socket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            break;
        }
        for (auto& socket : client_sockets) {
            if (socket != client_socket) {
                send(socket, buffer, bytesReceived + 1, 0);
            }
        }
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        std::cerr << "Blad WSAStartup" << std::endl;
        return 0;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Nie mozna utworzyc gniazda" << std::endl;
        return 0;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, 5);

    sockaddr_in client;
    int clientSize = sizeof(client);

    std::vector<std::thread> threads;

    while (true) {
        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        client_sockets.push_back(clientSocket);
        std::cout << "Polaczono z nowym klientem" << std::endl;
        threads.push_back(std::thread(handle_client, clientSocket));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    WSACleanup();

    return 0;
}
