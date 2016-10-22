#include <opencv2\core.hpp>

#pragma once
class Image
{
public:
	Image();
	~Image();
	
	cv::Mat getSourceImage();
	cv::Mat	getGrayscaleImage();
	cv::Mat getLaplaceImage();
	cv::Mat getFilteredLaplaceImage();
	cv::Mat getDilatedImage();
	
private:

	
	void loadImage();
	void convertGrayscale();
	void useLaplace();
	cv::Mat convertBinary(cv::Mat toProcess);
	void removeHorizontalLines();
	cv::Mat cutLine(int startX, int endX, int y, cv::Mat img);
	void doDilation();
	

	cv::Mat sourceImage;
	cv::Mat grayscaleImage;
	cv::Mat laplaceImage;
	cv::Mat filteredLaplaceImage;
	cv::Mat dilatedImage;
};

