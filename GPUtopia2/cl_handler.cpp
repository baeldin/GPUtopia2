#include "cl_handler.h"

std::string readCLFragmentFromFile(const std::string& clFileName)
{
	std::ifstream clFragmentFile;
	std::cout << "Opening " << clFileName << std::endl;
	clFragmentFile.open(clFileName);
	std::stringstream clFragmentSS;
	clFragmentSS << clFragmentFile.rdbuf();
	std::string clFragmentStr = clFragmentSS.str();
	std::cout << "Read:\n" << clFragmentStr << std::endl;
	return clFragmentStr;
}

std::string getFragmentPart(const std::string& fullStr, const std::string& searchStart, const std::string& searchEnd)
{
	unsigned startPatternLength= searchStart.length();
	unsigned startPos = fullStr.find(searchStart) + startPatternLength;
	// std::cout << "startPos for \"" << searchStart << "\": " << startPos << std::endl;

	unsigned endPatternLength = searchEnd.length();
	unsigned endPos = fullStr.find(searchEnd); // -endPatternLength;
	// std::cout << "endPos for \"" << searchEnd << "\": " << endPos << std::endl;
	return fullStr.substr(startPos, endPos - startPos);
}

const std::string par("__parameters:\n");
const std::string ini("__init:\n");
const std::string loo("__loop:\n");
const std::string bai("__bailout:\n");
const std::string fin("__final:\n");
const std::string eof("EOF");

const std::string AAFlag("//@__AA");
const std::string COFlag("//@__COLORING");
const std::string forIniFlag("//@__formulaInit");
const std::string forLooFlag("//@__formulaLoop");
const std::string forBaiFlag("//@__bailout");
const std::string colIniFlag("//@__coloringInit");
const std::string colLooFlag("//@__coloringLoop");
const std::string colFinFlag("//@__coloringFinal");
const std::string kernelArgFlag("//@__kernelArguments");

const std::string semiColon(";");
const std::string eq("=");
const std::string fPar("fPar_");
const std::string cPar("cPar_");


void trimTrailingCharacters(std::string& str, const char* trim = " ")
{
	str.erase(str.find_last_not_of(trim) + 1);
}

std::string fillKernelParamBlock(std::istringstream& paramList, std::string& fullStr, const std::string& parType)
{
	std::string kernelParamList("");
	std::string parameterLine;
	std::string kernelParameterBlock("");

	while (std::getline(paramList, parameterLine))
	{
		std::string fractalParameterLine = "    const " + std::regex_replace(
			parameterLine, std::regex("parameter "), parType);
		std::string inParameterName = getFragmentPart(fractalParameterLine, parType, eq);
		std::string inParameterValue = getFragmentPart(fractalParameterLine, eq, semiColon);
		trimTrailingCharacters(inParameterName);
		std::string oldParameterName = "@" + inParameterName;
		std::string newParameterName = parType + inParameterName;
		trimTrailingCharacters(fractalParameterLine, ";");
		kernelParameterBlock += fractalParameterLine + ",\n";
		std::cout << fractalParameterLine << "\n";
		std::cout << "Replacing " << oldParameterName << " with " << newParameterName << std::endl;
		fullStr = std::regex_replace(fullStr, std::regex(oldParameterName), newParameterName);

	}
	return kernelParameterBlock;
}

const std::string constStr("const");
const std::string floatStr("float");
const std::string intStr("int");

void addParam(parameterMaps& m,	const std::string name, const std::string type, 
	const std::string value, int index)
{
	if (type == "int")
	{ 
		std::cout << "Storing " << name << " for pos " << index << " as " << type << " with value: " << value << std::endl;
		int p;
		sscanf_s(value.c_str(), "%d", &p);
		m.integerParameters[name] = std::make_pair(p, index);
		// m.integerParameters.insert({ name.c_str(), std::make_pair(p, index) });
	}
	else if (type == "float")
	{
		std::cout << "Storing " << name << " for pos " << index << " as " << type << " with value: " << value << std::endl;
		float p;
		sscanf_s(value.c_str(), "%f", &p);
		std::pair val = std::make_pair(p, index);
		m.floatParameters[name] = val;
		// m.floatParameters.insert({ cname, std::make_pair(p, index) });
	}
}

