#include "GeometricTransformer.h"

int GeometricTransformer::RotateKeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	if (!srcImage.data) return 0;

	int width = srcImage.cols;
	int height = srcImage.rows;
	float x = cos(angle * PI / 180.0);//tính sin cos cho ma trận quay
	float x2 = sin(angle * PI / 180.0);
	int dstWidth = (int)(width * x + height * x2);//tính kích thước cho của sổ mới
	int dstHeight = (int)(width * x2 + height * x);

	dstImage.create(dstHeight, dstWidth, srcImage.type());//Khởi tạo ảnh đích có kích thước và type giống ảnh nguồn
	AffineTransform* aff = new AffineTransform(); //Khởi tạo đối tượng aff và gọi phương thức Rotate

	// quá trình xoay gồm
	// tịnh tiến về gốc
	// xoay
	// tịnh tiến lại tâm ban đầu

	// tạo ma trận biến đổi
	//Lấy tâm của ảnh thật khi quay góc
	aff->Translate((dstHeight / 2) - (height / 2), (dstWidth / 2) - (width / 2));
	aff->Translate(-(dstHeight / 2), -(dstWidth / 2));
	aff->Rotate(angle);
	aff->Translate(dstHeight / 2, dstWidth / 2);
	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	if (!dstImage.data) return 0;

	namedWindow("Rotate keep image", WINDOW_AUTOSIZE);
	cout << dstImage.size() << endl;
	imshow("Rotate keep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::RotateUnkeepImage(const Mat& srcImage, Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
	int width = srcImage.cols;
	int height = srcImage.rows;

	dstImage.create(height, width, srcImage.type());
	AffineTransform* aff = new AffineTransform();

	aff->Translate(-(height / 2), -(width / 2));
	aff->Rotate(angle);
	aff->Translate(height / 2, width / 2);

	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	if (!dstImage.data) return 0;

	namedWindow("Rotate unkeep image", WINDOW_AUTOSIZE);
	imshow("Rotate unkeep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::Scale(const Mat& srcImage, Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
	//if (srcImage.empty())
	//{
	//	// Trả với giá trị 0 nếu source image rỗng (không đọc được ảnh)
	//	return 0;
	//}
	//// Lấy kích thước ảnh gốc
	//int cols = srcImage.cols;
	//int rows = srcImage.rows;

	////Các thông số widthStep, nChannels để nội suy màu
	//int widthStep = srcImage.step[0];
	//int nChannels = srcImage.step[1];

	//// Khởi tạo ma trận affine scale
	//AffineTransform AT;
	//AT.Translate(-rows / 2, -cols / 2);
	//AT.Scale(sx, sy);
	//AT.Translate(rows / 2, cols / 2);

	//// Tạo ma trận nghịch đảo của affine scale
	//Mat Rinv = AT.getmatrixTransform().inv();

	////Lấy con trỏ của ảnh gốc và các thông số widthStep, nChannels để nội suy màu
	//uchar* pSrc = (uchar*)srcImage.data;

	//// Khởi tạo ảnh kết quả với kích thước tỉ lệ theo sx, sy cho trước
	//dstImage = Mat::zeros(rows, cols, CV_8UC3);


	//// Khởi tạo vector chứa [Px, Py, 1]
	//float B[] =
	//{
	//	0, 0, 1.0,
	//};
	//Mat Pos = Mat(3, 1, CV_32FC1, B);

	//// Dùng try catch để bắt lỗi trong quá trình transform
	//try
	//{
	//	for (int i = 0; i < rows; i++)
	//	{
	//		uchar* pData = dstImage.ptr<uchar>(i);
	//		for (int j = 0; j < cols; j++)
	//		{
	//			//Đặt Px = x, Py = y với x, y là tọa độ đối với tâm (0,0) là gốc trên cùng bên trái
	//			Pos.at<float>(0, 0) = i * 1.0;
	//			Pos.at<float>(1, 0) = j * 1.0;
	//			//Tìm tọa độ thực trên ảnh gốc
	//			Mat tpos = Rinv * Pos;
	//			//tx, ty là index thực của điểm ảnh trên ma trận ảnh gốc
	//			float tx = tpos.at<float>(0, 0);
	//			float ty = tpos.at<float>(1, 0);
	//			if (tx >= 0 and ty >= 0 and tx < rows and ty < cols)
	//			{
	//				for (int c = 0; c < nChannels; c++)
	//					pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);

	//			}
	//		}
	//	}
	//}
	//catch (Exception& e)
	//{
	//	// Trả về giá trị 1 nếu có bất kì lỗi nào xảy ra trong quá trình scale
	//	return 1;
	//}
	////Giải phóng các ma trận trung g
	//Rinv.release();
	//Pos.release();

	//// Trả về giá trị 0 sau khi scale thành công
	//return 0;
}

int GeometricTransformer::Resize(const Mat& srcImage, Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
	if (srcImage.empty())
		return 0;

	int rows = srcImage.rows;
	int cols = srcImage.cols;

	int widthStep = srcImage.step[0];
	int nChannels = srcImage.step[1];

	//Tạo khung cho ảnh đích với kích thước cho trước
	if (nChannels == 1) {
		dstImage = Mat(newHeight, newWidth, CV_8UC1, Scalar(0));
	}
	else if (nChannels == 3) {
		dstImage = Mat(newHeight, newWidth, CV_8UC3, Scalar(0));
	}
	else {
		return 1;
	}

	//Lấy con trỏ của ảnh gốc

	uchar* pSrc = (uchar*)srcImage.data;
	for (int i = 0; i < newHeight; i++) {
		uchar* pData = dstImage.ptr<uchar>(i);
		for (int j = 0; j < newWidth; j++) {

			float tx = (float)i * rows / newHeight;
			float ty = (float)j * cols / newWidth;

			//Chỉ xét tx, ty nằm trong ảnh gốc
			if (tx >= 0 and ty >= 0 and tx < rows and ty < cols)
			{
				for (int c = 0; c < nChannels; c++)
					pData[j * nChannels + c] = interpolator->Interpolate(tx, ty, pSrc + c, widthStep, nChannels);

			}
		}
	}
	return 0;
}

// Lộc
int GeometricTransformer::Flip(const Mat& srcImage, Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
	int r = srcImage.rows;
	int c = srcImage.cols;
	if (!srcImage.data)
		return 0;
	dstImage = Mat(r, c, CV_8UC3);
	int nChannel = srcImage.step[1];
	int widthStep = srcImage.step[0];
	uchar* srcPixel = (uchar*)srcImage.data;

	//Đối xứng theo trục hoành
	if (direction) {
		for (int i = 0; i < r; i++) {
			uchar* dstPixel = dstImage.ptr<uchar>(i);
			for (int j = 0; j < c; j++, dstPixel += nChannel) {
				//Dùng hàm nội suy để lấy giá trị pixel từ ảnh cũ ở tọa độ (x,height - y)
				//rồi gán cho pixel mới ở tọa độ (x,y)
				uchar temp = interpolator->Interpolate(j, r - i, srcPixel, widthStep, nChannel);
				dstPixel[0] = temp;
			}
		}
	}
	//Đối xứng theo trục tung
	else {
		for (int i = 0; i < r; i++) {
			uchar* dstPixel = dstImage.ptr<uchar>(i);
			for (int j = 0; j < c; j++, dstPixel += nChannel) {
				//Dùng hàm nội suy để lấy giá trị pixel từ ảnh cũ ở tọa độ (width - x,y)
				// rồi gán cho pixel mới ở tọa độ (x,y)
				uchar temp = interpolator->Interpolate(c - j, i, srcPixel, widthStep, nChannel);
				dstPixel[0] = temp;
			}
		}
	}
	return 1;
}

GeometricTransformer::GeometricTransformer()
{
}

GeometricTransformer::~GeometricTransformer()
{
}

NearestNeighborInterpolate::NearestNeighborInterpolate()
{
}

NearestNeighborInterpolate::~NearestNeighborInterpolate()
{
}

PixelInterpolate::PixelInterpolate()
{
}

PixelInterpolate::~PixelInterpolate()
{
}


BilinearInterpolate::BilinearInterpolate()
{
}

BilinearInterpolate::~BilinearInterpolate()
{
}

void AffineTransform::Translate(float dx, float dy)
{
	float value[3][3] = { {1, 0, dx}
					,{0, 1, dy}
					, {0, 0, 1} };
	Mat matTranslate = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matTranslate * this->_matrixTransform;
}

void AffineTransform::Rotate(float angle)
{
	float cosAngle = cos(angle * PI / 180);
	float sinAngle = sin(angle * PI / 180);
	float value[3][3] = { {cosAngle, -sinAngle, 0}
						,{sinAngle, cosAngle, 0}
						, {0, 0, 1} };
	Mat matRotate = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matRotate * this->_matrixTransform;
}

void AffineTransform::Scale(float sx, float sy)
{
	float value[3][3] = { {sx, 0, 0}
					,{0, sy, 0}
					, {0, 0, 1} };
	Mat matScale = Mat(3, 3, CV_32FC1, value);
	this->_matrixTransform = matScale * this->_matrixTransform;
}

void AffineTransform::TransformPoint(float& x, float& y)
{
	float v[] = { x, y, 1 };
	Mat matResult = this->_matrixTransform * (Mat(3, 1, CV_32FC1, v));
	x = matResult.ptr<float>(0)[0];
	y = matResult.ptr<float>(0)[1];
}

AffineTransform::AffineTransform()
{
	_matrixTransform = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);//khởi tạo ma trận
}

AffineTransform::~AffineTransform()
{
	_matrixTransform.release(); //giải phóng ma trận
}
