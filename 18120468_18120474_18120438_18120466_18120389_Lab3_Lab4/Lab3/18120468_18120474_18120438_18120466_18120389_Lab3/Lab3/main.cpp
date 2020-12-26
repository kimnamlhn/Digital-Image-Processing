#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Blur.h"
#include "Convolution.h"
#include "EdgeDetector.h"

using namespace cv;

int main()
{
	Mat src = imread("girl.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat dst = src.clone();

	Prewitt(src, dst);

	imshow("Source Image", src);
	imshow("Prewitt Edge Detection", dst);
	waitKey();

	return 0;
}