paramCollector parseKernelParameterBlock(std::string& kpb)
{
	std::istringstream kernelParamBlockStream(kpb.c_str());
	std::string kernelParamLine;
	// std::cout << "========= KERNEL PARAM VOMIT ===========\n";
	unsigned word_counter = 0;
	std::string kpb_noValues;
	paramCollector pc;
	std::string paramType;
	int paramAffiliaton = 0; // 0 for fractal, 1 for coloring
	// TODO: somehow get this from the cl core or smth?
	int paramIndex = 13; // first non-default param
	std::string paramName;
	std::string paramValue;
	while (std::getline(kernelParamBlockStream, kernelParamLine))
	{

		std::istringstream iss(kernelParamLine);
		do {
			std::string word;
			iss >> word;
			if (word_counter == 1)
			{
				paramType = word;
				kpb_noValues += word + " ";
			}
			if (word_counter == 2)
			{
				paramName = word;
				kpb_noValues += word + ",\n";
				paramAffiliaton = (word[0] == *"f") ? 0 : 1;
				paramName = std::regex_replace(paramName, std::regex("[fc]Par_"), "");
			}
			if (word_counter == 4)
			{
				paramValue = word;
				trimTrailingCharacters(paramValue, ","); // remove comma behind value
				if (paramAffiliaton == 0)
					addParam(pc.fractalParameterMaps, paramName, paramType, paramValue, paramIndex);
				else
					addParam(pc.coloringParameterMaps, paramName, paramType, paramValue, paramIndex);
				paramIndex++;
			}
			if (word == constStr) { 
				word_counter = 0; 
				kpb_noValues += "    const ";
			} // reset word counter
			//std::cout << word << std::endl;
			word_counter++;
		} while (iss);
		//std::cout << paramName << " is of type " << paramType << " and has value " << paramValue << " and index " << paramIndex << std::endl;
	}
	// std::cout << "========= END KERNEL PARAM VOMIT =======\n";
	trimTrailingCharacters(kpb_noValues, ",\n");
	// kpb_noValues += ")";
	kpb = kpb_noValues;
	std::cout << "========= DUMP MAP CONTENT =============\n";
	for (auto const& [key, val] : pc.coloringParameterMaps.floatParameters) {
		std::cout << key << ": " << val.first << ", " << val.second << std::endl;
	}
	std::cout << "Map has size " << pc.coloringParameterMaps.floatParameters.size() << std::endl;
	return pc;
}

paramCollector parseParameters(std::string& fullStr, const std::string& fractalFormula, const std::string& coloringAlgorithm)
{
	std::string formulaParamStr = getFragmentPart(fractalFormula, par, ini);
	std::string coloringParamStr = getFragmentPart(coloringAlgorithm, par, ini);
	std::cout << coloringParamStr << std::endl;
	std::istringstream sf(formulaParamStr.c_str());
	std::istringstream sc(coloringParamStr.c_str());
	std::cout << "======================= RAW PARAMETER BLOCK ====================\n";
	std::cout << formulaParamStr << std::endl;
	std::cout << coloringParamStr << std::endl;
	std::cout << "======================= END RAW PARAMETER BLOCK ================\n";	
	std::string parameterLine;
	std::string kernelParameterBlock = fillKernelParamBlock(sf, fullStr, fPar);
	kernelParameterBlock += fillKernelParamBlock(sc, fullStr, cPar);
	trimTrailingCharacters(kernelParameterBlock, ",\n");
	std::cout << "======================= KERNEL PARAMETER BLOCK =================\n";
	std::cout << kernelParameterBlock << std::endl;
	std::cout << "======================= END KERNEL PARAMETER BLOCK =============\n";
	paramCollector pc = parseKernelParameterBlock(kernelParameterBlock);
	fullStr = std::regex_replace(fullStr, std::regex(kernelArgFlag), kernelParameterBlock);
	return pc;
}

void clFractal::makeCLCode()
{
	std::string full_template("clFragments/full_template.cl");
	std::string fullTemplateStr = readCLFragmentFromFile(full_template);
	//std::string fractalFile("clFragments/fractalFormulas/gnarl.cl");
	std::string fractalFile(fractalCLFragmentFile);
	//std::string coloringFile("clFragments/coloringAlgorithms/dist.cl");
	std::string coloringFile(coloringCLFragmentFile);
	std::string fractalFormulaStr = readCLFragmentFromFile(fractalCLFragmentFile) + eof;
	std::string coloringAlgorithmStr = readCLFragmentFromFile(coloringCLFragmentFile) + eof;
	std::string antiAliasing("clFragments/full_template.cl");
	std::string antiAliasingStr = readCLFragmentFromFile("clFragments/anti_aliasing.cl");
	std::string colorStr = readCLFragmentFromFile("clFragments/colors.cl");

	std::cout << fullTemplateStr << std::endl;
	std::cout << fractalFormulaStr << std::endl;
	std::cout << coloringAlgorithmStr << std::endl;
	
	// get init, loop, bailout, and final fragments
	// UF-style separation
	std::string fractalInit = getFragmentPart(fractalFormulaStr, ini, loo);
	std::string fractalLoop = getFragmentPart(fractalFormulaStr, loo, bai);
	std::string fractalBailout = getFragmentPart(fractalFormulaStr, bai, eof);
	std::string coloringInit = getFragmentPart(coloringAlgorithmStr, ini, loo);
	std::string coloringLoop = getFragmentPart(coloringAlgorithmStr, loo, fin);
	std::string coloringFinal = getFragmentPart(coloringAlgorithmStr, fin, eof);
	
	std::cout << "BEFORE:\n" << fullTemplateStr << std::endl;
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(AAFlag), antiAliasingStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(COFlag), colorStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forIniFlag), fractalInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forLooFlag), fractalLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forBaiFlag), fractalBailout);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colIniFlag), coloringInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colLooFlag), coloringLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colFinFlag), coloringFinal);

	paramCollector pc = parseParameters(fullTemplateStr, fractalFormulaStr, coloringAlgorithmStr);
	std::cout << "AFTER:\n" << fullTemplateStr << std::endl;

	clFractal cf;
	this->fullCLcode = fullTemplateStr;
	this->params = pc;
	
}