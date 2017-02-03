#include <iostream>
#include <fstream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#include "Image.h"

void main()
{
	std::fstream file("config.txt");
	if (file) {
		std::stringstream configFile;
		configFile << file.rdbuf();
		file.close();
		std::string line;
		std::map<std::string, double> configData;
		while (std::getline(configFile, line)) {
			std::istringstream currentLine(line);
			std::string key;
			if (std::getline(currentLine, key, '=')) {
				std::string value;
				if (std::getline(currentLine, value)) {
					configData[key] = std::stod(value);
				}
			}
		}
		Image* img = new Image(configData);

		if (!img->getSourceImage().empty()) {
			cvNamedWindow("saliency");
			cv::imshow("saliency", img->getOriginalSaliencyMap());
			cvNamedWindow("blurred");
			cv::imshow("blurred", img->getBluredImage());
			cvNamedWindow("string");
			cv::imshow("string", img->getStringImage());
			cvNamedWindow("result");
			cv::imshow("result", img->showSeamCarved());
			cvNamedWindow("importanceMap");
			cv::imshow("importanceMap", img->getSaliencyMap());
			cvNamedWindow("seams");
			cv::imshow("seams", img->showSeams());
			if (img->histSet) {
				cvNamedWindow("marginHist");
				cv::imshow("marginHis", img->getMarginHist());
				cvNamedWindow("contentHist");
				cv::imshow("contentHist", img->getContentHist());
				cvNamedWindow("croppedHist");
				cv::imshow("croppedHist", img->getCroppingHist());
			}
		}


		cvWaitKey(0);
	}
	return;
}