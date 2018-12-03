import tensorflow as tf
import numpy as np
# import dataset_loader
import time


class DenseNN:
    def __init__(self,
                 window_size,
                 num_class,
                 units=[128, 64, 32]):
        self.saving_path = "./predict/nn_model/"
        self.window_size = window_size
        self.num_class = num_class
        self.epoch = 200
        self.lr = .0025
        self.batch_size = 50

        self.create_model(units)
        self.load()

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

