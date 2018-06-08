#include <fstream>

#include "config.h"

namespace sim{

void Config::readFromFile(const std::string &filename){
	std::ifstream reader(filename, std::ios::binary);
	if(!reader){
		Output::printError("Failed to open and process file '{}'.", filename);
		throw FileFormat::FileReadException();
	}

	std::stringstream err_fname_ss;
	err_fname_ss << "Failed to process file '" << filename << "': ";
	std::string err1 = err_fname_ss.str();

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
			
			token.pop_back();  // remove ':'
			std::string current_variable = token;

			if(token == "static_decay")
				tokenizer >> static_decay;
			else if(token == "max")
				tokenizer >> max;
			else if(token == "min")
				tokenizer >> min;
			else if(token == "neighbourhood"){
				tokenizer >> token;
				if(token == "von_neumann")
					neighbourhood = VON_NEUMANN;
				else if(token == "moore")
					neighbourhood = MOORE;
				else{
					Output::printError(err1 + "Invalid neighbourhood type: '{}'.", token);
					throw FileFormat::FileReadException();
				}
			}
			else if(token == "dynamic_usable_decay")
				tokenizer >> dynamic_usable_decay;
			else if(token == "dynamic_usable_max")
				tokenizer >> dynamic_usable_max;
			else if(token == "dynamic_step")
				tokenizer >> dynamic_step;
			else if(token == "dynamic_decay")
				tokenizer >> dynamic_decay;
			else if(token == "diffusion")
				tokenizer >> diffusion;
			else{
				Output::printError(err1 + "Unknown token: '{}'.", token);
				throw FileFormat::FileReadException();
			}

			if(!tokenizer){
				Output::printError(err1 + "Invalid value of '{}'.", token);
				throw FileFormat::FileReadException();
			}
		}
	}

	reader.close();
}

//************************************************************

void Config::writeToFile(const std::string &filename) const {
	std::ofstream writer(filename);  //no ios::binary
	if(!writer){
		Output::printError("Failed to save a snapshot to '{}'.", filename);
		throw FileFormat::FileWriteException();
	}

	writer << "static_decay: " << static_decay << '\n'
	       << "max: " << max << '\n'
	       << "min: " << min << "\n\n"

	       << "neighbourhood: " << (neighbourhood == MOORE ? "moore" : "von_neumann") << "\n\n"

	       << "dynamic_usable_decay: " << dynamic_usable_decay << '\n'
		   << "dynamic_usable_max: " << dynamic_usable_max << '\n'

	       << "dynamic_step: " << dynamic_step << '\n'
	       << "dynamic_decay: " << dynamic_decay << '\n'
	       << "diffusion: " << diffusion << "\n\n";

	writer.close();
}

}