#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 65534     // Порт для прослушивания входящих подключений

// Функция обработки клиента: принимает данные и записывает их в файл.
void *handle_client(void *arg) {
    int client_socket = *(int *)arg;                         // Получаем дескриптор сокета клиента из аргумента потока.
    free(arg);                                               // Освобождаем память после передачи сокета.

    char buffer[1024];                                       // Буфер для чтения данных от клиента.
    char client_file[256];                                   // Имя файла для логов клиента.
    snprintf(client_file, sizeof(client_file), "/tmp/client_%d.log", client_socket);

    FILE *logfile = fopen(client_file, "a");        // Открываем файл для записи логов.
    if (!logfile) {           // Проверяем успешность открытия файла.
        perror("Ошибка открытия файла");
        close(client_socket);
        return NULL;
    }

    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, logfile);          // Записываем полученные данные в файл.
        fflush(logfile);       // Сбрасываем буфер файла на диск.
    }

    fclose(logfile);      // Закрываем файл после завершения приема данных.
    close(client_socket);  // Закрываем соединение с клиентом.
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);     // Создаем TCP-сокет.
    if (server_socket < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    struct sockaddr_in server_addr;                         // Структура адреса сервера.
    server_addr.sin_family = AF_INET;                       // Используем IPv4.
    server_addr.sin_port = htons(PORT);                     // Указываем порт сервера.
    server_addr.sin_addr.s_addr = INADDR_ANY;               // Принимаем подключения на всех интерфейсах.

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {         // Начинаем прослушивание входящих соединений.
        perror("Ошибка прослушивания");
        close(server_socket);
        exit(1);
    }

    printf("Сервер запущен и ожидает подключений...\n");

    while (1) {
        int *client_socket = malloc(sizeof(int));           // Выделяем память под дескриптор нового клиента.
        *client_socket = accept(server_socket, NULL, NULL);

        if (*client_socket < 0) {       // Проверяем успешность подключения клиента.
            perror("Ошибка принятия подключения");
            free(client_socket);
            continue;
        }

        pthread_t client_thread;     // Создаем новый поток для обработки клиента.
        pthread_create(&client_thread, NULL, handle_client, client_socket);
        pthread_detach(client_thread);
    }

    // Закрываем серверный сокет при завершении работы программы.
    close(server_socket);
    return 0;
}
