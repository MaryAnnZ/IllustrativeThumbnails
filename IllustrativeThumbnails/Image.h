#include <opencv2\core.hpp>
#include "Word.h"

#pragma once
class Image
{
public:
	Image();
	~Image();
	
	cv::Mat getSourceImage();
	
	//text segmentation
	cv::Mat	getGrayscaleImage();
	cv::Mat getLaplaceImage();
	cv::Mat getFilteredLaplaceImage();
	cv::Mat getBluredImage();
	cv::Mat getStringImage();

	//scale invariant saliency map
	cv::Mat getCieluvImage();
	cv::Mat getSaliencyMap();
	

	
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
	void convertToCieluv(cv::Mat& img);
	void buildGaussPyramid();
	void buildContrastPyramid();
	void calculateSaliencyMap();


	
	cv::Mat sourceImage;

	// Text segmentation
	cv::Mat grayscaleImage;
	cv::Mat laplaceImage;
	cv::Mat filteredLaplaceImage;
	cv::Mat bluredImage;
	cv::Mat stringImage;

	//scale invariant saliency map
	cv::Mat cieluvImage;
	std::vector<cv::Mat> gaussPyramid;
	std::vector<cv::Mat> contrastPyramid;
	cv::Mat saliencyMap;
	cv::Mat weightedSaliencyMap;

	std::vector<Word> possibleWords;
	std::vector<Word> words;
	
};

