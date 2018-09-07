import socket
import queue
import threading
import datetime

# from datetime import datetime
import os
from time import sleep

from django.core.wsgi import get_wsgi_application
from django.utils import timezone

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'api.settings')

application = get_wsgi_application()

from report_app.models import Point

HOST = "0.0.0.0"
PORT = 7777

q = queue.Queue()

print("---- SERVER STARTED ----")


def receive():
    global q
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    client_socket, (client_address, client_port) = server_socket.accept()
    print('request from', str(client_address), str(client_port))
    while True:
        # received_data = client_socket.recv(510)
        # if q.qsize() > 5:
        #     break
        q.put(client_socket.recv(510))
        print('receive ',q.qsize())
        # break
        # if len(received_data) > 501:
        #     print('fuck you ', received_data[509])
        # else:
        #     print('--- ', received_data[500])


def save():
    while True:
        if not q.empty():
            received_data = q.get()
            processed_data = []
            print('save ',q.qsize())
            if len(received_data) > 501:
                # print('fuck you ', received_data[509])
                for i in range(0, 509):
                    if i % 2 == 0:
                        data = received_data[i] + (received_data[i + 1] * 256)
                        if data > 4096:
                            break
                        processed_data.append(Point(value=data, datetime = timezone.now()))
                        # processed_data.append(data)
                        # Point.objects.create(value=data)
                        # print(data)
                        # print(data)
            else:
                # print('--- ', received_data[500])
                for i in range(0, 500):
                    if i % 2 == 0:
                        data = received_data[i] + (received_data[i + 1] * 256)
                        if data > 4096:
                            break
                        # processed_data.append(data)
                        processed_data.append(Point(value=data, datetime = timezone.now()))
                        # Point.objects.create(value=data)
                        # print(data)
            # print(processed_data)
            # aList = [Point(value=val, datetime = time) for val, time in processed_data]
            # print(aList)
            Point.objects.bulk_create(processed_data)

receive_thread = threading.Thread(target=receive)
save_thread = threading.Thread(target=save)

receive_thread.start()
save_thread.start()
