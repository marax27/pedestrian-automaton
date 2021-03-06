#include "bmp.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <cstring>

using namespace std;

namespace JiMP2 {

BMP::BMP(uint16_t width, uint16_t height) :
		bitmapCoreHeader(width, height) {

	assert(IS_LITTLE_ENDIAN);
	assert(width > 0);
	assert(height > 0);

	const unsigned int rowSize = ((bitmapCoreHeader.bitsPerPixel * width + 31)
			/ 32) * 4;
	const unsigned int imgSize = rowSize * height;

	bmpFileHeader.size = 14 + bitmapCoreHeader.size + imgSize;
	bmpFileHeader.dataOffset = 14 + bitmapCoreHeader.size;

	pixelData = new unsigned char[imgSize];
	std::memset(pixelData, 255, imgSize);
}

BMP::~BMP() {
	delete[] pixelData;
}

void BMP::setPixel(Point p, Colour clr) {
	assert(bitmapCoreHeader.bitsPerPixel == 24);
	p.y += 1;

	if(p.y > bitmapCoreHeader.bmpHeight || p.x > bitmapCoreHeader.bmpWidth)
		return;

	const size_t rowSize = ((bitmapCoreHeader.bitsPerPixel
			* bitmapCoreHeader.bmpWidth + 31) / 32) * 4;
	const size_t offset = rowSize * (bitmapCoreHeader.bmpHeight - p.y)
			+ p.x * (bitmapCoreHeader.bitsPerPixel / 8);
	
	pixelData[offset + 0] = clr.b;
	pixelData[offset + 1] = clr.g;
	pixelData[offset + 2] = clr.r;
}

void BMP::drawLine(
	Point A, Point B, Colour clr){
	
	int dx =  abs (B.x - A.x), sx = A.x < B.x ? 1 : -1,
	    dy = -abs (B.y - A.y), sy = A.y < B.y ? 1 : -1; 
	int err = dx + dy,
	    e2;
 
	while(true){
		setPixel(A, clr);
		if(A.x == B.x && A.y == B.y)
			break;
		e2 = 2*err;
		if(e2 >= dy){
			err += dy;
			A.x += sx;
		}
		if(e2 <= dx){
			err += dx;
			A.y += sy;
		}
	}
}

void BMP::drawFillRect(Point A, Point B, Colour clr){
	
	if(A.x > B.x)
		std::swap(A.x, B.x);
	if(A.y > B.y)
		std::swap(A.y, B.y);

	for(int y = A.y; y <= B.y; ++y)
		drawLine({A.x, y}, {B.x, y}, clr);
}

std::ostream& operator<<(std::ostream& os, const BMP& bmp) {
	os.write(bmp.bmpFileHeader.id, sizeof(bmp.bmpFileHeader.id));
	os.write((const char*) &bmp.bmpFileHeader.size,
			sizeof(bmp.bmpFileHeader.size));
	os.write(bmp.bmpFileHeader.reserved, sizeof(bmp.bmpFileHeader.reserved));
	os.write((const char*) &bmp.bmpFileHeader.dataOffset,
			sizeof(bmp.bmpFileHeader.dataOffset));

	os.write((const char*) &bmp.bitmapCoreHeader.size,
			sizeof(bmp.bitmapCoreHeader.size));
	os.write((const char*) &bmp.bitmapCoreHeader.bmpWidth,
			sizeof(bmp.bitmapCoreHeader.bmpWidth));
	os.write((const char*) &bmp.bitmapCoreHeader.bmpHeight,
			sizeof(bmp.bitmapCoreHeader.bmpHeight));
	os.write((const char*) &bmp.bitmapCoreHeader.colorPlanes,
			sizeof(bmp.bitmapCoreHeader.colorPlanes));
	os.write((const char*) &bmp.bitmapCoreHeader.bitsPerPixel,
			sizeof(bmp.bitmapCoreHeader.bitsPerPixel));

	const unsigned int rowSize = ((bmp.bitmapCoreHeader.bitsPerPixel
			* bmp.bitmapCoreHeader.bmpWidth + 31) / 32) * 4;
	const unsigned int imgSize = rowSize * bmp.bitmapCoreHeader.bmpHeight;

	os.write((const char*) bmp.pixelData, imgSize);

	return os;
}
}
