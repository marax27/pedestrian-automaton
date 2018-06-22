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
	// PATHS = 16
};
typedef uint8_t DrawingFlags;
//------------------------------

class SnapshotDrawer : protected Simulation::Viewer {
public:

	SnapshotDrawer(const Simulation &instance, DrawingFlags drawing_flags)
		: Viewer(instance), flags(drawing_flags), cell_size(32) {}

	void readAndDraw(const std::string &filename);

private:
	DrawingFlags flags;
	int cell_size;
};

}

#endif  //_SNAPSHOT_DRAWER_H_
