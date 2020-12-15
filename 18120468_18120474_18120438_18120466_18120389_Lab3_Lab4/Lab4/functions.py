#class va ham tinh duong bien canh va can chinh hinh anh
import cv2;

class CannyEdgeDetector:
    def __init__(self):
        _lowThreshold =0;
        _highThreshold=0;

    def HysteresisThresholding (const Mat& G, Mat& dstImage, int x, int y):
    
