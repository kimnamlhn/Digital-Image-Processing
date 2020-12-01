#include "ColorTransformer.h"


//Trung Nam
int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	if (sourceImage.data) {
		for (int i = 0; i < sourceImage.rows; i++) {
			for (int j = 0; j < sourceImage.cols; j++) {

				for (int k = 0; k < sourceImage.channels(); k++) {
					// thay doi tren tung kenh mau  R, G, B

					if (sourceImage.at<Vec3b>(i, j)[k] + b > 255) {
						// Neu gia tri do sang vuot qua 255 thi gan bang 255
						destinationImage.at<Vec3b>(i, j)[k] = 255;
					}
					else if (sourceImage.at<Vec3b>(i, j)[k] + b < 0) {
						// Neu gia tri do sang nho hon 0 thi gan bang 0
						destinationImage.at<Vec3b>(i, j)[k] = 0;
					}
					else {
						saturate_cast<uchar>(destinationImage.at<Vec3b>(i, j)[k] += b);
					}
				}
			}
		}
	}
	else {
		return 0;
	}

	if (!destinationImage.data) {
		return 0;
	}
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
{
	if (sourceImage.empty())
		return 0;

	uchar lookup[256];
	for (int i = 0; i < 256; i++)
		lookup[i] = saturate_cast<uchar>(i * c);

	//Khởi tạo ảnh đích có kích thước và type giống ảnh nguồn
	destinationImage.create(sourceImage.rows, sourceImage.cols, sourceImage.type());

	int width = sourceImage.cols, height = sourceImage.rows;
	//nChannels là số kênh màu
	int nChannels = sourceImage.channels();
	//widthStep là khoảng cách tính theo byte giữa 2 pixel cùng cột trên 2 dòng kế tiếp
	int widthStep = sourceImage.step[0];
	//pData là con trỏ quản lý vùng nhớ ảnh
	uchar* pData = (uchar*)destinationImage.data; //COn tro data cua anh dich
	uchar* psData = (uchar*)sourceImage.data; //COn tro data cua anh nguon
	for (int i = 0; i < height; i++, psData += widthStep, pData += widthStep) {
		uchar* pRow = pData; //Con tro dong cua anh dich
		uchar* psRow = psData;//Con tro dong cua anh nguon
		for (int j = 0; j < width; j++, pRow += nChannels, psRow += nChannels) {
			for (int k = 0; k < nChannels; k++)
				pRow[k] = lookup[(int)psRow[k]];
		}
	}
	if (destinationImage.empty())
		return 0; //Loi 
	return 1;
}


