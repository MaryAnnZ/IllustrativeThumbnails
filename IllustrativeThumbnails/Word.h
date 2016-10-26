#include <opencv2\core.hpp>

#pragma once
class Word
{
public:
	Word(cv::Point minCorner, cv::Point maxCorner);
	~Word();

	int getBaseline();
	int getHeight();
	int getWidth();
	cv::Point getMinCorner();
	cv::Point getMaxCorner();

private:
	int baseline;
	int height;
	int width;
	cv::Point minCorner;
	cv::Point maxCorner;
};

