#ifndef __BMP_H
#define __BMP_H

#include <stdint.h>
#include <iostream>

//************************************************************

struct Point{
	Point(uint16_t _x, uint16_t _y) : x(_x), y(_y) {}
	uint16_t x, y;
};

struct Colour{
	uint8_t r, g, b;

	static Colour greyScale(uint8_t gs){
		return Colour{gs, gs, gs};
	}
};

//************************************************************

// define IS_LITTLE_ENDIAN (!!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)
#define IS_LITTLE_ENDIAN (*(uint16_t *)"\0\xff" > 0x100)

namespace JiMP2 {

/*
 * Informacje o formacie BMP https://en.wikipedia.org/wiki/BMP_file_format
 */

struct BMPFileHeader {
	char id[2];
	uint32_t size;
	char reserved[4];
	uint32_t dataOffset;

	BMPFileHeader() :
			id { 'B', 'M' }, size(0), dataOffset(0) {
		//id[0] = 'B';
		//id[1] = 'M';
	}
};

struct BitmapCoreHeader {
	const uint32_t size;
	const uint16_t bmpWidth;
	const uint16_t bmpHeight;
	const uint16_t colorPlanes;
	const uint16_t bitsPerPixel;

	BitmapCoreHeader(uint16_t width, uint16_t height) :
			size(12), bmpWidth(width), bmpHeight(height), colorPlanes(1),
			bitsPerPixel(24)
	{
	}

};

class BMP {
	BMPFileHeader bmpFileHeader;
	BitmapCoreHeader bitmapCoreHeader;
	unsigned char* pixelData;

public:

	BMP(uint16_t width, uint16_t height);
	virtual ~BMP();

	void setPixel(Point p, Colour clr);
	
	void drawLine(Point A, Point B, Colour clr);

	void drawFillRect(Point A, Point B, Colour clr);

	uint16_t getWidth() const { return bitmapCoreHeader.bmpWidth; }
	uint16_t getHeight() const { return bitmapCoreHeader.bmpHeight; }

	friend std::ostream& operator<<(std::ostream& os, const BMP& bmp);
};

}

#endif
