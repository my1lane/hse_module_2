#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 1024 // Максимальный размер принимаемого сообщения

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Порт для приема сообщений
    int udp_socket; // Дескриптор UDP-сокета
    struct sockaddr_in server_address; // Структура для хранения адреса сервера
    struct sockaddr_in client_address; // Структура для хранения адреса клиента
    char buffer[MAX_MESSAGE_SIZE]; // Буфер для принимаемых сообщений
    socklen_t client_address_length = sizeof(client_address); // Размер структуры адреса клиента

    // Создаем UDP-сокет
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера для привязки сокета
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_port = htons(port); // Указываем порт
    server_address.sin_addr.s_addr = INADDR_ANY; // Принимаем сообщения с любого адреса

    // Привязываем сокет к адресу и порту
    if (bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP Chat Server started. Let's see 0.o..." << std::endl;

    // Основной цикл приема сообщений
    while (true) {
        memset(buffer, 0, MAX_MESSAGE_SIZE); // Очищаем буфер перед приемом сообщения

        // Принимаем сообщение от клиента
        int bytes_received = recvfrom(udp_socket, buffer, MAX_MESSAGE_SIZE - 1, 0,
                                      (struct sockaddr *)&client_address, &client_address_length);
        if (bytes_received < 0) {
            perror("Recvfrom failed");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0'; // Завершаем строку нулевым символом

        // Выводим сообщение и информацию о клиенте в консоль
        std::cout << "Message from " << inet_ntoa(client_address.sin_addr)
                  << ": " << buffer << std::endl;

        // Если получено сообщение "exit", завершаем работу сервера
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(udp_socket); // Закрываем сокет перед завершением работы
    return 0;
}
