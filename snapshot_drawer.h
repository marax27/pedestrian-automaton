#ifndef _SNAPSHOT_DRAWER_H_
#define _SNAPSHOT_DRAWER_H_

#include <string>

#include "simulation.h"
#include "bmp.h"

namespace sim{

//------------------------------
enum e_DrawFlag : uint8_t {
	PEDESTRIANS = 1,
	WALLS = 2,
	EXITS = 4,
	DYNAMIC_FIELD = 8,
	HAPPINESS = 16
};
typedef uint8_t DrawingFlags;
//------------------------------

class SnapshotDrawer{
public:

	SnapshotDrawer(DrawingFlags drawing_flags, uint16_t _cell_size = 32)
		: flags(drawing_flags), cell_size(_cell_size) {}

	void draw(const Snapshot &snapshot, const std::string &filename);

private:
	DrawingFlags flags;
	uint16_t cell_size;
};

}

#endif  //_SNAPSHOT_DRAWER_H_
