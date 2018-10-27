# importing required python packages #
import socket
import queue
import threading
import os
from django.utils import timezone

# adding Django settings to this python file to work properly inside a Django project
# since this file is a pure python and can work stand-alone
from django.core.wsgi import get_wsgi_application

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'api.settings')
application = get_wsgi_application()

# importing models
from report_app.models import Point

# initialize variables
HOST = "0.0.0.0"  # open to the world!
PORT = 7777  # just a random port
buffer = queue.Queue()  # consider a queue for data
max_index = -1
first_packet = True
stored_data = [-1] * 10


# receive data from sensor and add it to a buffer
def receive():
    global buffer  # tell this thread to use the global buffer
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # define server socket and it's type
    # make the socket reusable at same port, because the sensor hardware doesnt close the socket normaly
    # and throw an exception so the port stays open and not usable
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))  # assign host and port to socket
    server_socket.listen(1)  # just accepts one client since I have only one sensor
    print("---- SERVER STARTEDddddddd ----")  # just tell that socket is ready and listening
    client_socket, (client_address, client_port) = server_socket.accept()  # accept client and save it's address + port
    print('request from ', str(client_address), ':', str(client_port))  # tell that someone connected to socket
    while True:  # always accept new data from client
        buffer.put(client_socket.recv(510))  # add new received data to buffer /// each packet of data is max 510 bytes
        # print('receive ', buffer.qsize())
        # if len(received_data) > 501:
        #     print('fuck you ', received_data[509])
        # else:
        #     print('--- ', received_data[500])


# read data from buffer and process and insert it to the database
def save():
    while True:  # it's a thread

        if not buffer.empty():  # read till there's no data waiting for process

            received_data = buffer.get()  # pop one packet of data
            processed_data = []  # data needs to be processed before adding to databse so consider a variable for it
            packet_number = received_data[500]
            # print('save ', buffer.qsize())
            # we receive two kind of packets, first with 510 bytes of data and the second with 501 bytes
            # this is a limitation from sensor hardware so I need to handle it here
            # if len(received_data) > 501:  # first kind of packets
            #     # print('last byte of 510 bytes packets (packet number) ', received_data[509],  received_data[508],  received_data[507],  received_data[506],  received_data[505],  received_data[504],  received_data[503],  received_data[502],  received_data[501],  received_data[500], received_data[499], received_data[498])
            #     # for i in range(0, 509):
            global first_packet, max_index, stored_data
            if first_packet and packet_number == 9:
                first_packet = False
                pass
            else:
                print(packet_number)
                for i in range(0, 500):
                    # each two bytes (16 bit) creates one point
                    # each point is a 4 char number from 0 to 4096
                    # sensor has a 12 bit output (but hardware should send 16 bit, so we have 0 for first 4 bits)
                    # below I create a  point from 2 bytes
                    if i % 2 == 0:
                        data = received_data[i] + (received_data[i + 1] * 256)
                        if data > 4096:
                            print('noise data : ', data)
                            break
                        # now that we have a nice 4 char readable number which is our Point!
                        # we assign it to the Point model and then we append it to list of processed data
                        processed_data.append(Point(value=data, datetime=timezone.now()))
                        # processed_data.append(data)
                        # Point.objects.create(value=data)
                        # print(data)
                # print(processed_data[0].value)
                if packet_number >= 0 and packet_number <= max_index:
                    for i, data in enumerate(stored_data):
                        if data == -1:
                            prv_index = i - 1
                            while stored_data[prv_index] == -1:
                                prv_index -= 1
                            stored_data[i] = stored_data[prv_index]
                    abc = []
                    for data in stored_data:
                        abc.extend(data)
                    Point.objects.bulk_create(abc)  # insert processed data list to the database

                    stored_data = [-1] * 10
                    max_index = -1
                    stored_data[packet_number] = processed_data.copy()
                else:
                    max_index = max(max_index, packet_number)
                    stored_data[packet_number] = processed_data.copy()


# our program has two threads, one for receiving data and one for saving it to database
receive_thread = threading.Thread(target=receive)
save_thread = threading.Thread(target=save)

# starting threads
receive_thread.start()
save_thread.start()
