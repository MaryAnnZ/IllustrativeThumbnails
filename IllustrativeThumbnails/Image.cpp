#include "Image.h"

#include <opencv2\highgui.hpp>
#include <Windows.h>





Image::Image()
{
	loadImage();
}


Image::~Image()
{
}

cv::Mat Image::getImage()
{
	return myImage;
}

void Image::loadImage()
{
	OPENFILENAME dialog;
	TCHAR pathBuffer[260];

	ZeroMemory(&dialog, sizeof(dialog));
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = NULL;
	dialog.lpstrFile = pathBuffer;
	dialog.lpstrFile[0] = '\0';
	dialog.nMaxFile = sizeof(pathBuffer);
	dialog.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	dialog.nFilterIndex = 1;
	dialog.lpstrFileTitle = NULL;
	dialog.nMaxFileTitle = 0;
	dialog.lpstrInitialDir = NULL;
	dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&dialog) == TRUE) {
		char filePath[260];
		int len = MultiByteToWideChar(CP_ACP, 0, dialog.lpstrFile, -1, NULL, 0);
		const wchar_t* file = new wchar_t[len];
		wcstombs(filePath, file, 260);

		myImage = cv::imread(filePath);
	}
}
