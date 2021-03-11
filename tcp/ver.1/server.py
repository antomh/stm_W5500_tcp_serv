# coding: cp1251
import socket
import time
# открытие сокета, тип TCP (SOCK_STREAM)
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# задание прослушиваемого порта сервера
tcp_socket.bind(('localhost', 10000))
# прослушивание входящих соединений, максимум 1
tcp_socket.listen(1)
# бесконечный цикл
while True:
# задание тайм-аута
    tcp_socket.settimeout(2)
    # конструкция try: ... except: для обработки исключений
    try:
        # приём входящего подключения
        conn,addr = tcp_socket.accept()
        # получение 2 байт
        data = conn.recv(2)
        # если не получены данные - закрытие соединения
        if not data:
            conn.close()
            break
        # конвертация полученных от клиента байт в число
        n = int.from_bytes(data,byteorder='big')
        print("Получено от клиента: ", n)
        # конвертация числа в тип bytes (2 байта)
        data_send = (n).to_bytes(2, byteorder='big')
        # передача числа клиенту
        conn.send(data_send)
        # закрытие соединения
        conn.close()
    except socket.timeout:
        print("Тайм-аут")
# закрытие сокета
tcp_socket.close() 