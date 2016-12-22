#include <opencv2\core.hpp>
#include "Word.h"

#pragma once
class Image
{
public:
	Image(std::map<std::string, double> configData);
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

	//central window
	cv::Mat getCroppedImage();

	cv::Mat showSeamCarved();
	cv::Mat showSeams();

	cv::Mat getReference();
	
	
private:
	double size;
	
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
	bool checkHistogram(Word word);
	void convertToCieluv(cv::Mat& img);
	void buildGaussPyramid();
	void buildContrastPyramid();
	void calculateSaliencyMap();
	void cropHorizontalBorders();
	void cropVerticalBorders();


	
	cv::Mat sourceImage;

	// Text segmentation
	cv::Mat grayscaleImage;
	cv::Mat laplaceImage;
	cv::Mat filteredLaplaceImage;
	cv::Mat bluredImage;
	cv::Mat stringImage;
	int kernelSizeLaplace;
	int scaleLaplace;
	int deltaLaplace;

	//scale invariant saliency map
	cv::Mat cieluvImage;
	std::vector<cv::Mat> gaussPyramid;
	std::vector<cv::Mat> contrastPyramid;
	cv::Mat saliencyMap;
	cv::Mat weightedSaliencyMap;
	cv::Mat isTextMap;

	//important central window
	cv::Mat croppedImage;
	double borderSize;
	double borderSteps;
	double lowerBorderCorr;
	double upperBorderCorr;
	double leftBorderCorr;
	double rightBorderCorr;

	std::vector<Word> possibleWords;
	std::vector<Word> words;
	int horizontalLineOffset;
	int convertBinaryTh;
	int blurWidth;
	int blurHeight;
	int findStringBinaryTh;
	int minWordWidth;
	int minWordHeight;
	double minAvgHeight;
	double maxAvgHeight;
	int histSize;
	int minHistRange;
	int maxHistRange;
	double histTh;
	int wordWeight;

	//seam carving
	//source: http://eric-yuan.me/seam-carving/
	typedef struct Entity {
		double data;
		int path;
	}entity;
	int whichMin(float x, float y);
	int whichMin(float x, float y, float z);
	int whichMin(float x, float y, float z, float v);
	int whichMin(float x, float y, float z, float v, float w);
	std::vector<std::vector<Entity>> calculateSeams(bool vertical);
	bool doLines;
	typedef struct minPixel {
		int index;
		double intensity;
	};
	minPixel findStartingPoint(std::vector<std::vector<Entity>> pathValues);
	double findMaxImportance(std::vector<std::vector<Entity>> pathValues);
	void findVerticalPath(minPixel startingPoint, std::vector<std::vector<Entity>> pathValues);
	void drawSeams(std::vector<std::vector<Entity>> pathValues);
	cv::Mat verticalSeamsImage;
	cv::Mat showSeamsImage;
	int debugSeamAmount;
	double resamplingTh;
	int goalWidth;
	int goalHeight;
	
};

