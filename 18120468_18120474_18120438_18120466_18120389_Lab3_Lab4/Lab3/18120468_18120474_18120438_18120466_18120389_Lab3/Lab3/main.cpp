#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Blur.h"
#include "Convolution.h"
#include "EdgeDetector.h"

using namespace cv;

//char command[255];
//char inputPath[255];

int main(int argc, char* argv[])
{
#pragma region ONLY_FOR_DEBUG
	argc = 5;
	char* temp[5];
	temp[0] = _strdup("main.exe");
	temp[1] = _strdup("--laplace");
	temp[2] = _strdup("girl_gray.png");
	temp[3] = _strdup("3");
	temp[4] = _strdup("3");
	argv = temp;
#pragma endregion

	if (argc < 3) {
		cout << "Error arguments";
		return 0;
	}

	char* cmd = argv[1];
	char* inputImg = argv[2];
	Mat srcImg = imread(inputImg, IMREAD_COLOR);
	//k tìm thấy ảnh
	if (srcImg.empty()) {
		cout << "image not found";
		return 1;
	}

	Mat desImg;
	Blur BlurMethod;
	Mat graySourceImage;
	cvtColor(srcImg, graySourceImage, CV_BGR2GRAY);
	if (strcmp(cmd, "--mean") == 0) {
		//lỗi thiếu tham số
		if (argc < 5) {
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, desImg, kWidth, kHeight, 0) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Error";
		waitKey(0);
		return 0;
	}
	if (strcmp(cmd, "--median") == 0) {
		//lỗi thiếu tham số
		if (argc < 5) {
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, desImg, kWidth, kHeight, 1) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Error";
		waitKey(0);
		return 0;
	}
	if (strcmp(cmd, "--gauss") == 0) {
		//lỗi thiếu tham số
		if (argc < 5) {
			return 1;
		}
		int kWidth = atoi(argv[3]);
		int kHeight = atoi(argv[4]);
		if (BlurMethod.BlurImage(graySourceImage, desImg, kWidth, kHeight, 2) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Errors";
		waitKey(0);
		return 0;
	}
	EdgeDetector edgeDetector;
	if (strcmp(cmd, "--sobel") == 0) {
		if (edgeDetector.DetectEdge(graySourceImage, desImg, 3, 3, 1) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Error";
		waitKey(0);
		return 0;
	}
	if (strcmp(cmd, "--prewitt") == 0) {
		if (edgeDetector.DetectEdge(graySourceImage, desImg, 3, 3, 2) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Error";
		waitKey(0);
		return 0;
	}
	if (strcmp(cmd, "--laplace") == 0) {
		if (edgeDetector.DetectEdge(graySourceImage, desImg, 3, 3, 3) == 0) {
			imshow("Source", srcImg);
			imshow("Destination", desImg);
		}
		else cout << "Error";
		waitKey(0);
		return 0;
	}

#pragma region ONLY_FOR_DEBUG
	free(temp[0]);
	free(temp[1]);
	free(temp[2]);
	free(temp[3]);
	free(temp[4]);
#pragma endregion

	waitKey(0);
	return 0;
}



