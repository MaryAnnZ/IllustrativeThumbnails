#define NOMINMAX

#include "Image.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d.hpp>
#include <Windows.h>
#include <iostream>





Image::Image()
{
	loadImage();
}


Image::~Image()
{
}

cv::Mat Image::getSourceImage()
{
	return sourceImage;
}

cv::Mat Image::getGrayscaleImage()
{
	if (grayscaleImage.empty()) {
		convertGrayscale();
	}
	return grayscaleImage;
}

cv::Mat Image::getLaplaceImage()
{
	if (laplaceImage.empty()) {
		useLaplace();
	}
	return laplaceImage;
}

cv::Mat Image::getFilteredLaplaceImage()
{
	if (filteredLaplaceImage.empty()) {
		removeHorizontalLines();
		//filteredLaplaceImage = convertBinary(getLaplaceImage());
	}
	return filteredLaplaceImage;
}

cv::Mat Image::getBluredImage()
{
	if (bluredImage.empty()) {
		doBlur();
	}
	return bluredImage;
}

cv::Mat Image::getStringImage()
{
	if (stringImage.empty()) {
		findString();
	}
	return stringImage;
}

cv::Mat Image::getCieluvImage()
{
	if (cieluvImage.empty()) {
		convertToCieluv(cieluvImage);
	}
	return cieluvImage;
}

cv::Mat Image::getSaliencyMap()
{
	if (saliencyMap.empty()) {
		buildGaussPyramid();
		buildContrastPyramid();
		calculateSaliencyMap();
	}
	return saliencyMap;
}

cv::Mat Image::getCentralWindow()
{
	if (croppedImage.empty()) {
		cropBorders();
	}
	return croppedImage;
}

void Image::loadImage()
{
	OPENFILENAME dialog;
	TCHAR pathBuffer[660];

	ZeroMemory(&dialog, sizeof(dialog));
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = NULL;
	dialog.lpstrFile = pathBuffer;
	dialog.lpstrFile[0] = '\0';
	dialog.nMaxFile = sizeof(pathBuffer);
	dialog.lpstrFilter = "Image\0*.JPG;*.PNG\0";
	dialog.nFilterIndex = 1;
	dialog.lpstrFileTitle = NULL;
	dialog.nMaxFileTitle = 0;
	dialog.lpstrInitialDir = NULL;
	dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&dialog) == TRUE) {
		if (dialog.lpstrFile == NULL) {
			return;
		}
		char filePath[660];


		int len = MultiByteToWideChar(CP_ACP, 0, dialog.lpstrFile, -1, NULL, 0);
		wchar_t* file = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, dialog.lpstrFile, -1, file, len);

		wcstombs(filePath, file, 660);

		sourceImage = cv::imread(filePath);
	}
}

void Image::convertGrayscale()
{
	//TODO: RGB or BGR
	cv::cvtColor(sourceImage, grayscaleImage, CV_BGR2GRAY);
}

