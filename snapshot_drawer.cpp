#include "snapshot_drawer.h"
#include <fstream>

namespace sim{

//************************************************************

void SnapshotDrawer::draw(const Snapshot &snapshot, const std::string &filename){
	const index_t DIM = snapshot.dimension;
	JiMP2::BMP bmp(DIM*cell_size, DIM*cell_size);

	auto fillCell = [&bmp, this](vec2 pos, Colour clr){
		bmp.drawFillRect(Point(cell_size*pos.x, cell_size*pos.y),
			Point(cell_size*(pos.x+1), cell_size*(pos.y+1)), clr);
	};

	if(flags & PEDESTRIANS){
		for(const auto &p : snapshot.pedestrians){
			// auto u = p.second.getPosition();
			// Output::printWarning("Attempt: ({}, {}) -- [{}, {}]/[{}, {}]", u.x, u.y, DIM*u.x,DIM*u.y, DIM*(u.x+1), DIM*(u.y+1));

			Colour clr;
			if(flags & HAPPINESS)
				clr = p.second.isHappy() 
				      ? Colour{0,0x56,0x11} : Colour{0x73,0,0};
			else
				clr = Colour::greyScale(0);

			fillCell(p.second.getPosition(), clr);
		}
	}
	if(flags & DYNAMIC_FIELD){
		const auto &df = snapshot.dynamic_field;
		fp_t avg = df.sum() / (df.dimension() * df.dimension());

		for(index_t y = 0; y != DIM; ++y){
			for(index_t x = 0; x != DIM; ++x){
				int d = cell_size/4;
				int t = 255 - 20*atLeastZero(df(x, y) - avg);
				uint8_t ut = static_cast<uint8_t>(atLeastZero(t<0 ? 0 : t));

				bmp.drawFillRect(Point(x*cell_size+d, y*cell_size+d),
					Point(cell_size*(x+1)-d, cell_size*(y+1)-d), {0xff, ut, ut});
			}
		}
	}
	if(flags & WALLS){
		for(const auto &w : snapshot.walls)
			fillCell(w, {0x66, 0x33, 0});
	}
	if(flags & EXITS){
		for(const auto &x : snapshot.exits)
			fillCell(x, {0x66, 0x66, 0xff});
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