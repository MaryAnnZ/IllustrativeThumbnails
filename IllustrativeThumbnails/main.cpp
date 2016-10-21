#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include "Image.h"

void main()
{
	std::cout << "Hello world!" << std::endl;
	Image* img = new Image();
	
	if (!img->getSourceImage().empty()) {
		cvNamedWindow("helloWorld");
		cv::imshow("helloWorld", img->getGrayscaleImage());
	}

	
	cvWaitKey(0);
	return;
}