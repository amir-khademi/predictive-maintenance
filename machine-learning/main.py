import tensorflow as tf
import numpy as np
import dataset_loader

class DenseNN:
    def __init__(self,
                 window_size,
                 num_class,
                 units=[128, 64, 32]):
        self.saving_path = "./model/"
        self.window_size = window_size
        self.num_class = num_class
        self.epoch = 200
        self.lr = .0025
        self.batch_size = 50

        self.create_model(units)
        self.load()


    def next_batch(self, data, labels):
        idx = np.arange(0, len(data))
        np.random.shuffle(idx)
        idx = idx[:self.batch_size]
        data_shuffle = [data[i] for i in idx]
        labels_shuffle = [labels[i] for i in idx]

        return np.asarray(data_shuffle), np.asarray(labels_shuffle)

    def create_model(self, units):
        self.input_data = tf.placeholder(dtype=tf.float32, shape=[None, self.window_size])
        self.input_label = tf.placeholder(dtype=tf.float32, shape=[None, self.num_class])

        last_layer = None
        for i, u in enumerate(units):
            if i == 0:
                last_layer = tf.layers.dense(self.input_data, u, tf.nn.relu)
            else:
                last_layer = tf.layers.dense(last_layer, u, tf.nn.relu)
        self.pred_label = tf.layers.dense(last_layer, self.num_class, activation=tf.nn.softmax)

        self.loss_function = tf.losses.softmax_cross_entropy(self.input_label, self.pred_label)
        opt = tf.train.GradientDescentOptimizer(self.lr)
        self.training_step = opt.minimize(self.loss_function)

        correct_prediction = tf.equal(tf.argmax(self.input_label, 1), tf.argmax(self.pred_label, 1))
        self.accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

        self.saver = tf.train.Saver()
        self.sess = tf.Session()

    def train(self, train_data, train_labels, test_data, test_labels):
        data_len = len(train_data)
        for e in range(self.epoch):
            for _ in range(data_len//self.batch_size):
                x_batch, y_batch = self.next_batch(train_data, train_labels)
                _ = self.sess.run((self.training_step),
                                         feed_dict={self.input_data: x_batch, self.input_label: y_batch})
            if (e % 10 == 0):
                acc, loss = self.sess.run((self.accuracy, self.loss_function),
                                    feed_dict={self.input_data: test_data, self.input_label: test_labels})
                print(
                    "****** epoch {} ******\n"
                    "loss_value: {}\n"
                    "accuracy: {}\n"
                    .format(e, loss, acc))
                self.save()
        print("model saved!")

    def load(self):
        import os
        self.sess.run(tf.global_variables_initializer())
        self.sess.run(tf.local_variables_initializer())
        if not os.path.exists(self.saving_path):
            os.makedirs(self.saving_path)
        if not tf.train.checkpoint_exists(self.saving_path + 'checkpoint'):
            print('Saved temp_models not found! Randomly initialized.')
        else:
            self.saver.restore(self.sess, self.saving_path)
            print('Model loaded!')

    def save(self):
        self.saver.save(self.sess, self.saving_path)

    def predict(self, data):
        return np.argmax(self.sess.run(self.pred_label, feed_dict={self.input_data: data}), 1)

train_data, train_labels, test_data, test_labels, predict_data = dataset_loader.load_dataset()

model = DenseNN(300, 3)
model.train(train_data, train_labels, test_data, test_labels)
print(model.predict(predict_data))
