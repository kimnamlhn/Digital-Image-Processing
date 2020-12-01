#include "Converter.h"
#include "ColorTransformer.h"
#include <iostream>
using namespace std;

int main()
{
	// khởi tạo phần tử để biểu diễn hình ảnh
	ColorTransformer* CTs;
	CTs = new ColorTransformer(); // cấp phát bộ nhớ cho con trỏ CTs
	const Mat srcImage = imread("girl.jpg", CV_LOAD_IMAGE_COLOR); // Load ảnh màu của 
	Mat dstImage = imread("girl.jpg", CV_LOAD_IMAGE_COLOR); // Load ảnh màu của ảnh 
	int bright = 50; // độ sáng tăng thêm của ảnh
	int result = CTs->ChangeBrighness(srcImage, dstImage, bright); // tăng độ sáng của 
		// kiểm tra xem hình ảnh input có tồn tại hay không?
	if (result) {
		// nếu có dữ liệu ảnh gốc thì hiển thị ảnh gốc và ảnh được thay đổi độ sáng
		imshow("Source Image", srcImage);
		imshow("Destination Image", dstImage);
	}
	else {
		// nếu không thì thông báo cho người dùng biết
		cout << "Khong tim thay hinh anh can xu ly" << endl;
	}
	waitKey(0); // hàm dừng màn hình để xem hảnh hiển thị
	// Xóa bộ nhớ đã cấp phát cho con trỏ CTs
	delete CTs;
	return 0;
}

