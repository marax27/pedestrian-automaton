#include <fstream>

#include "snapshot.h"

namespace sim{

void Snapshot::readFromFile(const std::string &filename){
	std::ifstream reader(filename, std::ios::binary);
	if(!reader){
		Output::printError("Failed to open and process file '{}'.", filename);
		throw FileReadException();
	}

	index_t dim = 0;
	std::vector<std::string> map;

	while(reader.good()){
		std::string line;
		std::getline(reader, line);

		// Ignore empty line.
		if(!line.size())
			continue;

		// CLRF correction.
		if(line[line.size()-1] == '\r')
			line.erase(line.size()-1);

		std::string token;
		std::istringstream tokenizer{line};
		while(true){
			tokenizer >> token;
			if(!tokenizer)  //end of line
				break;
			if(token[0] == '#')  //beginning of comment
				break;
			
			if(token == "dimension:"){
				tokenizer >> dim;
				if(!tokenizer){
					Output::printError("Failed to process file '{}': could not have read map dimension.", filename);
					throw FileReadException();
				}
			}
			else if(token == "map:"){
				if(!dim){
					Output::printError("Failed to process file '{}': Dimension is 0 or not provided.", filename);
					throw FileReadException();
				}
				// Read a number of tokens - each one represents a row of a map.
				for(index_t i = 0; i != dim; ++i){
					reader >> token;
					if(reader.fail()){
						Output::printError("Failed to process file '{}': Failed to read {}. row of a map ({} expected).", filename, i+1, dim);
						throw FileReadException();
					}
					if(token.size() != dim){
						Output::printError("Failed to process file '{}': Row {} is of invalid length ({} expected).", filename, i+1, dim);
						throw FileReadException();
					}
					map.push_back(token);
				}
			}
		}
	}

	this->dimension = dim;

	for(index_t r = 0; r != dimension; ++r){
		for(index_t c = 0; c != dimension; ++c){
			vec2 pos(c, r);
			switch(tolower(map[r][c])){
			case 'p':
				this->pedestrians.push_back(Pedestrian(pos));
				break;
			case 'w':
				this->walls.push_back(pos);
				break;
			case 'x':
				this->exits.push_back(pos);
				break;
			case '.':
				break;
			default:
				Output::printError("Failed to process file '{}': Unknown character: '{}'", filename, map[r][c]);
			}
		}
	}

	this->dynamic_field = Field<fp_t>(this->dimension);
	for(index_t i = 0; i != dimension; ++i)
		for(index_t j = 0; j != dimension; ++j)
			dynamic_field(j, i) = 0.0f;

}

//************************************************************

void Snapshot::writeToFile(const std::string &filename) const {
	std::ofstream writer(filename);  //no ios::binary
	if(!writer){
		Output::printError("Failed to save a snapshot to '{}'.", filename);
		throw FileWriteException();
	}

	// Prepare text-mode map.
	std::vector<std::string> rows;
	rows.resize(dimension);
	for(auto &r : rows)
		r.resize(dimension, '.');
	for(const auto &p : pedestrians){
		auto pos = p.getPosition();
		rows[pos.y][pos.x] = 'p';
	}
	for(const auto &w : walls){
		rows[w.y][w.x] = 'W';
	}
	for(const auto &x : exits){
		rows[x.y][x.x] = 'X';
	}

	writer << "dimension: " << dimension << '\n';
	writer << "map:\n";
	for(const auto &r : rows)
		writer << r << '\n';
}


}