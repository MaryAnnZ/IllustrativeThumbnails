#include <opencv2\core.hpp>

#pragma once
class Image
{
public:
	Image();
	~Image();
	
	cv::Mat getImage();
private:

	
	void loadImage();

	cv::Mat myImage;
};

