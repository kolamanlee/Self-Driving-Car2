#!/usr/bin/env python
from styx_msgs.msg import TrafficLight
import os
import sys

import tensorflow as tf
from io import StringIO
import time
import numpy as np 
import cv2

class TLClassifier(object):
    def __init__(self, simulator):
        #TODO load classifier

        current_path = os.path.dirname(os.path.realpath(__file__))
        self.simulator_used = simulator

        if (self.simulator_used == 1):
            frozen_model = current_path + '/models/tl_output_graph.pb'
        else:
            frozen_model = current_path + '/models/tl_output_graph.pb'
        
        label_map_file = current_path + '/models/tl_output_labels.txt'

        lines = tf.gfile.GFile(label_map_file)
        self.uid_to_human={}

        for uid, line in enumerate(lines):
            line = line.strip('\n')
            self.uid_to_human[uid]=line
        # 
        #
        self.detection_graph = tf.Graph()

        config = tf.ConfigProto()
        config.gpu_options.allow_growth = True

        with self.detection_graph.as_default():
            od_graph_def = tf.GraphDef()
            with tf.gfile.GFile(frozen_model,'rb') as f:
                od_graph_def.ParseFromString(f.read())
                tf.import_graph_def(od_graph_def,name='')
            #
            self.softmax_tensor = self.detection_graph.get_tensor_by_name('final_result:0')

        
        self.sess = tf.Session(graph=self.detection_graph, config=config)



    def get_classification(self, image):
        """Determines the color of the traffic light in the image

        Args:
            image (cv::Mat): image containing the traffic light

        Returns:
            int: ID of traffic light color (specified in styx_msgs/TrafficLight)

        """
        #TODO implement light color prediction
        
        img_as_string = cv2.imencode('.jpg', image)[1].tostring()

        with self.detection_graph.as_default():
            predictions = self.sess.run(self.softmax_tensor,{'DecodeJpeg/contents:0':img_as_string})

        predictions = np.squeeze(predictions)

        #sort
        top_id = predictions.argsort()[-1]
        if top_id  in self.uid_to_human:
            human_string = self.uid_to_human[top_id]
            if human_string == "GREEN":
                return TrafficLight.GREEN
            elif human_string == "RED":
                return TrafficLight.RED
            elif human_string == "YELLOW":
                return TrafficLight.YELLOW
        else:
            return TrafficLight.UNKNOWN
