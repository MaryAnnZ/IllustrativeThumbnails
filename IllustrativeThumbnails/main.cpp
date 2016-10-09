#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

void main()
{
	std::cout << "Hello world!" << std::endl;
	cv::Mat test;
	test = cv::imread("images/rainbowDash.jpg");
	cvNamedWindow("helloWorld");
	cv::imshow("helloWorld", test);

	
	cvWaitKey(0);
	return;
}