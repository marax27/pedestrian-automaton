#include "snapshot_drawer.h"
#include <fstream>

namespace sim{

//************************************************************

void SnapshotDrawer::readAndDraw(const std::string &filename){
	const index_t DIM = getDimension();
	JiMP2::BMP bmp(DIM*cell_size, DIM*cell_size);

	auto fillCell = [&DIM, &bmp](vec2 pos,
		unsigned char r, unsigned char g, unsigned char b){
		bmp.drawFillRect(Point(DIM*pos.x, DIM*pos.y),
			Point(DIM*(pos.x+1), DIM*(pos.y+1)), r, g, b);
	};

	if(flags & PEDESTRIANS){
		for(const auto &p : getPedestrians()){
			// auto u = p.second.getPosition();
			// Output::printWarning("Attempt: ({}, {}) -- [{}, {}]/[{}, {}]", u.x, u.y, DIM*u.x,DIM*u.y, DIM*(u.x+1), DIM*(u.y+1));

			fillCell(p.second.getPosition(), 0, 0, 0);
		}
	}
	if(flags & DYNAMIC_FIELD){
		const auto &df = getDynamicField();
		for(index_t y = 0; y != DIM; ++y){
			for(index_t x = 0; x != DIM; ++x){
				int d = DIM/4;
				int t = 255 - 20*df(x, y);
				t = t<0 ? 0 : t;

				bmp.drawFillRect(Point(x*DIM+d, y*DIM+d),
					Point(DIM*(x+1)-d, DIM*(y+1)-d), 0xff, t, t);
			}
		}
	}
	if(flags & WALLS){
		for(const auto &w : getWalls())
			fillCell(w, 0x66, 0x33, 0);
	}
	if(flags & EXITS){
		for(const auto &x : getExits())
			fillCell(x, 0x66, 0x66, 0xff);
	}

	// Write to file.
	std::ofstream writer(filename);
	if(!writer)
		return;
	writer << bmp;
	writer.close();
}

//************************************************************

}