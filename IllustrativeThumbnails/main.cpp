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
			if (img->wantOrgImportanceMap) {
				cvNamedWindow("saliency");
				cv::imshow("saliency", img->getOriginalSaliencyMap());
			}
			if (img->wantBlurredImage) {
				cvNamedWindow("blurred");
				cv::imshow("blurred", img->getBluredImage());
			}
			if (img->wantTextImage) {
				cvNamedWindow("string");
				cv::imshow("string", img->getStringImage());
			}
			if (img->wantCroppedImage) {
				cvNamedWindow("cropped");
				cv::imshow("cropped", img->getCroppedImage());
			}
			if (img->wantFinalResult) {
				cvNamedWindow("result");
				cv::imshow("result", img->showSeamCarved());
			}
			if (img->wantImportanceMap) {
				cvNamedWindow("importanceMap");
				cv::imshow("importanceMap", img->getSaliencyMap());
			}
			if (img->wantDebugSeams) {
				cvNamedWindow("seams");
				cv::imshow("seams", img->showSeams());
			}
			if (img->wantHists) {
				cv::Mat margin = img->getMarginHist();
				cv::Mat content = img->getContentHist();
				cv::Mat cropped = img->getCroppingHist();
				if (!margin.empty()) {
					cvNamedWindow("marginHist");
					cv::imshow("marginHist", margin);
					cvNamedWindow("contentHist");
					cv::imshow("contentHist", content);
					cvNamedWindow("croppedHist");
					cv::imshow("croppedHist", cropped);
				}
			}
		}


		cvWaitKey(0);
	}
	return;
}