#ifndef _CHART_H_
#define _CHART_H_

#include "simulation.h"
#include "updateable.h"
#include "bmp.h"

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

namespace sim{

class Chart : protected Simulation::Viewer, public Updateable {
public:
	Chart(const Simulation &instance, Colour clr)
		: Viewer(instance), chart_colour(clr) {}
	
protected:
	Colour chart_colour;
};

//************************************************************

class PopulationChart : public Chart {
public:
	PopulationChart(const Simulation &instance, Colour clr)
		: Chart(instance, clr) {}

	void update() {
		data.push_back( Chart::getPedestrians().size() );
	}

	// Save a chart in a BMP file.
	void saveToFile(const std::string &filename) const {
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
		bmp.drawLine(Point(0, h/4), Point(w, h/4), Colour::greyScale(gs));
		bmp.drawLine(Point(0, h/2), Point(w, h/2), Colour::greyScale(gs));
		bmp.drawLine(Point(0, 3*h/4), Point(w, 3*h/4), Colour::greyScale(gs));

		for(std::size_t i = 0; i != data.size(); ++i){
			int y = h - norm(data[i]);
			bmp.drawLine(Point(i*STEP, y), Point((i+1)*STEP, y), chart_colour);
			if(i > 0){
				// Vertical line, if the gap between horizontal
				// lines is too large.
				auto y1 = h - norm(data[i]),
				     y2 = h - norm(data[i-1]);
				if(abs(y2 - y1) > 1){
					bmp.drawLine(Point(i*STEP, y1), Point(i*STEP, y2),
						chart_colour);
				}
			}
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
