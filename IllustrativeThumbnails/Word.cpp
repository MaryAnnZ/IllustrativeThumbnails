#include "Word.h"



Word::Word(cv::Point minCorner, cv::Point maxCorner)
{
	this->minCorner = minCorner;
	this->maxCorner = maxCorner;
	baseline = maxCorner.x;
	width = maxCorner.x - minCorner.x;
	height = maxCorner.y - minCorner.y;
}


Word::~Word()
{
}

int Word::getBaseline()
{
	return baseline;
}

int Word::getHeight()
{
	return height;
}

int Word::getWidth()
{
	return width;
}

cv::Point Word::getMinCorner()
{
	return minCorner;
}

cv::Point Word::getMaxCorner()
{
	return maxCorner;
}
