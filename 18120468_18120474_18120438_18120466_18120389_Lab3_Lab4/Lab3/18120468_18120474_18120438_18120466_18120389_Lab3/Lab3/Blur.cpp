#include "Blur.h"
#include "Convolution.h"

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method)
{

	// số hàng, số cột
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;

	// số kênh màu
	int nChannels = sourceImage.step[1];

	//widthStep là khoảng cách tính theo byte giữa 2 pixel cùng cột trên 2 dòng kế tiếp
	int widthStep = sourceImage.step[0];

	//kiểm tra
	if (nChannels != 1) return 1;

	//swith cho 3 trường hợp sử dụng: lọc trung bình, lọc trung vị và gaussian
	switch (method)
	{
	// lọc trung bình
	case 0: {
		//khai báo
		Convolution Conv;
		vector<float> kernel;
		
		float n = kWidth * kHeight;
		for (int i = 0; i < kWidth * kHeight; i++) {
			kernel.push_back(1.0 / n);
		}
		Conv.SetKernel(kernel, kWidth, kHeight);
		
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));
		// tính chập
		Conv.DoConvolution(sourceImage, destinationImage);

		//set ảnh des
		for (int i = 0; i < destinationImage.rows; i++) {
			float* dataRow = destinationImage.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				dataRow[j] /= 255.0;
			}
		}
	}
		  break;

	//lọc trung vị
	case 1: {
		destinationImage = Mat(rows - kHeight + 1, cols - kWidth + 1, CV_32FC1, Scalar(0));

		//Tạo bảng offsets
		int kHalfWidth = kWidth >> 1;
		int kHalfHeight = kHeight >> 1;
	
		vector<int> offsets;
		for (int y = -kHalfHeight; y <= kHalfHeight; y++)
			for (int x = -kHalfWidth; x <= kHalfWidth; x++)
				offsets.push_back(y * widthStep + x);

		// con trỏ data ảnh nguồn
		uchar* p = (uchar*)sourceImage.data;

		for (int i = 0; i < destinationImage.rows; i++) {
			float* dataRow = destinationImage.ptr<float>(i);
			for (int j = 0; j < destinationImage.cols; j++) {
				int i_source = i + (kWidth / 2), j_source = j + (kHeight / 2);
				uchar* pSource = p + i_source * widthStep + j_source * nChannels;
				//lấy lân cận
				vector<uchar> value;
				for (int k = 0; k < offsets.size(); k++) {
					value.push_back(pSource[offsets[k]]);
				}
				//sắp xếp và xét median
				sort(value.begin(), value.begin() + value.size());
				dataRow[j] = value[value.size() / 2 + 1] / 255.0;
			}
		}
	}
		  break;
	//gaussian
	case 2: {
		// tạo bản sao
		Mat cloneImage = sourceImage.clone(); 
		destinationImage = Mat(rows, cols, CV_32FC1, Scalar(0.0));
		vector <int> dx;
		vector <int> dy;


		for (int i = 0; i < kHeight; i++)
			for (int j = 0; j < kWidth; j++) {
				dx.push_back(i - (kHeight / 2));
				dy.push_back(j - (kWidth / 2));
			}

		// tính độ lệch chuẩn
		float stddev;
		if ((kHeight == 3) && (kWidth == 3)) stddev = 3;
		else
			stddev = (1.0 * kHeight / 3) * (1.0 * kWidth / 3) * 3;
		vector <float> h(kHeight * kWidth);
		for (int k = 0; k < kHeight * kWidth; k++) { 
			int i = dx[k], j = dy[k];
			float disValue = 1 / (sqrt(2 * 3.14) * stddev) * exp(-(i * i + j * j) / (2 * stddev * stddev));
			h[k] = disValue;
		}
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++) {
				float sumDisValue = 0;
				for (int k = 0; k < kHeight * kWidth; k++) {
					int i_pos = i - dx[k];
					int j_pos = j - dy[k];
					float dataImage;
					if ((i_pos - (kHeight / 2) < 0) || (i_pos + (kHeight / 2) >= rows)
						|| (j_pos - (kWidth / 2) < 0) || (j_pos + (kWidth / 2) >= cols)) {
						dataImage = 1.0 * cloneImage.ptr<uchar>(i)[j];
					}
					else
						dataImage = 1.0 * cloneImage.ptr<uchar>(i_pos)[j_pos];
					sumDisValue += dataImage * h[k];
				}
				if (sumDisValue > 255.0)
					destinationImage.ptr<float>(i)[j] = 255;
				else
					destinationImage.ptr<float>(i)[j] = sumDisValue;
			}

		//set ảnh đích
		float minValue = 255;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				if (destinationImage.ptr<float>(i)[j] < minValue) minValue = destinationImage.ptr<float>(i)[j];

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				destinationImage.ptr<float>(i)[j] = (destinationImage.ptr<float>(i)[j] - minValue) / (255 - minValue);
	}
		  break;

	}

	return 0;
}

Blur::Blur()
{
}

Blur::~Blur()
{
}
