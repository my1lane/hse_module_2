//
//  main.m
//  hseууу
//
//  Created by Alexander on 25.12.2024.
//

#include <Cocoa/Cocoa.h> // Подключение фреймворка Cocoa для работы с GUI на macOS
#include <AVFoundation/AVFoundation.h> // Подключение фреймворка AVFoundation для работы со звуком

// Функция для воспроизведения звука
void playBeepSound() {
    @autoreleasepool { // Создание собственного пула автоматического управления памятью
        NSString *path = [[NSBundle mainBundle] pathForResource:@"beep" ofType:@"wav"]; // Поиск пути к файлу beep.wav в ресурсах приложения
        if (!path) { // Проверка, найден ли файл
            NSLog(@"Файл beep.wav не найден"); // Вывод сообщения об ошибке, если файл не найден
            return; // Завершение функции, если путь не найден
        }
        NSURL *url = [NSURL fileURLWithPath:path]; // Создание URL-объекта для файла
        NSError *error = nil; // Переменная для хранения возможной ошибки
        AVAudioPlayer *player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error]; // Инициализация аудиоплеера с указанным файлом
        if (error) { // Проверка, возникла ли ошибка при инициализации плеера
            NSLog(@"Ошибка воспроизведения звука: %@", error); // Вывод сообщения об ошибке
            return; // Завершение функции при ошибке
        }
        [player setNumberOfLoops:-1]; // Установка бесконечного повторения звука (-1 означает бесконечный цикл)
        [player play]; // Запуск воспроизведения звука
    }
}

// Основная функция приложения
int main(int argc, const char *argv[]) {
    @autoreleasepool { // Создание пула автоматического управления памятью
        NSApplication *app = [NSApplication sharedApplication]; // Создание объекта приложения

        // Создание окна приложения
        NSRect frame = NSMakeRect(0, 0, 800, 600); // Задание размеров окна (ширина 800, высота 600)
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable; // Установка стиля окна (заголовок, возможность закрытия и изменения размера)
        NSWindow *window = [[NSWindow alloc] initWithContentRect:frame 
                                                       styleMask:style 
                                                         backing:NSBackingStoreBuffered 
                                                           defer:NO]; // Инициализация окна с заданными параметрами
        [window setTitle:@"Приложение на C"]; // Установка заголовка окна
        [window makeKeyAndOrderFront:nil]; // Отображение окна на переднем плане

        // Установка синего фона для содержимого окна
        NSView *contentView = [window contentView]; // Получение представления содержимого окна
        [contentView setWantsLayer:YES]; // Включение поддержки слоев для представления
        contentView.layer.backgroundColor = [[NSColor blueColor] CGColor]; // Установка синего цвета фона

        // Добавление текста в окно приложения
        NSTextField *label = [[NSTextField alloc] initWithFrame:NSMakeRect(200, 300, 400, 50)]; // Создание текстового поля с заданным размером и положением
        [label setStringValue:@"Отправьте деньги и получите пароль"]; // Установка текста в текстовом поле
        [label setFont:[NSFont systemFontOfSize:24]]; // Установка размера шрифта текста
        [label setTextColor:[NSColor whiteColor]]; // Установка белого цвета текста
        [label setAlignment:NSTextAlignmentCenter]; // Выравнивание текста по центру
        [label setBezeled:NO]; // Отключение рамки вокруг текстового поля
        [label setDrawsBackground:NO]; // Отключение фона текстового поля
        [label setEditable:NO]; // Отключение возможности редактирования текста пользователем
        [label setSelectable:NO]; // Отключение возможности выделения текста пользователем
        [contentView addSubview:label]; // Добавление текстового поля в содержимое окна

        // Добавление поля ввода пароля в окно приложения
        NSSecureTextField *passwordField = [[NSSecureTextField alloc] initWithFrame:NSMakeRect(300, 250, 200, 30)]; // Создание защищенного текстового поля для ввода пароля с заданным размером и положением
        [passwordField setPlaceholderString:@"Введите пароль"]; // Установка строки-заполнителя (placeholder) для поля ввода пароля
        [contentView addSubview:passwordField]; // Добавление поля ввода пароля в содержимое окна

        playBeepSound(); // Воспроизведение звукового сигнала

        [app run]; // Запуск основного цикла обработки событий приложения
    }
    return 0; // Завершение программы с кодом возврата 0 (успешное выполнение)
}
