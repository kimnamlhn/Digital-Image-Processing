int ColorTransformer::ChangeContrast(const cv::Mat& sourceImage, cv::Mat& destinationImage, float c) {
	for (int i = 0; i < destinationImage.rows; i++)
		for (int j = 0; j < destinationImage.cols; j++)
			for (int k = 0; k < 3; k++) {
				int value = c * destinationImage.at<cv::Vec3b>(i, j)[k];
				//destinationImage.at<cv::Vec3b>(i, j)[k] = cv::saturate_cast<uchar>(c * destinationImage.at<cv::Vec3b>(i, j)[k]);
				destinationImage.at<cv::Vec3b>(i, j)[k] = uchar(value > UCHAR_MAX ? UCHAR_MAX : value > 0 ? value : 0);
			}
	return 1;
}
int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	cv::Mat img = sourceImage.clone();
	destinationImage = Mat(sourceImage.rows, sourceImage.cols, (sourceImage.channels() == 3) ? CV_8UC3 : CV_8UC1);

	int offset = sourceImage.step[0];
	int nChannels = sourceImage.step[1];
	int total = img.rows * img.cols;

	//Step 1: Probability Mass Function (PMF): Calc Frequence
	float prob[256] = { 0 };
	int count[256] = { 0 };

	uchar* pSourceImage = (uchar*)sourceImage.data;
	for (int x = 0; x < sourceImage.rows; x++, pSourceImage += offset) {
		uchar* pRowSourceImage = pSourceImage;
		for (int y = 0; y < sourceImage.cols; y++, pRowSourceImage += nChannels) {
			count[pRowSourceImage[2]]++;
		}
	}
	for (int level = 0; level < 256; level++)
		prob[level] = count[level] * 1.0f / total;

	//Step 2: Cumulative Distributive Function (CDF)
	float cdf[256] = { 0 };
	cdf[0] = prob[0];
	for (int level = 1; level < 256; level++)
		cdf[level] = cdf[level - 1] + prob[level];

	//Step 3: Multiple
	int newLevel[256] = { 0 };
	for (int level = 0; level < 256; level++)
		newLevel[level] = (int)(255 * cdf[level]);

	//Step 4: Mapping
	pSourceImage = (uchar*)sourceImage.data;
	uchar* pDestinationImage = (uchar*)destinationImage.data;
	for (int x = 0; x < sourceImage.rows; x++, pSourceImage += offset, pDestinationImage += offset) {
		uchar* pRowSourceImage = pSourceImage;
		uchar* pRowDestinationImage = pDestinationImage;
		for (int y = 0; y < sourceImage.cols; y++, pRowSourceImage += nChannels, pRowDestinationImage += nChannels) {
			// Update value for each channel
			for (int channel = 0; channel < nChannels; channel++) {
				pRowDestinationImage[channel] = newLevel[pRowSourceImage[channel]];
			}
		}
	}
	return 1;
}