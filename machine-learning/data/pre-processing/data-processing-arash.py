import csv

input_file_name = 'off.csv'
output_file_name = 'off-output.csv'
train_x_file_name = 'off-x-train.csv'
test_x_file_name = 'off-x-test.csv'
train_y_file_name = 'off-y-train.csv'
test_y_file_name = 'off-y-test.csv'
train_size = 2000
test_size = 900
window_size = 300

with open(input_file_name, newline='') as my_input, open(output_file_name, 'w', newline='') as my_output:
    reader = csv.reader(my_input)
    writer = csv.writer(my_output)
    window = []
    for index, row in enumerate(reader):
        if index > 0 and index % window_size == 0:
            writer.writerow(window)
            window = []
        window.append(row[1])

with open(output_file_name, newline='') as my_input, \
        open(train_x_file_name, 'w', newline='') as train_x_output, \
        open(test_x_file_name, 'w', newline='') as test_x_output:
    reader = csv.reader(my_input)
    train_x_writer = csv.writer(train_x_output)
    test_x_writer = csv.writer(test_x_output)
    for index, row in enumerate(reader):
        if index < train_size:
            train_x_writer.writerow(row)
        elif index < train_size + test_size:
            test_x_writer.writerow(row)
        else:
            break

with open(train_y_file_name, 'w', newline='') as y_output:
    writer = csv.writer(y_output)
    for i in range(0, train_size):
        writer.writerow('1')

with open(test_y_file_name, 'w', newline='') as y_output:
    writer = csv.writer(y_output)
    for i in range(0, test_size):
        writer.writerow('1')
