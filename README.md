# Messanger C++ Application

<hr>

## Client & Server

Приложение построено на клиент-серверной архитектуре, при которой множество клиентов, желающих попасть в "комнату" подключаются к одному и тому же серверу.

На одном из компьютеров запускается сервер - `chat-room/server` (базовая версия) или `chat-room/legacy/c-server`, затем приложения-клиенты подключаются к нему через ip-адрес (**loopback**, по-умолчанию **IPv4**) и порт (по-умолчанию **8080**)


*Порт* можно изменить в `chat-room/server/server.cpp`
```cpp
class ChatServer {
private:
    inline static const char PORT[] = "8080";

```

или в `chat-room/legacy/c-server/server.cpp`
```c
    const char PORT[] = "8080";

    int main() {
```

<hr>

## Исполнение

```sh
./server     # Linux/MacOS
./server.exe # Windows
```

```sh
./client <host> <port>     # Linux/MacOS
./client.exe <host> <port> # Windows
```

<hr>

## Быстрая установка

Вы можете быстро получить исполняемые файлы для вашей ОС из вкладки **"Release"**

### Linux / Mac OS

* Установить нужный бинарный файл (сервер или клиент)

* Дать исполняемому файлу права

```sh
chmod +x <file>
```

### Windows

* Установить нужный .exe файл (сервер или клиент)

* Запускать исполняемый файл от имени администратора

<hr>

## Ручная компиляция

* Необходимы пакеты cmake, gcc, opengl

### Linux / MacOS

```sh
cd /path/to/folder
git clone --branch main git@github.com:ossla/chat-room.git . 
```

#### * `server`

```sh
cd server
chmod +x build.sh
./build.sh && cd build
./server
```

#### *  `client`

```sh
cd client
chmod +x build.sh
./build.sh && cd build
./client <host> <port>
```

### Windows

* Скачать и установить `MingW` (https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download) 
* Скачать и установить `cmake` (https://cmake.org/download/)

<hr>

## `chat-room/legacy`

в папке legacy расположена упрощённая версия приложения, написанная на Си,
компилируется стандартным образом `gcc server.c` или `gcc client.c`

в этой версии клиент реализован через `select()`, поэтому при написании сообщения собеседнику блокируется поток, слушающий сервер (отсутствие многопоточности).