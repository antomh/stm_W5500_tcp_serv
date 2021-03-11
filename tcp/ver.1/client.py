# coding: cp1251
import socket
import sys
import time
# задание адреса сервера и порта
addr = ('localhost', 10000)
# объявление переменных
n = 1; rec = 0
# бесконечный цикл
while True:
 # открытие сокета, тип TCP (SOCK_STREAM)
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 # конструкция try: ... except: для обработки исключений
    try:
 # установка соединения с сервером
        tcp_socket.connect(addr)
 # конвертация числа в тип bytes (2 байта)
        data_send = (n).to_bytes(2, byteorder = 'big')
 # передача числа на сервер
        tcp_socket.send(data_send)
 # получение 2 байт от сервера
        data = tcp_socket.recv(2)
 # конвертация полученных от сервера байт в число
        rec = int.from_bytes(data, byteorder = 'big')
        print("Получено от сервера: ", rec)
 # закрытие сокета
        tcp_socket.close()
 # увеличение счётчика
        n+=2
 # задержка 1 секунда
        time.sleep(1)
    except socket.error:
        print("Ошибка подключения!")
