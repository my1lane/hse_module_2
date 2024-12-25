#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 1024 // Максимальный размер сообщения
#define BROADCAST_IP "255.255.255.255" // Широковещательный IP-адрес

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Порт для отправки сообщений
    int udp_socket; // Дескриптор UDP-сокета
    struct sockaddr_in broadcast_address; // Структура для хранения адреса широковещания
    char message[MAX_MESSAGE_SIZE]; // Буфер для отправляемого сообщения
    int enable_broadcast = 1; // Флаг для включения широковещания

    // Создаем UDP-сокет
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Включаем режим широковещания
    if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &enable_broadcast, sizeof(enable_broadcast)) < 0) {
        perror("Failed to set broadcast option");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес для отправки сообщений
    memset(&broadcast_address, 0, sizeof(broadcast_address));
    broadcast_address.sin_family = AF_INET; // IPv4
    broadcast_address.sin_port = htons(port); // Указываем порт
    broadcast_address.sin_addr.s_addr = inet_addr(BROADCAST_IP); // Указываем широковещательный адрес

    std::cout << "UDP Chat Client started. Type messages to send:" << std::endl;

    // Основной цикл отправки сообщений
    while (true) {
        std::cout << "> ";
        std::cin.getline(message, MAX_MESSAGE_SIZE); // Читаем сообщение от пользователя

        // Если пользователь ввел "exit", завершаем работу клиента
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Отправляем сообщение через широковещание
        if (sendto(udp_socket, message, strlen(message), 0,
                   (struct sockaddr *)&broadcast_address, sizeof(broadcast_address)) < 0) {
            perror("Sendto failed");
            exit(EXIT_FAILURE);
        }
    }

    close(udp_socket); // Закрываем сокет перед завершением работы
    return 0;
}
