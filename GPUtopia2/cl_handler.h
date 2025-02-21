#pragma once

#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <map>
#include <CL/opencl.h>

#include "cl_fractal.h"

typedef std::vector<std::pair<std::string, std::string>> replacer;

// read a text file into a string
std::string readCLFragmentFromFile(
	const std::string& clFileName);

// get substring delimited by two substrings
std::string getFragmentPart(
	const std::string& fullStr, 
	const std::string& seachStart, 
	const std::string& searchEnd);

// trim whitespace at the end of string
void trimTrailingCharacters(
	std::string& str, 
	const char* trim);

//void addParam(
//	parameterMaps& m, 
//	const std::string& name, 
//	const std::string& type,
//	const std::string& value);
void addParam(parameterMaps& m, const std::string name, const std::string type,
	enumParameter& ep, int index);

std::vector<std::string> splitLines(const std::string& text);
paramCollector parseKernelParameterBlock(std::string& kpb);

std::string fillKernelParamBlock(
	std::istringstream& paramList, 
	std::string& fullStr,
	const std::string& parType);

// detect and name all parameters
// add parameters as kernel arguments
// figure out how to auto-hand the respective parameters to the kernel???
paramCollector parseParameters(std::string& fullStr, const std::string& fractalFormula,
	const std::string& coloringInsideAlgorithm, const std::string& coloringOutsideAlgorithm);
