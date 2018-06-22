#ifndef _CHART_H_
#define _CHART_H_

#include "simulation.h"
#include "bmp.h"

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

namespace sim{

class Chart : protected Simulation::Viewer {
public:
	Chart(const Simulation &instance) : Viewer(instance) {}

	// Read necessary data from Simulation object.
	virtual void read() = 0;
};

//************************************************************

class PopulationChart : public Chart {
public:
	PopulationChart(const Simulation &instance) : Chart(instance) {}

	void read() {
		data.push_back( Chart::getPedestrians().size() );
	}

	// Save a chart in a BMP file.
	void draw(const std::string &filename) const {
		const int STEP = 5;
		int w = data.size() * STEP;
		int h = 100;
		JiMP2::BMP bmp(w, h);
		auto max_n = *(std::max_element(data.begin(), data.end()));

		// Linear normalization.
		auto norm = [&h, &max_n](std::size_t n){
			return static_cast<int>(h * (fp_t)(n) / max_n);
		};

		// Grid.
		unsigned char gs = 0xaa;
		bmp.drawLine(Point(0, h/4), Point(w, h/4), gs, gs, gs);
		bmp.drawLine(Point(0, h/2), Point(w, h/2), gs, gs, gs);
		bmp.drawLine(Point(0, 3*h/4), Point(w, 3*h/4), gs, gs, gs);

		for(std::size_t i = 0; i != data.size(); ++i){
			int y = h - norm(data[i]);
			bmp.drawLine(Point(i*STEP, y), Point((i+1)*STEP, y), 0xff, 0, 0);
		}

		std::ofstream writer(filename, std::ios::binary);
		writer << bmp;
		writer.close();
	}

private:
	std::vector<std::size_t> data;
};

//************************************************************

}

#endif  //_CHART_H_
