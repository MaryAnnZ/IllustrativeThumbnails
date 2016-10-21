#include <opencv2\core.hpp>

#pragma once
class Image
{
public:
	Image();
	~Image();
	
	cv::Mat getSourceImage();
	cv::Mat	getGrayscaleImage();

	void convertGrayscale();
private:

	
	void loadImage();
	

	cv::Mat sourceImage;
	cv::Mat grayscaleImage;
};

