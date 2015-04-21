#ifndef CBMP_H
#define CBMP_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef void* LPVOID;

/*typedef struct tagBITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffbits;
}
BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}
BITMAPINFOHEADER;
*/
typedef struct tagBMP
{
	LONG bmWidth;
	LONG bmHeight;
	LONG bmWidthBytes;
	WORD bmBitsPixel;
	LPVOID bmBits;
}
BMP;


bool LoadBmp(BMP& bmp, const char* path)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	FILE *fp;
	fopen_s(&fp, path, "rb");
	if (fp == NULL)
		return false;

	//read file
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//init
	bmp.bmWidth = infoHeader.biWidth;
	bmp.bmHeight = infoHeader.biHeight;
	size_t lineSize = (infoHeader.biWidth * infoHeader.biBitCount + 31)/8;
	bmp.bmWidthBytes = lineSize;
	bmp.bmBitsPixel = infoHeader.biBitCount;
	//pixels
	size_t size = lineSize * infoHeader.biHeight;
	bmp.bmBits = malloc(size);
	fseek(fp, fileHeader.bfOffBits, SEEK_SET);
	fread(bmp.bmBits, size, 1, fp);

	fclose(fp);
	return true;
}


#endif CBMP_H