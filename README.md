# Chat-room c++ application / messenger
<hr>

## Client & Server

Приложение построено на клиент-серверной архитектуре, при которой множество клиентов, желающих попасть в "комнату" подключаются к одному и тому же серверу.

На одном из компьютеров запускается сервер - `chat-room/server` (базовая версия) или `chat-room/legacy/c-server`, затем приложения-клиенты подключаются к нему через ip-адрес (**loopback**, по-умолчанию **IPv4**) и порт (по-умолчанию **8080**)

<br>

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

<br>

## Второй способ установки (ручная компиляция)

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

<br>

### Windows

> *В примерах используется PowerShell*

* Скачать и установить `MingW` (https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download), пакет g++
  
* Скачать и установить `cmake` (https://cmake.org/download/)
![2025-04-13-02:00:04-screenshot](https://github.com/user-attachments/assets/325a3cd2-409f-46e9-bcce-1ba70796bfbe)

* Скачать и установить git - необходим для библиотек с FetchContent cmake’а (официальный сайт Git: https://git-scm.com/downloads/win)

* Проверить установленные библиотеки вы можете, выполнив команды:

  ```sh
  git --version
  cmake --version
  g++ --version
  ```
  
* Загрузить папку проекта (через `git clone` или ZIP архив)

* Переместиться в папку проекта из консоли, скомпилировать:

`chat-room/client` & `chat-room/server` (в каждой из папок проекта выполнить команды ниже)
```sh
cmake -B build -S .
cmake --build build
```

* CMake создаст исполняемые файлы в папках `app/build/Debug/app.exe` (где "app" - клиент или сервер)

<hr>

<br>

## legacy папка

в папке legacy расположена упрощённая версия приложения, написанная на Си.

Компилируется стандартным образом `gcc server.c` или `gcc client.c`

в этой версии клиент реализован через `select()`, поэтому при написании сообщения собеседнику блокируется главный поток.

<br>

*Порт* сервера можно изменить в `chat-room/server/server.cpp`
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

<br>

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
