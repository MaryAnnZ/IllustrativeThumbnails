#include <opencv2\core.hpp>
#include "Word.h"

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
	cv::Mat getBluredImage();
	cv::Mat getStringImage();
	
private:

	
	void loadImage();
	void convertGrayscale();
	void useLaplace();
	cv::Mat convertBinary(cv::Mat toProcess, int th, bool invert);
	void removeHorizontalLines();
	cv::Mat cutLine(int startX, int endX, int y, cv::Mat img);
	void doBlur();
	void findString();
	void markAsWord(std::vector<cv::Point> contours);
	void checkWords();


	

	cv::Mat sourceImage;
	cv::Mat grayscaleImage;
	cv::Mat laplaceImage;
	cv::Mat filteredLaplaceImage;
	cv::Mat bluredImage;
	cv::Mat stringImage;

	std::vector<Word> possibleWords;
	std::vector<Word> words;
	
};

