import csv

input_file_name = 'trimmer.csv'
train_x_file_name = 'trimmer_x_train.csv'
test_x_file_name = 'trimmer_x_test.csv'
train_y_file_name = 'trimmer_y_train.csv'
test_y_file_name = 'trimmer_y_test.csv'
train_size = 2500
test_size = 1000
window_size = 128
category_code = '3'

with open(input_file_name, newline='') as train_input, open(train_x_file_name, 'w', newline='') as train_output:
    reader = csv.reader(train_input)
    writer = csv.writer(train_output)
    data = []
    for row in reader:
        data.append(row[1])
    for j in range(0, train_size):
        window = []
        print(int((j * 100) / train_size))
        for i in range(0, window_size):
            window.append(data.pop(0))
        data.append('1')
        writer.writerow(window)

with open(test_x_file_name, 'w', newline='') as test_output:
    writer = csv.writer(test_output)
    for j in range(0, test_size):
        window = []
        print(int((j * 100) / test_size))
        for i in range(0, window_size):
            window.append(data.pop(0))
        writer.writerow(window)


with open(train_y_file_name, 'w', newline='') as y_output:
    writer = csv.writer(y_output)
    for i in range(0, train_size):
        writer.writerow(category_code)

with open(test_y_file_name, 'w', newline='') as y_output:
    writer = csv.writer(y_output)
    for i in range(0, test_size):
        writer.writerow(category_code)