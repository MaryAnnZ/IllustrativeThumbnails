#include "Image.h"

#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <Windows.h>





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
