#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include "Image.h"

void main()
{
	std::cout << "Hello world!" << std::endl;
//	cv::Mat test;
//	test = cv::imread("images/rainbowDash.jpg");
	Image* img = new Image();
	cvNamedWindow("helloWorld");
	cv::imshow("helloWorld", img->getImage());

	
	cvWaitKey(0);
	return;
}