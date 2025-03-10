﻿#include "Convolution.h"

// trả về kernel
vector<float> Convolution::GetKernel()
{
	return this->_kernel;
}

// gán kernel
void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight)
{
	this->_kernel = kernel;
	this->_kernelHeight = kHeight;
	this->_kernelWidth = kWidth;
}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage)
{
	//Kiểm tra ma trận nguồn
	if (sourceImage.empty())
		return 0;

	// số hàng, số cột
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;

	// tạo bản sao
	Mat cloneImage = sourceImage.clone(); 

	// tính tích chập
	vector <int> dx;
	vector <int> dy;
	for (int i = 0; i < this->_kernelHeight; i++)
		for (int j = 0; j < this->_kernelWidth; j++) {
			dx.push_back(i - (this->_kernelHeight / 2));
			dy.push_back(j - (this->_kernelWidth / 2));
		}

	//set cho ảnh des
	destinationImage = Mat(rows - this->_kernelHeight + 1, cols - this->_kernelWidth + 1, CV_32FC1, Scalar(0.0));
	
	for (int i = 0; i < destinationImage.rows; i++) {
		float* dataRow = destinationImage.ptr<float>(i);
		for (int j = 0; j < destinationImage.cols; j++) {
			int i_source = i + (this->_kernelHeight / 2), j_source = j + (this->_kernelWidth / 2);
			float convolution = 0.0;
			for (int k = 0; k < this->_kernelHeight * this->_kernelWidth; k++) {
				float dataImage = 1.0 * cloneImage.ptr<uchar>(i_source - dx[k])[j_source - dy[k]];
				float dataKernel = 1.0 * this->_kernel[(dx[k] + (this->_kernelHeight / 2)) * this->_kernelHeight + dy[k] + (this->_kernelWidth / 2)];
				convolution += dataImage * dataKernel;
			}
			dataRow[j] = convolution;
		}
	}
	
	return 0;

}

// hàm khởi tạo
Convolution::Convolution()
{
	this->_kernel = vector<float>(0, 0);
	this->_kernelHeight = 0;
	this->_kernelWidth = 0;
}


//hàm hủy
Convolution::~Convolution()
{
}
