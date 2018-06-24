#include "snapshot_drawer.h"
#include <fstream>

namespace sim{

//************************************************************

void SnapshotDrawer::draw(const Snapshot &snapshot, const std::string &filename){
	const index_t DIM = snapshot.dimension;
	JiMP2::BMP bmp(DIM*cell_size, DIM*cell_size);

	auto fillCell = [&DIM, &bmp](vec2 pos, Colour clr){
		bmp.drawFillRect(Point(DIM*pos.x, DIM*pos.y),
			Point(DIM*(pos.x+1), DIM*(pos.y+1)), clr);
	};

	if(flags & PEDESTRIANS){
		for(const auto &p : snapshot.pedestrians){
			// auto u = p.second.getPosition();
			// Output::printWarning("Attempt: ({}, {}) -- [{}, {}]/[{}, {}]", u.x, u.y, DIM*u.x,DIM*u.y, DIM*(u.x+1), DIM*(u.y+1));

			fillCell(p.second.getPosition(), Colour::greyScale(0));
		}
	}
	if(flags & DYNAMIC_FIELD){
		const auto &df = snapshot.dynamic_field;
		for(index_t y = 0; y != DIM; ++y){
			for(index_t x = 0; x != DIM; ++x){
				int d = DIM/4;
				int t = 255 - 20*df(x, y);
				uint8_t ut = static_cast<uint8_t>(t<0 ? 0 : t);

				bmp.drawFillRect(Point(x*DIM+d, y*DIM+d),
					Point(DIM*(x+1)-d, DIM*(y+1)-d), {0xff, ut, ut});
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