/*
Hàm tính lược đồ màu tổng quát cho ảnh bất kỳ
Tham so :
	sourceImage : ảnh ban đầu
	histMatrix : ma trận histogram (nChannels x 256), mỗi dòng là 1 mảng 256 phần tử histogram của từng kênh màu
Hàm trả về:
	1: Nếu thành công thì trả về matrix kết quả (ảnh gốc vẫn giữ nguyên giá trị)
	0: Nếu không tính được histogram hoặc ảnh input không tồn tại
*/
//Phú Hồng
int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	// Nếu không đọc được ảnh
	if (sourceImage.data == NULL || sourceImage.rows <= 0 || sourceImage.cols <= 0)
		return 0;

	// Lấy kích thước ma trận, số kênh
	int width = sourceImage.cols, height = sourceImage.rows;
	int srcnChannels = sourceImage.channels();

	// ảnh xám
	if (srcnChannels == 1)
	{
		int histogram[256];
		for (int i = 0; i < 256; i++)	// khởi tạo histogram
		{
			histogram[i] = 0;
		}

		//truy xuất từng phần tử trong ma trận
		for (int y = 0; y < height; y++)
		{
			const uchar* pSrcRow = sourceImage.ptr<uchar>(y);

			for (int x = 0; x < width; x++, pSrcRow += srcnChannels)
			{
				histogram[pSrcRow[0]]++;	// +1 nếu giá trị xuất hiện và lặp lại
			}
		}

		// tần suất xuất hiện của các giá trị màu khá lớn nên dùng float mới lưu được
		histMatrix = Mat(1, 256, CV_32FC1);
		float* pHistMatrix = histMatrix.ptr<float>(0);		// con trỏ đầu dòng

		//gán từng tần suất xuất hiện màu vào từng phần tử trong ma trận
		for (int i = 0; i < histMatrix.cols; i++, pHistMatrix += histMatrix.channels())
		{
			pHistMatrix[0] = histogram[i];
		}
		//pHistMatrix -= 256; // muốn in xem thử thì nhớ trừ 256 để con trỏ về vị trí đầu ma trận

		/*for (int i = 0; i < histMatrix.cols; i++, pHistMatrix += histMatrix.channels())
		{
			cout << pHistMatrix[0] << " ";
		}
		cout << endl << endl << endl;
		for (int i = 0; i < 256; i++)
			cout << histogram[i] << " ";*/
	}


	// ảnh màu RGB
	else if (srcnChannels == 3)
	{
		int histogram[3][256];
		for (int i = 0; i < 3; i++)			// khởi tạo ma trận histogram
		{
			for (int j = 0; j < 256; j++)
			{
				histogram[i][j] = 0;
			}
		}

		// truy xuất các phần từ trong ma trận
		for (int y = 0; y < height; y++)
		{
			const uchar* pSrcRow = sourceImage.ptr<uchar>(y);

			for (int x = 0; x < width; x++, pSrcRow += srcnChannels)	// +1 nếu giá trị xuất hiện và lặp lại
			{
				histogram[0][pSrcRow[0]]++;		// kênh B
				histogram[1][pSrcRow[1]]++;		// kênh G
				histogram[2][pSrcRow[2]]++;		// kênh R
			}
		}

		// tần suất xuất hiện của các giá trị màu khá lớn nên dùng float mới lưu được
		histMatrix = Mat(3, 256, CV_32FC1);
		float* pHistMatrix = histMatrix.ptr<float>(0);	// con trỏ đầu dòng

		//gán từng tần suất xuất hiện màu vào từng phần tử trong ma trận
		for (int y = 0; y < histMatrix.rows; y++)
		{
			for (int x = 0; x < histMatrix.cols; x++, pHistMatrix += histMatrix.channels())
			{
				pHistMatrix[0] = histogram[0][x];
				pHistMatrix[1] = histogram[1][x];
				pHistMatrix[2] = histogram[2][x];
			}
			//pHistMatrix -= 256; // muốn in xem thử thì nhớ trừ 256 để con trỏ về vị trí đầu ma trận
		}
		/*for (int y = 0; y < histMatrix.rows; y++)
		{
			for (int i = 0; i < histMatrix.cols; i++, pHistMatrix += histMatrix.channels())
			{
				cout << pHistMatrix[0] << " ";
			}
			cout << endl << endl << endl;
		}*/
		/*for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				cout << histogram[i][j] << " ";
			}
		}
		*/
	}

	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	int** hist;
	int row = sourceImage.rows;
	int col = sourceImage.cols;
	int nChannel = sourceImage.channels();
	int temp;

	sourceImage.copyTo(destinationImage);
	destinationImage.setTo(0);

	hist = (int**)calloc(nChannel, sizeof(int*));
	for (int i = 0; i < nChannel; i++)
	{
		hist[i] = (int*)calloc(256, sizeof(int));

		memset(hist[i], 0, 256 * sizeof(int));
	} // Tạo mảng chứa ma trận Histogram với chiều rộng là 256


	for (int i = 0; i < row; i++)
	{
		const uchar* pRow = sourceImage.ptr<uchar>(i);
		for (int j = 0; j < col; j++, pRow += nChannel)
		{
			for (int o = 0; o < nChannel; o++)
			{
				for (int u = 1; u <= 255; u++)
				{
					if (pRow[o] < u) // Kiểm tra xem giá trị màu tại vị trí (i,j) của kênh màu thứ o có nằm trong khoảng bin của histogram không 
					{
						hist[o][u - 1]++; // Tăng số lần xuất hiện nếu giá trị màu nằm trong khoảng bin nhất định của histogram
						break;
					}
				}
			}

		}
	}
	// Tính histogram của ảnh

	double** p = NULL;
	p = (double**)calloc(nChannel, sizeof(double*));
	for (int i = 0; i < nChannel; i++)
	{
		p[i] = (double*)calloc(256, sizeof(double));
	} // Khai báo ma mảng chứa xác suất của từng bin trong histogram


	for (int i = 0; i < nChannel; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			p[i][j] = double(hist[i][j]) / double((col * row));
		}
	} // Tính xác suất của các bin trong histogram

	for (int i = 0; i < row; i++)
	{
		const uchar* pRow = sourceImage.ptr<uchar>(i);
		for (int j = 0; j < col; j++, pRow += nChannel)
		{
			for (int o = 0; o < nChannel; o++)
			{
				uchar* p2 = destinationImage.ptr<uchar>(i, j);
				for (int u = 0; u <= 255; u++)
				{
					temp = p2[o] + floor(p[o][u] * 255.0);
					p2[o] += floor(p[o][u] * 255.0);
					if (temp > 255)
					{
						p2[o] = 255;
						break;
					}

					if (pRow[o] < u)
					{
						break;
					}
				}
			}

		}
	}
	return 0;
}

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	if (!histMatrix.data)
		return 0;
	const int bin = 256;
	const size_t noChannels = histMatrix.rows;
	//Cài đặt thông số cho ảnh đầu ra
	const Scalar background_cl(0, 0, 0);
	const Scalar white(255, 255, 255);
	const int height = 480;
	const int width = 512;

	const int bin_width = width / bin;
	const int margin = 4;
	const int miny = margin;
	const int maxy = height - margin;
	histImage = Mat(height, width, CV_8UC3, background_cl);

	Scalar bgr[] = { {0, 0, 255},{0,255,0},	{255,0,0} };
	//Nếu ảnh xám
	if (noChannels == 1) {
		const float* data = histMatrix.ptr<float>(0);
		float max = data[0];
		//Tìm giá trị lớn nhất trong histogram để vẽ hình
		for (int i = 1; i < bin - 1; i++) {
			if (max < data[i])
				max = data[i];
		}
		//Vẽ hình
		for (int i = 1; i < bin - 1; i++) {
			//{x1,y1} và {x2,y2} là các điểm tính toán theo số mức màu (256) và giá trị số lần xuất hiện 
			const int x1 = bin_width * (i - 1);
			const int x2 = bin_width * i;

			const int y1 = int(maxy - ((data[i - 1] / max) * maxy));
			const int y2 = int(maxy - ((data[i] / max) * maxy));
			//Vẽ histogram bằng hàm line nối các cặp điểm {x1,y1} và {x2,y2}
			line(histImage, Point(x1, y1), Point(x2, y2), white, 2, cv::LINE_AA);
			
			
			
		}
	}
	//Nếu ảnh màu
	else {

		float max = 0;
		const float* data = histMatrix.ptr<float>(0);
		//Tìm giá trị lớn nhất trong 3 kênh của histogram để vẽ hình
		for (int i = 0; i < noChannels; i++) {
			int count = 1;
			for (int j = 0; j < bin - 1; j++) {
				if (max < data[j]) {
					max = data[j];
				}
			}
			data = data + 1;
		}
		//Vẽ hình
		for (int i = 0; i < noChannels; i++) {
			const float* data = histMatrix.ptr<float>(i);
			for (int j = 1; j < bin; j++) {
				//{x1,y1} và {x2,y2} là các điểm tính toán theo số mức màu (256) và giá trị số lần xuất hiện 
				const int x1 = bin_width * (j - 1);
				const int x2 = bin_width * j;

				const int y1 = int(maxy - ((data[j - 1] / max) * maxy));
				const int y2 = int(maxy - ((data[j] / max) * maxy));
				//Vẽ histogram bằng hàm line nối các cặp điểm {x1,y1} và {x2,y2}
				line(histImage, Point(x1, y1), Point(x2, y2), bgr[i], 1, cv::LINE_AA);
			}
		}
	}
	return 1;
}

float ColorTransformer::CompareImage(const Mat& image1, Mat& image2)
{
	int hist1[256];
	int hist2[256];
	for (int i = 0; i < 256; i++)
	{
		hist1[i] = 0;
	}

	for (int i = 0; i < 256; i++)
	{
		hist2[i] = 0;
	}

	for (int y = 0; y < image1.rows; y++)
		for (int x = 0; x < image1.cols; x++)
			hist1[(int)image1.at<uchar>(y, x)]++;
	for (int y = 0; y < image2.rows; y++)
		for (int x = 0; x < image2.cols; x++)
			hist2[(int)image2.at<uchar>(y, x)]++;

	float similarity = 0.0; // Do do tuong dong cua 2 anh: similarity
	float normalize_similarity = 100.0 / 256; // Chuan hoa do do ve 100%
	for (int i = 0; i < 256; i++) {

		if (hist1[i] != 0 && hist1[i] <= hist2[i]) {
			similarity += normalize_similarity * (float)hist1[i] / hist2[i];
		}
		else if (hist2[i] != 0 && hist2[i] < hist1[i]) {
			similarity += normalize_similarity * (float)hist2[i] / hist1[i];
		}

	}
	return similarity;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}
