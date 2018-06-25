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
			else if(token == "static_strength")
				tokenizer >> static_strength;
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
			else if(token == "dynamic_strength")
				tokenizer >> dynamic_strength;
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

	// Verify constants.
	if(static_decay <= 0.0)
		Output::printWarning("Nonpositive 'static_decay'.");
	if(max <= min || min <= 0.0)
		Output::printWarning("'max' should be greater than 'min' and both should be positive.");
	if(static_strength <= 0.0)
		Output::printWarning("Nonpositive 'static_strength'.");
	if(dynamic_usable_decay <= 0.0)
		Output::printWarning("Nonpositive 'dynamic_usable_decay'.");
	if(dynamic_usable_max <= 0.0)
		Output::printWarning("Nonpositive 'dynamic_usable_max'.");
	if(dynamic_step <= 0.0)
		Output::printWarning("Nonpositive 'dynamic_step'.");
	if(dynamic_decay <= 0.0 || dynamic_decay >= 100.0)
		Output::printWarning("'dynamic_decay' should be in range from 0 to 100.");
	if(diffusion <= 0.0 || diffusion >= 100.0)
		Output::printWarning("'diffusion' should be in range from 0 to 100.");
	if(dynamic_strength <= 0.0)
		Output::printWarning("Nonpositive 'dynamic_strength'.");

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
	       << "min: " << min << '\n'
		   << "static_strength: " << static_strength << "\n\n"

	       << "neighbourhood: " << (neighbourhood == MOORE ? "moore" : "von_neumann") << "\n\n"

	       << "dynamic_usable_decay: " << dynamic_usable_decay << '\n'
		   << "dynamic_usable_max: " << dynamic_usable_max << '\n'

	       << "dynamic_step: " << dynamic_step << '\n'
	       << "dynamic_decay: " << dynamic_decay << '\n'
	       << "diffusion: " << diffusion << '\n'
		   << "dynamic_strength: " << dynamic_strength << "\n\n";

	writer.close();
}

}