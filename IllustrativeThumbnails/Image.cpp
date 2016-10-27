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
	cv::cvtColor(sourceImage, grayscaleImage, CV_RGB2GRAY);
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
				drawing.at<cv::Vec3b>(cv::Point(i, j)) = cv::Vec3b(255, 0, 0);
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
	double avgHeight = sumHeight / possibleWords.size();
	for (Word w : possibleWords) {
		if (w.getHeight() >= avgHeight && w.getHeight() < avgHeight * 2) {
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
					if ((wMin < min && wMax > min)  || (wMin > min && wMax < max) || (wMin > min && wMax > max)){
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
