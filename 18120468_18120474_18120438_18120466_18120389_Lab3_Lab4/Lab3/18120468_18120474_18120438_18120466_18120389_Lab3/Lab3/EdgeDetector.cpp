#include "EdgeDetector.h"
#include "Convolution.h"

int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{
	// số hàng số cột
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;

	// số kênh màu
	int nChannels = sourceImage.channels();

	//kiểm tra số kênh màu
	if (nChannels != 1) return 1;

	// con số để xác định có quá nhỏ để bỏ qua k
	float eps = 1e-6;


	// switch cho 3 trường hợp sobel, prewitt, laplace
	switch (method) {
	//sobel
	case 1: {
		//khai báo
		float threshold = 25;
		// tạo bản sao
		Mat cloneImage = sourceImage.clone(); 
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));

		vector <float> Wx = { -1 , 0, 1, -2, 0, 2, -1, 0, 1 };
		vector <float> Wy = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
		for (int i = 0; i < Wx.size(); i++) {
			Wx[i] *= 1.0 / 4;
			Wy[i] *= 1.0 / 4;
		}

		//set và tính chập 
		Mat Gx;
		Mat Gy;
		Convolution ConvoX, ConvoY;
		ConvoX.SetKernel(Wx, 3, 3);
		ConvoY.SetKernel(Wy, 3, 3);
		ConvoX.DoConvolution(cloneImage, Gx);
		ConvoY.DoConvolution(cloneImage, Gy);

		// set cho ảnh des
		for (int i = 0; i < destinationImage.rows; i++)
			for (int j = 0; j < destinationImage.cols; j++) {
				float fx = Gx.ptr<float>(i)[j];
				float fy = Gy.ptr<float>(i)[j];
				float e = sqrt(fx * fx + fy * fy);
				if (e - threshold >= eps) {
					destinationImage.ptr<float>(i)[j] = 1;
				}
			}
	}
		  break;
	//prewitt
	case 2: {

		//khai báo 
		float threshold = 50;
		Convolution ConvOx, ConvOy;
		Mat fx;
		Mat fy;

		vector <float> Wx = { 1.0, 0, -1.0, 1.0, 0, -1.0, 1.0, 0, -1.0 };
		vector <float> Wy = { -1.0, -1.0, -1.0, 0, 0, 0, 1.0, 1.0, 1.0 };
		for (int i = 0; i < Wx.size(); i++) {
			Wx[i] *= 1.0 / 3;
			Wy[i] *= 1.0 / 3;
		}

		// tính chập 
		ConvOx.SetKernel(Wx, kWidth, kHeight);
		ConvOx.DoConvolution(sourceImage, fx);

		ConvOy.SetKernel(Wy, kWidth, kHeight);
		ConvOy.DoConvolution(sourceImage, fy);

		//set cho ảnh des
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));
		for (int i = 0; i < destinationImage.rows; i++) {
			float* DestRow = destinationImage.ptr<float>(i);
			float* fxRow = fx.ptr<float>(i);
			float* fyRow = fy.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				float e = sqrt(fxRow[j] * fxRow[j] + fyRow[j] * fyRow[j]);
				if (e - threshold >= eps) {
					DestRow[j] = 1;
				}
			}
		}
		fx.release();
		fy.release();
	}
		  break;
	//laplace
	case 3: {
		try {
			vector<float> laplace = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
			Mat destinationImageCopied = Mat(rows, cols, CV_32FC1);

			// tính chập
			Convolution Laplace;
			Laplace.SetKernel(laplace, 3, 3);
			// kiểm tra điều kiện
			if (Laplace.DoConvolution(sourceImage, destinationImageCopied) == 1) return 1;

			// tính threshold
			float threshold = -1.0 * INT_MAX;
			destinationImage = Mat::zeros(rows, cols, CV_8UC1);
			for (int x = 0; x < destinationImageCopied.rows; x++) {
				for (int y = 0; y < destinationImageCopied.cols; y++) {
					float value = destinationImageCopied.at<float>(x, y);
					threshold = value > threshold ? value : threshold;
				}
			}
			threshold = threshold > 255 ? 255 : threshold;
			threshold = threshold * 25 / 100.0;

			//tính zero crossing
			int dx[] = { -1, 1, 0, -1 };
			int dy[] = { -1, -1, 1, 0 };
			for (int x = 1; x < destinationImageCopied.rows - 1; x++) {
				for (int y = 1; y < destinationImageCopied.cols - 1; y++) {
					int count = 0;
					for (int k = 0; k < 4; k++) {
						float value1 = destinationImageCopied.at<float>(x + dx[k], y + dy[k]);
						float value2 = destinationImageCopied.at<float>(x - dx[k], y - dy[k]);
						int sign1 = value1 < 0 ? -1 : 1;
						int sign2 = value2 < 0 ? -1 : 1;
						if (sign1 != sign2 && abs(value1 - value2) - eps > threshold) {
							count++;
						}
					}
					// điểm zero crossing
					if (count >= 2) destinationImage.at<uchar>(x, y) = 255;
				}
			}


		}
		catch (exception& e) {
			return 1;
		}
		return 0;
	}
		  break;
	}


	return 0;
}


EdgeDetector::EdgeDetector()
{
}

EdgeDetector::~EdgeDetector()
{
}
