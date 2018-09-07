import socket
from datetime import datetime
import os

from django.core.wsgi import get_wsgi_application

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'api.settings')

application = get_wsgi_application()

from report_app.models import Point

HOST = "0.0.0.0"
PORT = 7777

print("---- SERVER STARTED ----")


def server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen(1)
    client_socket, (client_address, client_port) = server_socket.accept()
    print('request from', str(client_address), str(client_port))
    # file = open("data.txt", "w")
    cnt=1
    while True:
        received_data = client_socket.recv(510)
        cnt+=1
        if not received_data: break
        # print(received_data[500]) # this is packet number , i don't need it
        # print(received_data,datetime.now())
        for i in range(0, 500):
            # print(received_data[i] + (received_data[i+1]*256))
            if i % 2 == 0:
                data = received_data[i] + (received_data[i + 1] * 256)
                print(cnt," : ",i,i+1,received_data[i],(received_data[i + 1]), data)
                # file.write(str(data))
                # file.write("\n")
                Point.objects.create(value=data)
                print(cnt, " : ", i, i + 1, received_data[i], (received_data[i + 1]), data)
                # print(data)
            # if i % 2 != 0:
            #     print('fuck youuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu')
                # print(datetime.now())
        print("********************************************")
        # server_socket.close()
        # break
        # print('data : ', received_data)


server()