void Image::useLaplace()
{
	//TODO: set manually?
	int kernelSize = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	cv::Mat distance;

	cv::Laplacian(getGrayscaleImage(), distance, ddepth, kernelSize, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(distance, laplaceImage);

}

cv::Mat Image::convertBinary(cv::Mat toProcess, int th, bool invert)
{
	//TODO: adaptive?
	cv::Scalar color;
	for (int y = 0; y < toProcess.rows; y++) {
		//int countSamePixels = 0;
		for (int x = 0; x < toProcess.cols; x++) {
			color = toProcess.at<uchar>(cv::Point(x, y));
			if (color.val[0] < th) {
				if (invert) {
					toProcess.at<uchar>(cv::Point(x, y)) = 255;
				}
				else {
					toProcess.at<uchar>(cv::Point(x, y)) = 0;
				}
			}
			else {
				if (invert) {
					toProcess.at<uchar>(cv::Point(x, y)) = 0;
				}
				else {
					toProcess.at<uchar>(cv::Point(x, y)) = 255;
				}
			}
		}
	}
	return toProcess;
}

void Image::removeHorizontalLines()
{
	//TODO: set manually
	int offset = 30;
	cv::Mat toProcess = cv::Mat(convertBinary(getLaplaceImage(), 200, false));


	cv::Scalar color;	
	for (int y = 0; y < toProcess.rows; y++) {
		int tmpStart = -1;
		int start = -1;
		int end = -1;
		int countWhitePixels = 0;
		int countBlackPixels = 0;

		for (int x = 0; x < toProcess.cols; x++) {
			color = toProcess.at<uchar>(cv::Point(x, y));
			
			if (color.val[0] == 255) {	
				if (start == -1) {
					start = x;
				}
				countWhitePixels++;
				if (x == toProcess.cols - 1) {
					toProcess = cutLine(start, x, y, toProcess);
				}
			}
			else {
				if (countWhitePixels > offset) {
					toProcess = cutLine(start, x, y, toProcess);
					countBlackPixels = 0;
				}
				countBlackPixels++;
				start = -1;
				countWhitePixels = 0;
			}			
		}
	}
	filteredLaplaceImage = toProcess;
}

cv::Mat Image::cutLine(int startX, int endX, int y, cv::Mat img)
{
	for (int i = startX; i <= endX; i++) {
		img.at<uchar>(cv::Point(i, y)) = 0;
	}
	return img;
}

void Image::doBlur()
{
	int dilationKernel[] = { 1, 1, 1,
							 10, 10, 10,
							1, 1, 1 };
	cv::Mat kernelMat(3, 3, CV_64FC1, dilationKernel);
	//cv::dilate(getFilteredLaplaceImage(), dilatedImage, kernelMat, cv::Point(-1, -1), 1, 0, cv::morphologyDefaultBorderValue());
	cv::Mat test;
	cv::blur(getFilteredLaplaceImage(), bluredImage, cv::Size(15, 1));
}

void Image::findString()
{
	cv::Mat toProcess = convertBinary(getBluredImage(), 100, false);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(toProcess, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::Mat drawing = cv::Mat::zeros(toProcess.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {
		cv::Scalar color = cv::Scalar(255, 255, 0);
		cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		std::vector<cv::Point> myObjectContours = contours.at(i);
		std::vector<int> xCoords;
		std::vector<int> yCoords;
		for (int j = 0; j < myObjectContours.size(); j++) {
			xCoords.push_back(myObjectContours.at(j).x);
			yCoords.push_back(myObjectContours.at(j).y);
		}
		std::sort(xCoords.begin(), xCoords.end());
		std::sort(yCoords.begin(), yCoords.end());
		int width = xCoords.at(xCoords.size() - 1) - xCoords.at(0);
		int height = yCoords.at(yCoords.size() - 1) - yCoords.at(0);
		if (width > 2 * height) {
			markAsWord(myObjectContours);
		}
	}

	checkWords();

	for (Word w : words) {
		for (int i = w.getMinCorner().x; i <= w.getMaxCorner().x; i++) {
			for (int j = w.getMinCorner().y; j <= w.getMaxCorner().y; j++) {
				drawing.at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(100, 0, 0);
			}
		}
	}

	stringImage = drawing;
}


void Image::markAsWord(std::vector<cv::Point> contours)
{
	//find corners
	cv::Point minCorner = cv::Point(-1, -1);
	cv::Point maxCorner = cv::Point(-1, -1);
	for (int i = 0; i < contours.size(); i++) {
		cv::Point contourPoint = contours.at(i);
		// minCorner
		if (minCorner.x == -1 || minCorner.y == -1) {
			minCorner = contourPoint;
		}
		else {
			if (contourPoint.x < minCorner.x) {
				minCorner.x = contourPoint.x;
			}
			if (contourPoint.y < minCorner.y) {
				minCorner.y = contourPoint.y;
			}
		}
		// maxCorner
		if (maxCorner.x == -1 || maxCorner.y == -1) {
			maxCorner = contourPoint;
		}
		else {
			if (contourPoint.x > maxCorner.x) {
				maxCorner.x = contourPoint.x;
			}
			if (contourPoint.y > maxCorner.y) {
				maxCorner.y = contourPoint.y;
			}
		}
	}

	int height = maxCorner.y - minCorner.y;
	int width = maxCorner.x - minCorner.x;
	if (height > 5 && width > 5) {
		possibleWords.push_back(Word(minCorner, maxCorner));
	}


}

void Image::checkWords()
{
	int sumHeight = 0;
	std::vector<int> minY;
	std::vector<int> maxY;
	for (Word w : possibleWords) {
		sumHeight += w.getHeight();
		minY.push_back(w.getMinCorner().y);
		maxY.push_back(w.getMaxCorner().y);
	}
	if (!possibleWords.empty()) {
		double avgHeight = sumHeight / possibleWords.size();
		for (Word w : possibleWords) {
			if (w.getHeight() >= avgHeight * 0.5 && w.getHeight() < avgHeight * 200) {
				if (checkHistogram(w)) {
					bool maxxOk = true;
					if ((std::find(minY.begin(), minY.end(), w.getMaxCorner().y) != minY.end())) {
						std::vector<int> occurs;
						std::vector<int>::iterator iter = minY.begin();
						while ((iter = std::find(iter, minY.end(), w.getMaxCorner().y)) != minY.end()) {
							occurs.push_back(std::distance(minY.begin(), iter));
							iter++;
						}
						for (int o : occurs) {
							Word word = possibleWords.at(o);
							int min = word.getMinCorner().x;
							int max = word.getMaxCorner().x;
							int wMin = w.getMinCorner().x;
							int wMax = w.getMaxCorner().x;
							if ((wMin < min && wMax > min) || (wMin > min && wMax < max) || (wMin > min && wMax > max)) {
								maxxOk = false;
								break;
							}
						}
					}
					if (maxxOk) {
						if ((std::find(maxY.begin(), maxY.end(), w.getMinCorner().y) != maxY.end())) {
							std::vector<int> occurs;
							std::vector<int>::iterator iter = maxY.begin();
							while ((iter = std::find(iter, maxY.end(), w.getMinCorner().y)) != maxY.end()) {
								occurs.push_back(std::distance(maxY.begin(), iter));
								iter++;
							}
							for (int o : occurs) {
								Word word = possibleWords.at(o);
								int min = word.getMinCorner().x;
								int max = word.getMaxCorner().x;
								int wMin = w.getMinCorner().x;
								int wMax = w.getMaxCorner().x;
								if ((wMin < min && wMax > min) || (wMin > min && wMax < max) || (wMin > min && wMax > max)) {
									maxxOk = false;
									break;
								}
							}
						}
					}
					if (maxxOk) {
						words.push_back(w);
					}
					else {
						std::cout << "NOT WORD 2" << std::endl;
					}
				}
				else {
					std::cout << "NOT WORD" << std::endl;
				}
			}
		}
	}

}

bool Image::checkHistogram(Word word)
{
	cv::Rect rect = cv::Rect(word.getMinCorner(), word.getMaxCorner());
	cv::Mat source = getGrayscaleImage();
	cv::Mat roi = source(rect);
	int histSize = 10;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	cv::Mat histogram;
	cv::calcHist(&roi, 1, 0, cv::Mat(), histogram, 1, &histSize, &histRange, true, false);
	int sumPixels = word.getWidth() * word.getHeight();
	int sumMax1 = 0;
	int sumMax2 = 0;
	for (int i = 0; i < histogram.rows; i++) {
		int curr = histogram.at<float>(i);
		if (curr > std::min(sumMax1, sumMax2)) {
			if (sumMax1 > sumMax2) {
				sumMax2 = curr;
			}
			else {
				sumMax1 = curr;
			}
		}
	}
	if ((sumMax1 + sumMax2) > (sumPixels * 0.65)) {
		return true;
	}
	else {
		return false;
	}
}

void Image::convertToCieluv(cv::Mat & img)
{
	cv::cvtColor(getSourceImage(), img, CV_BGR2Luv);
}

void Image::buildGaussPyramid()
{
	int pyramidLevels = (int)log2(getCieluvImage().rows / 10);
	if (gaussPyramid.empty()) {
		for (int i = 0; i < pyramidLevels; i++) {
			cv::Mat nextLevel;
			cv::Mat baseImage;
			if (i == 0) {		// getCieluv image
				baseImage = getCieluvImage();
				gaussPyramid.push_back(baseImage);
			}
			else {				//get image of the index before
				baseImage = gaussPyramid.at(i - 1);
			}
			cv::pyrDown(baseImage, nextLevel);
			gaussPyramid.push_back(nextLevel);
		}
	}
}

void Image::buildContrastPyramid()
{
	if (!gaussPyramid.empty()) {
		for (int i = 0; i < gaussPyramid.size(); i++) {
			cv::Mat gaussImage = gaussPyramid.at(i);
			cv::Mat contrastMap = cv::Mat::zeros(gaussImage.size(), CV_8UC1);
			int maxX, maxY;
			if (gaussImage.rows % 2 == 1) {
				maxY = (gaussImage.rows - 1) / 2;
			}
			else {
				maxY = gaussImage.rows / 2;
			}
			if (gaussImage.cols % 2 == 1) {
				maxX = (gaussImage.cols - 1);
			}
			else {
				maxX = gaussImage.cols;
			}
			double maxDistance = cv::sqrt(maxX * maxX + maxY * maxY);
			
			//calculate only full neigbourhood
			for (int x = 1; x < gaussImage.cols - 1; x++) {
				for (int y = 1; y < gaussImage.rows - 1; y++) {
					double colorDistances = 0.0;
					cv::Scalar myPixel = gaussImage.at<cv::Vec3b>(cv::Point(x, y));
					//gaussImage.at<uchar>(cv::Point(x, y)) = 255;
					cv::Scalar neighbour1 = gaussImage.at<cv::Vec3b>(cv::Point(x-1, y));
					cv::Scalar neighbour2 = gaussImage.at<cv::Vec3b>(cv::Point(x, y-1));
					cv::Scalar neighbour3 = gaussImage.at<cv::Vec3b>(cv::Point(x, y+1));
					cv::Scalar neighbour4 = gaussImage.at<cv::Vec3b>(cv::Point(x+1, y));
					colorDistances += cv::norm(myPixel, neighbour1, cv::NORM_L2);
					colorDistances += cv::norm(myPixel, neighbour2, cv::NORM_L2);
					colorDistances += cv::norm(myPixel, neighbour3, cv::NORM_L2);
					colorDistances += cv::norm(myPixel, neighbour4, cv::NORM_L2);
					int xDist = maxX - x;
					int yDist = maxY - y;
					double myDistance = cv::sqrt(xDist * xDist + yDist * yDist);
					double weight = 1 - (myDistance / maxDistance);
					contrastMap.at<uchar>(cv::Point(x, y)) = colorDistances * weight;
				}
			}
			//cut the unfiltered margins
			if (contrastMap.cols >= 3 && contrastMap.rows >= 3) {
				contrastMap = contrastMap.rowRange(1, contrastMap.rows - 1);
				contrastMap = contrastMap.colRange(1, contrastMap.cols - 1);
				contrastPyramid.push_back(contrastMap);
			}
		}
	}
}

void Image::calculateSaliencyMap()
{

	if (!contrastPyramid.empty()) {
		cv::Mat mySaliencyMap = cv::Mat::zeros(getCieluvImage().size(), CV_8UC1);
		double mapWeight = 1.0 / contrastPyramid.size();
		for (int i = 0; i < contrastPyramid.size(); i++) {
			cv::Mat contrastMap;
			cv::resize(contrastPyramid.at(i), contrastMap, mySaliencyMap.size());
			for (int x = 0; x < mySaliencyMap.cols; x++) {
				for (int y = 0; y < mySaliencyMap.rows; y++) {
					mySaliencyMap.at<uchar>(cv::Point(x, y)) = mySaliencyMap.at<uchar>(cv::Point(x, y)) + (contrastMap.at<uchar>(cv::Point(x, y)) * mapWeight);
				}
			}
		}
		cv::Mat textImage = getStringImage();
		for (int x = 0; x < textImage.cols; x++) {
			for (int y = 0; y < textImage.rows; y++) {
				cv::Scalar color = textImage.at<cv::Vec3b>(cv::Point(x, y));
				if (color.val[0] == 100) {
					mySaliencyMap.at<uchar>(cv::Point(x, y)) = mySaliencyMap.at<uchar>(cv::Point(x, y)) * 2;
				}
			}
		}
		cv::normalize(mySaliencyMap, saliencyMap, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	}
}

void Image::cropBorders()
{
	cv::Mat source = getSourceImage();
	int upperBorder = source.rows * 0.2;
	int upperCroppingPoint = 0;
	//detect line
	for (int y = 0; y < upperBorder; y++) {
		for (int x = 1; x < source.cols; x++) {
			cv::Scalar color1 = source.at<cv::Vec3b>(cv::Point(x - 1, y));
			cv::Scalar color2 = source.at<cv::Vec3b>(cv::Point(x, y));
			if (color1 != color2) {
				break;
			}
			else {
				if (x == (source.cols - 1)) {
					upperCroppingPoint = y;
				}
			}
		}
	}
	int lowerCroppingPoint = source.rows;
	for (int y = source.rows; y > source.rows - upperBorder; y--) {
		for (int x = 1; x < source.cols; x++) {
			cv::Scalar color1 = source.at<cv::Vec3b>(cv::Point(x - 1, y));
			cv::Scalar color2 = source.at<cv::Vec3b>(cv::Point(x, y));
			if (color1 != color2) {
				break;
			}
			else {
				if (x == (source.cols - 1)) {
					lowerCroppingPoint = y;
				}
			}
		}
	}
	croppedImage = source.rowRange(upperCroppingPoint, lowerCroppingPoint);
	//calculate histogram
	if (lowerCroppingPoint >source.rows - upperBorder) {
		int step = source.rows * 0.025;
		int binAmount = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		//margin hist
		cv::vector<cv::Mat> bgrPlanesMargin;
		cv::split(croppedImage.rowRange(croppedImage.rows - step, croppedImage.rows), bgrPlanesMargin);
		cv::Mat bHistMargin, gHistMargin, rHistMargin;
		cv::calcHist(&bgrPlanesMargin[0], 1, 0, cv::Mat(), bHistMargin, 1, &binAmount, &histRange, true, false);
		cv::normalize(bHistMargin, bHistMargin, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		cv::calcHist(&bgrPlanesMargin[1], 1, 0, cv::Mat(), gHistMargin, 1, &binAmount, &histRange, true, false);
		cv::normalize(gHistMargin, gHistMargin, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		cv::calcHist(&bgrPlanesMargin[2], 1, 0, cv::Mat(), rHistMargin, 1, &binAmount, &histRange, true, false);
		cv::normalize(rHistMargin, rHistMargin, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		//content hist
		cv::vector<cv::Mat> bgrPlanesContent;
		cv::split(croppedImage.rowRange(croppedImage.rows - upperBorder, croppedImage.rows - upperBorder + step), bgrPlanesContent);
		cv::Mat bHistContent, gHistContent, rHistContent;
		cv::calcHist(&bgrPlanesContent[0], 1, 0, cv::Mat(), bHistContent, 1, &binAmount, &histRange, true, false);
		cv::normalize(bHistContent, bHistContent, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		cv::calcHist(&bgrPlanesContent[1], 1, 0, cv::Mat(), gHistContent, 1, &binAmount, &histRange, true, false);
		cv::normalize(gHistContent, gHistContent, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		cv::calcHist(&bgrPlanesContent[2], 1, 0, cv::Mat(), rHistContent, 1, &binAmount, &histRange, true, false);
		cv::normalize(rHistContent, rHistContent, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
		for (int i = croppedImage.rows; i > croppedImage.rows - upperBorder; i -= step) {
			//test histogram
			cv::vector<cv::Mat> bgrPlanesCompare;
			cv::split(croppedImage.rowRange(i - step, i), bgrPlanesCompare);
			cv::Mat bHistCompare, gHistCompare, rHistCompare;
			cv::calcHist(&bgrPlanesCompare[0], 1, 0, cv::Mat(), bHistCompare, 1, &binAmount, &histRange, true, false);
			cv::normalize(bHistCompare, bHistCompare, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
			cv::calcHist(&bgrPlanesCompare[1], 1, 0, cv::Mat(), gHistCompare, 1, &binAmount, &histRange, true, false);
			cv::normalize(gHistCompare, gHistCompare, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
			cv::calcHist(&bgrPlanesCompare[2], 1, 0, cv::Mat(), rHistCompare, 1, &binAmount, &histRange, true, false);
			cv::normalize(rHistCompare, rHistCompare, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
			//compare
			double marginB = cv::compareHist(bHistMargin, bHistCompare, CV_COMP_CORREL);
			double marginG = cv::compareHist(gHistMargin, gHistCompare, CV_COMP_CORREL);
			double marginR = cv::compareHist(rHistMargin, rHistCompare, CV_COMP_CORREL);
			double contentB = cv::compareHist(bHistContent, bHistCompare, CV_COMP_CORREL);
			double contentG = cv::compareHist(gHistContent, gHistCompare, CV_COMP_CORREL);
			double contentR = cv::compareHist(rHistContent, rHistCompare, CV_COMP_CORREL);
			double marginCorrel = marginB + marginG + marginR;
			double contentCorrel = contentB + contentG + contentR;
			if (contentCorrel > marginCorrel) {
				croppedImage = croppedImage.rowRange(0, i - step);
				std::cout << "hist crop by: " << i << std::endl;
				break;
			}
		}
	}
}
