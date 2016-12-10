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
		cvNamedWindow("seamCarved");
		cv::imshow("seamCarved", img->showSeamCarved());
		cvNamedWindow("importanceMap");
		cv::imshow("importanceMap", img->getSaliencyMap());
		cvNamedWindow("seams");
		cv::imshow("seams", img->showSeams());
		cvNamedWindow("source");
		cv::imshow("source", img->getSourceImage());
	}

	
	cvWaitKey(0);
	return;
}