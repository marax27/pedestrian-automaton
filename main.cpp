#include "matrix.h"
#include "field.h"

#include "bmp.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cmath>
#include <cstdlib>

using sim::fp_t;
using sim::index_t;

const index_t DIM = 100;

int main(){
	srand(time(NULL));
	sim::Field<fp_t> field(DIM);

	for(index_t y = 0; y != DIM; ++y)
		for(index_t x = 0; x != DIM; ++x)
			field(y, x) = 0.0;
	field(50, 50) = 20.0;

	for(int i = 0; i != 25; ++i){
		std::stringstream ss;
		ss << "dumps/" << i << ".bmp";
		std::ofstream writer(ss.str(), std::ios::binary);
		JiMP2::BMP bmp(DIM+2, DIM+2);

		for(index_t y = 0; y != DIM; ++y)
			for(index_t x = 0; x != DIM; ++x){
				auto gg = field.at(y, x) * 256.0;
				uint8_t g = gg > 255.0 ? 255 : (uint8_t)gg;
				bmp.setPixel(x+1, y+1, g, g, g);
			}
		
		writer << bmp;
		writer.close();

		// Update.
		for(index_t y=0; y!=DIM; ++y)
			for(index_t x=0; x!=DIM; ++x){
				auto &p = field(y, x);
				if(!p)
					continue;
				int dx = rand() % 3 - 1;
				int dy = rand() % 3 - 1;
				fp_t df = 0.125 * p;
				p -= df;
				field.at(y + dy, x + dx) += df;
			}

	}
}