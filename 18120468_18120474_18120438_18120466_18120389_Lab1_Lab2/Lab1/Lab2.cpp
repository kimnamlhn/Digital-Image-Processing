AffineTransform::AffineTransform() {
	// Xây dựng ma trận biến đổi đơn vị
	_matrixTransform = Mat(3, 3, CV_32FC1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j)
				_matrixTransform.at<float>(i, j) = (float)1;
			else {
				_matrixTransform.at<float>(i, j) = (float)0;
			}
		}
	}
}

void AffineTransform::Translate(float dx, float dy)
{
	//Tạo matrix tạm temp
	Mat temp;
	temp.create(3, 3, CV_32FC1);
	//Khởi tạo matrix 3x3 với [1, 0, 0]
	//						  [0, 1, 0]
	//						  [0, 0, 1]	
	int width = temp.cols, height = temp.rows;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (x == y)
				temp.at<float>(x, y) = 1;
			else
				temp.at<float>(x, y) = 0;
	//Gán các giá trị dx, dy tại (0, 2), (1, 2) tạo ma trận tịnh tiến
	temp.at<float>(0, 2) = dx;
	temp.at<float>(1, 2) = dy;
	//Nhân matrix gốc với ma trận tịnh tiến
	_matrixTransform = _matrixTransform * temp;

	/*for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			std::cout << _matrixTransform.at<float>(i, j) << " ";
		std::cout << "\n";
	}

}

void AffineTransform::Rotate(float angle)
{
	//Tính sinA, cosA và chuyển góc angle về radian
	angle = angle * M_PI / 180;
	float sinAlpha = sinf(angle), cosAlpha = cosf(angle);

	//Tạo matrix temp là ma trận scale
	Mat temp;
	temp.create(3, 3, CV_32FC1);
	//Khởi tạo matrix 3x3 với [1, 0, 0]
	//						  [0, 1, 0]
	//						  [0, 0, 1]	
	int width = temp.cols, height = temp.rows;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (x == y)
				temp.at<float>(x, y) = 1;
			else
				temp.at<float>(x, y) = 0;
	//Gán các giá trị cosA, -sinA, sinA, cosA  tại (0, 0), (0, 1), (1, 0), (1, 1) tạo ma trận xoay
	temp.at<float>(0, 0) = cosAlpha;
	temp.at<float>(0, 1) = -sinAlpha;
	temp.at<float>(1, 0) = sinAlpha;
	temp.at<float>(1, 1) = cosAlpha;
	//Nhân matrix gốc với ma trận tịnh tiến
	_matrixTransform = _matrixTransform * temp;

	

}

void AffineTransform::Scale(float sx, float sy)
{
	//Tạo matrix temp là ma trận scale
	Mat temp;
	temp.create(3, 3, CV_32FC1);
	//Khởi tạo matrix 3x3 với [1, 0, 0]
	//						  [0, 1, 0]
	//						  [0, 0, 1]	
	int width = temp.cols, height = temp.rows;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (x == y)
				temp.at<float>(x, y) = 1;
			else
				temp.at<float>(x, y) = 0;
	//Gán các giá trị sx, sy tại(0, 0), (1, 1) tạo ma trận Scale
	temp.at<float>(0, 0) = sx;
	temp.at<float>(1, 1) = sy;
	//Nhân matrix gốc với ma trận tịnh tiến
	_matrixTransform = _matrixTransform * temp;

	//Gán các giá trị sx, sy tại (0, 0), (1, 1)
	/*_matrixTransform.at<float>(0, 0) = sx;
	_matrixTransform.at<float>(1, 1) = sy;*/
}

void AffineTransform::TransformPoint(float & x, float & y)
{
	float P[3] = { x, y, 1 }, P1[3] = { 0, 0, 0 }; //Tạo 2 ma trận điểm ảnh nguồn và đích
	//Nhân ma trận nguồn với ma trận chuyển đổi
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			P1[i] += P[j] * _matrixTransform.at<float>(i, j);
	//Gán lại giá trị mới cho điểm ảnh
	x = P1[0];
	y = P1[1];
}




int GeometricTransformer::RotateKeepImage(const Mat & srcImage, Mat & dstImage, float angle, PixelInterpolate * interpolator)
{
	if (srcImage.empty())
		return 0;

	//Khởi tạo đối tượng affineTf và gọi phương thức Rotate
	AffineTransform * affineTf = new AffineTransform();
	AffineTransform * affineTf1 = new AffineTransform();

	float angle1 = angle * M_PI / 180;
	int width = srcImage.cols, height = srcImage.rows;
	int dstWidth = width * fabs(cosf(angle1)) + height * fabs(sinf(angle1));
	int dstHeight = width * fabs(sinf(angle1)) + height * fabs(cosf(angle1));

	float srcX0 = height / 2;
	float srcY0 = width / 2;
	float dstX0 = dstHeight / 2;
	float dstY0 = dstWidth / 2;
	//Khởi tạo ảnh đích có kích thước và type giống ảnh nguồn
	dstImage.create(dstHeight, dstWidth, srcImage.type());
	//Lấy tâm của ảnh thật khi quay góc angle
	affineTf1->Rotate(angle);
	affineTf1->TransformPoint(srcX0, srcY0);
	//Quay góc -angle, và tịnh tiến theo vector (- (x0 - xCenter), - (y0 - yCenter));
	//Vì nội suy màu
	affineTf->Rotate(-angle);
	affineTf->Translate(-(dstImage.rows / 2 - srcX0), -(dstImage.cols / 2 - srcY0));

	Transform(srcImage, dstImage, affineTf, interpolator);
	return 1;
}

int GeometricTransformer::RotateUnkeepImage(const Mat & srcImage, Mat & dstImage, float angle, PixelInterpolate * interpolator)
{
	if (srcImage.empty())
		return 0;

	AffineTransform * affineTf = new AffineTransform();
	AffineTransform * affineTf1 = new AffineTransform();
	//Khởi tạo ảnh đích có kích thước và type giống ảnh nguồn
	float xCenter = srcImage.rows*1.0f / 2, yCenter = srcImage.cols * 1.0f / 2;
	dstImage.create(srcImage.rows, srcImage.cols, srcImage.type());

	//Lấy tâm của ảnh thật khi quay góc angle
	affineTf1->Rotate(angle);
	affineTf1->TransformPoint(xCenter, yCenter);
	//Quay góc -angle, và tịnh tiến theo vector (- (x0 - xCenter), - (y0 - yCenter));
	//Vì nội suy màu
	affineTf->Rotate(-angle);
	affineTf->Translate(-(srcImage.rows / 2 - xCenter), -(srcImage.cols / 2 - yCenter));
	
	Transform(srcImage, dstImage, affineTf, interpolator);

	return 1;
}
