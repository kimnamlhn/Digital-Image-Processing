#include "Converter.h"



int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.data == NULL || sourceImage.rows <= 0 || sourceImage.cols <= 0)
		return 0;

	int width = sourceImage.cols, height = sourceImage.rows;

	destinationImage = cv::Mat(height, width, CV_8UC1, Scalar(0));

	int srcnChannels = sourceImage.channels();
	int dstnChannels = destinationImage.channels();

	for (int y = 0; y < height; y++)
	{
		const uchar* pSrcRow = sourceImage.ptr<uchar>(y);
		uchar* pDstRow = destinationImage.ptr<uchar>(y);

		for (int x = 0; x < width; x++, pSrcRow += srcnChannels, pDstRow += dstnChannels)
		{
			uchar B = pSrcRow[0];
			uchar G = pSrcRow[1];
			uchar R = pSrcRow[2];
			float gray = (float)(R + G + B) / 3;
			pDstRow[0] = (uchar)gray;
		}
	}

	return 1;
}


int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage)
{
	if (!sourceImage.data)
	{
		return 1;
	}
	Mat input = sourceImage.clone();
	int row = sourceImage.rows;
	int col = sourceImage.cols;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			int B = sourceImage.at<Vec3b>(i, j)[0];
			int G = sourceImage.at<Vec3b>(i, j)[1];
			int R = sourceImage.at<Vec3b>(i, j)[2];
			int gray = (R + G + B) / 3.0;
			input.at<Vec3b>(i, j)[0] = gray;
			input.at<Vec3b>(i, j)[1] = gray;
			input.at<Vec3b>(i, j)[2] = gray;
		}
	}
	return 0;
}



int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage)
{
	destinationImage.create(sourceImage.size(), CV_8UC3);

	if (sourceImage.data) {
		int rows = sourceImage.rows;
		int cols = sourceImage.cols;
		int i, j, V, mini;
		float S, H;
		for (i = 0; i < rows; i++)
		{
			for (j = 0; j < cols; j++)
			{
				Vec3b intensity = sourceImage.at<Vec3b>(i, j);
				int blue = intensity.val[0];
				int green = intensity.val[1];
				int red = intensity.val[2];
				//V <- max(R,G,B)
				V = max(blue, max(green, red));
				mini = min(blue, min(green, red));
				// S <--
				if (V == 0)
				{
					S == 0;
					H == 0;
				}
				else
				{
					int delta = V - mini;
					S = 1.0 * (V - mini) / V;
					//H <--
					if (V == red)
						H = 1.0 * (60 * (green - blue)) / delta;
					else if (V == green)
						H = 120 + 1.0 * 60 * (blue - red) / delta;
					else if (V == blue)
						H = 240 + 1.0 * 60 * (red - green) / delta;

					if (H < 0)
						H += 360;

				}
				destinationImage.at<Vec3b>(i, j) = Vec3b(H / 2, S * 255, V);
			}
		}
		return 1;
	}
	else
		return 0;
}


/*
Hàm chuyển đổi không gian màu của ảnh từ HSV sang RGB
sourceImage: ảnh input
destinationImage: ảnh output
Hàm trả về
0: nếu chuyển thành công
1: nếu chuyển thất bại (không đọc được ảnh input,...)
*/
int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage)
{
	// Nếu không đọc được ảnh
	if (sourceImage.data == NULL || sourceImage.rows <= 0 || sourceImage.cols <= 0)
		return 0;

	// Xử lí
	// Tạo MAT mới chứa ảnh kết quả
	int width = sourceImage.cols, height = sourceImage.rows;
	destinationImage = Mat(height, width, sourceImage.type(), Scalar(0));

	// Bước nhảy qua phần từ bên dưới
	int srcWidthStep = sourceImage.step[0];
	int dstWidthStep = destinationImage.step[0];

	// Bước nhảy qua phần tử kế tiếp
	int srcnChannels = sourceImage.step[1];
	int dstnChannels = destinationImage.step[1];

	// Con trỏ Data
	const uchar* pSrcData = (uchar*)sourceImage.data;
	uchar* pDstData = (uchar*)destinationImage.data;

	for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep)
	{
		// Con trỏ Data của 1 dòng
		const uchar* pSrcRow = pSrcData;
		uchar* pDstRow = pDstData;

		for (int x = 0; x < width; x++, pSrcRow += srcnChannels, pDstRow += dstnChannels)
		{
			/*float h = (float)pSrcRow[0];
			float s = (float)pSrcRow[1] / 100;
			float v = (float)pSrcRow[2] / 100;
			float C = s * v;
			float X = C * (1 - abs(fmod(h/60.0,2) - 1));
			float m = v - C;
			float r, g, b;
			if (h >= 0 && h <= 60)
				r = C, g = X, b = 0;
			else if (h >= 60 && h < 120) {
				r = X, g = C, b = 0;
			}
			else if (h >= 120 && h < 180) {
				r = 0, g = C, b = X;
			}
			else if (h >= 180 && h < 240) {
				r = 0, g = X, b = C;
			}
			else if (h >= 240 && h < 300) {
				r = X, g = 0, b = C;
			}
			else {
				r = C, g = 0, b = X;
			}

			pDstRow[0] = (uchar)(b+m)*255;
			pDstRow[1] = (uchar)(g+m)*255;
			pDstRow[2] = (uchar)(r+m)*255;*/
			uchar r, g, b;

			// Lấy Hue, Saturation, Value
			uchar h = pSrcRow[0];
			uchar s = pSrcRow[1];
			uchar v = pSrcRow[2];

			// Thuật toán
			uchar i = h * 6;
			uchar f = h * 6 - i;
			uchar p = v * (1 - s);
			uchar q = v * (1 - f * s);
			uchar t = v * (1 - (1 - f) * s);

			switch (i % 6)
			{
			case 0:
				r = q, g = t, b = p; break;
			case 1:
				r = q, g = v, b = p; break;
			case 2:
				r = p, g = v, b = t; break;
			case 3:
				r = p, g = q, b = v; break;
			case 4:
				r = t, g = p, b = v; break;
			case 5:
				r = v, g = p, b = q; break;
			}

			// Trả về giá trị R, G, B
			pDstRow[2] = r * 255;
			pDstRow[1] = g * 255;
			pDstRow[0] = b * 255;
		}
	}
	return 1;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type)
{
	if (!sourceImage.data)
	{
		return 1;
	}
	if (type == 0)
		RGB2GrayScale(sourceImage, destinationImage);
	else if (type == 1)
		GrayScale2RGB(sourceImage, destinationImage);
	else if (type == 2)
		RGB2HSV(sourceImage, destinationImage);
	else if (type == 3)
		HSV2RGB(sourceImage, destinationImage);
	else return 1;
	return 0;
}

int Converter::Convert(IplImage* sourceImage, IplImage* destinationImage, int type)
{
	return 0;
}

Converter::Converter()
{
}

Converter::~Converter()
{
}
