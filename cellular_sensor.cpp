#include <fstream>
#include "cellular_sensor.h"

namespace sim{

//************************************************************

void MotionSensor::update(){
	const auto &pmap = getSnapshot().pedestrians;

	// Find pedestrian that is currently occupying the cell.
	auto it = std::find_if(
		pmap.begin(), pmap.end(),
		[this](const std::pair<uid_t, Pedestrian> &p){
			return p.second.getPosition() == this->position;
		}
	);

	if(it == pmap.end()){  //the cell is empty
		if(previous_pedestrian != pmap.end()){  //the cell was not empty
			departures.push_back(std::make_pair(getTime(), previous_pedestrian->first));
			previous_pedestrian = pmap.end();
		}
	}
	else{  //the cell is not empty
		if(it != previous_pedestrian){
			if(previous_pedestrian != pmap.end())  //the cell was not empty
				departures.push_back(std::make_pair(getTime(), previous_pedestrian->first));
			
			arrivals.push_back(std::make_pair(getTime(), it->first));
			previous_pedestrian = it;
		}
		// else: pedestrian hasn't moved. Nothing to report.
	}
}

void MotionSensor::saveToFile(const std::string &filename) const {
	std::ofstream writer(filename /*, std::ios::binary*/);

	writer << "Motion sensor at cell (" << position.x << ", " << position.y << ")\n\n";

	auto i = arrivals.begin(),
	     j = departures.begin();

	// Write all logs to file - chronologically.		
	while(i != arrivals.end() && j != departures.end()){
		if(i->first < j->first){
			writer << '[' << i->first << "]: arrival of #" << i->second << '\n';
			++i;
		}
		else{
			writer << '[' << j->first << "]: departure of #" << j->second << '\n';
			++j;
		}
	}

	// Write the remaining logs.
	while(i != arrivals.end()){
		writer << '[' << i->first << "]: arrival of #" << i->second << '\n';
		++i;
	}
	while(j != departures.end()){
		writer << '[' << j->first << "]: departure of #" << j->second << '\n';
		++j;
	}
	writer.close();
}

//************************************************************

void DynamicSensor::update(){
	data.push_back(CellularSensor::getSnapshot().dynamic_field(position.x, position.y));
}

void DynamicSensor::saveToFile(const std::string &filename) const {
	const int STEP = 5;
	int w = data.size() * STEP;
	int h = 100;
	JiMP2::BMP bmp(w, h);
	auto max_n = *(std::max_element(data.begin(), data.end()));

	// Linear normalization.
	auto norm = [&h, &max_n](std::size_t n){
		return static_cast<int>(h * (fp_t)(n) / max_n);
	};

	drawGrid(bmp, w, h);

	for(std::size_t i = 0; i != data.size(); ++i){
		int y = h - norm(data[i]) + 1;
		bmp.drawLine(Point(i*STEP, y), Point((i+1)*STEP, y), chart_colour);
		if(i > 0){
			// Vertical line, if the gap between horizontal
			// lines is too large.
			auto y1 = h - norm(data[i]) + 1,
			     y2 = h - norm(data[i-1]) + 1;
			if(abs(y2 - y1) > 1){
				bmp.drawLine(Point(i*STEP, y1), Point(i*STEP, y2),
					chart_colour);
			}
		}
	}

	saveBitmap(bmp, filename);
}

//************************************************************

}