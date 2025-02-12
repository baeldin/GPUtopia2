#include "cl_handler.h"

std::string readCLFragmentFromFile(const std::string& clFileName)
{
	std::ifstream clFragmentFile;
	// std::cout << "Opening " << clFileName << std::endl;
	clFragmentFile.open(clFileName);
	std::stringstream clFragmentSS;
	clFragmentSS << clFragmentFile.rdbuf();
	std::string clFragmentStr = clFragmentSS.str();
	// std::cout << "Read:\n" << clFragmentStr << std::endl;
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
const std::string fun("__functions:\n");
const std::string eof("EOF");

const std::string DPFlag("//@__DOUBLE");
const std::string DPString("#define USE_DOUBLE");
const std::string FLFlag("//@__MODE");
const std::string FLString("#define FLAME");

const std::string AAFlag("//@__AA");
const std::string COFlag("//@__COLORING");
const std::string CPFlag("//@__COMPLEX");
const std::string FFFlag("//@__formulaFunctions");
const std::string CFFlag("//@__coloringFunctions");

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
		fractalParameterLine = std::regex_replace(fractalParameterLine, std::regex("float"), std::string("real"));
		fractalParameterLine = std::regex_replace(fractalParameterLine, std::regex("double"), std::string("real"));
		std::string inParameterName = getFragmentPart(fractalParameterLine, parType, eq);
		std::string inParameterValue = getFragmentPart(fractalParameterLine, eq, semiColon);
		trimTrailingCharacters(inParameterName);
		std::string oldParameterName = "@" + inParameterName;
		std::string newParameterName = parType + inParameterName;
		trimTrailingCharacters(fractalParameterLine, ";");
		kernelParameterBlock += fractalParameterLine + ",\n";
		// std::cout << fractalParameterLine << "\n";
		// std::cout << "Replacing " << oldParameterName << " with " << newParameterName << std::endl;
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
		int p;
		sscanf_s(value.c_str(), "%d", &p);
		m.integerParameters[name] = std::make_pair(p, index);
	}
	else if (type == "real" || type == "double" || type == "float")
	{
		double p = std::stod(value);
		// sscanf_s(value.c_str(), "%f", &p);
		std::pair val = std::make_pair(p, index);
		m.realParameters[name] = val;
	}
	else if (type == "bool")
	{
		// std::cout << "Storing " << name << " for pos " << index << " as " << type << " with value: " << value << std::endl;
		bool p = std::stoi(value.c_str());
		std::pair val = std::make_pair(p, index);
		m.boolParameters[name] = val;
	}
	else if (type == "complex")
	{
		// std::cout << "Storing " << name << " for pos " << index << " as " << type << " with value: " << value << std::endl;
		Complex<double> p;
		char comma;
		std::istringstream iss(value);
		if (iss >> p.x >> comma >> p.y)
			m.complexParameters[name] = std::make_pair(p, index);
		else
			std::cout << "Error parsing complex argument.";
	}
}

// overload for enumParameter type
void addParam(parameterMaps& m, const std::string name, const std::string type,
	const enumParameter& ep, int index)
{
	m.enumParameters[name] = std::make_pair(ep, index);
}

std::vector<std::string> splitLines(const std::string& text) 
{
	std::vector<std::string> lines;
	std::istringstream iss(text);
	std::string line;
	while (std::getline(iss, line)) {
		if (!line.empty())
			lines.push_back(line);
	}
	return lines;
}

std::vector<std::string> splitWords(const std::string& line)
{
    std::vector<std::string> words;
    std::istringstream iss(line);
    std::string word;
    while (iss >> word)
    {
        trimTrailingCharacters(word, ",");
        words.push_back(word);
    }
    return words;
}

paramCollector parseKernelParameterBlock(std::string& kpb)
{
	std::string kpb_noValues;
	paramCollector pc;
	std::string paramType;
	int paramIndex = 0; // first non-default param
	std::string paramName;
	std::vector<std::string> kpbLines = splitLines(kpb);
	int N = 0;
	while (N < kpbLines.size()) 
	{
		std::vector<std::string> words = splitWords(kpbLines[N]);
		parameterMaps* paramMaps = (words[2][0] == *"f") ? &pc.fractalParameterMaps : &pc.coloringParameterMaps;
		paramName = std::regex_replace(words[2], std::regex("[fc]Par_"), "");
		if (words[1] == "int")
		{
			int val = std::stoi(words[4]);
			paramMaps->integerParameters[paramName] = std::make_pair(val, paramIndex);
			kpb_noValues += std::string("    const int ") + words[2] + std::string(",\n");
		}
		if (words[1] == "float" || words[1] == "double" || words[1] == "real")
		{
			double val = std::stod(words[4]);
			paramMaps->realParameters[paramName] = std::make_pair(val, paramIndex);
			kpb_noValues += std::string("    const real ") + words[2] + std::string(",\n");
		}
		if (words[1] == "complex")
		{
			double x = std::stod(std::regex_replace(words[4], std::regex("\\("), ""));
			double y = std::stod(std::regex_replace(words[5], std::regex("\\)"), ""));
			Complex<double> val(x, y);
			paramMaps->complexParameters[paramName] = std::make_pair(val, paramIndex);
			kpb_noValues += std::string("    const complex ") + words[2] + std::string(",\n");
		}
		if (words[1] == "enum")
		{
			int val = std::stoi(words[4]);
			enumParameter ep;
			ep.value = val;
			ep.labels = splitWords(kpbLines[N + 1]);
			ep.labels.erase(ep.labels.begin());
			paramMaps->enumParameters[paramName] = std::make_pair(ep, paramIndex);
			kpb_noValues += std::string("    const int ") + words[2] + std::string(",\n");
			N++;
		}
		N++;
		paramIndex++;
	}

	trimTrailingCharacters(kpb_noValues, ",\n");
	kpb = kpb_noValues;
	return pc;
}

paramCollector parseParameters(std::string& fullStr, const std::string& fractalFormula, const std::string& coloringAlgorithm)
{
	std::string formulaParamStr = getFragmentPart(fractalFormula, par, ini);
	std::string coloringParamStr = getFragmentPart(coloringAlgorithm, par, ini);
	std::istringstream sf(formulaParamStr.c_str());
	std::istringstream sc(coloringParamStr.c_str());
	std::string parameterLine;
	std::string kernelParameterBlock = fillKernelParamBlock(sf, fullStr, fPar);
	kernelParameterBlock += fillKernelParamBlock(sc, fullStr, cPar);
	trimTrailingCharacters(kernelParameterBlock, ",\n");
	paramCollector pc = parseKernelParameterBlock(kernelParameterBlock);
	fullStr = std::regex_replace(fullStr, std::regex(kernelArgFlag), kernelParameterBlock);
	return pc;
}

bool clFractal::makeCLCode(const bool newFiles)
{
	std::string full_template("clFragments/full_template.cl");
	std::string fullTemplateStr = readCLFragmentFromFile(full_template);
	std::string fractalFile = this->fractalCLFragmentFile;
	if (!this->pathOK(this->fractalCLFragmentFile, std::string("fractalFormulas")))
		return false;
	if (!this->pathOK(this->coloringCLFragmentFile, std::string("coloringAlgorithms")))
		return false;
	std::string fractalFormulaStr = readCLFragmentFromFile(fractalCLFragmentFile) + eof;
	std::string coloringAlgorithmStr = readCLFragmentFromFile(coloringCLFragmentFile) + eof;
	std::string antiAliasing("clFragments/full_template.cl");
	std::string antiAliasingStr = readCLFragmentFromFile("clFragments/anti_aliasing.cl");
	std::string colorStr = readCLFragmentFromFile("clFragments/colors.cl");
	std::string complexStr = readCLFragmentFromFile("clFragments/complex.cl");

	// UF-style separation
	std::string fractalInit = getFragmentPart(fractalFormulaStr, ini, loo);
	std::string fractalLoop = getFragmentPart(fractalFormulaStr, loo, bai);
	std::string fractalBailout = getFragmentPart(fractalFormulaStr, bai, fun);
	std::string fractalFunctions = getFragmentPart(fractalFormulaStr, fun, eof);
	std::string coloringInit = getFragmentPart(coloringAlgorithmStr, ini, loo);
	std::string coloringLoop = getFragmentPart(coloringAlgorithmStr, loo, fin);
	std::string coloringFinal = getFragmentPart(coloringAlgorithmStr, fin, fun);
	std::string coloringFunctions = getFragmentPart(coloringAlgorithmStr, fun, eof);

	// std::cout << "BEFORE:\n" << fullTemplateStr << std::endl;
	if (this->useDouble)
		fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(DPFlag), DPString);
	if (this->mode == 1)
		fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FLFlag), FLString);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(AAFlag), antiAliasingStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(COFlag), colorStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(CPFlag), complexStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forIniFlag), fractalInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forLooFlag), fractalLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forBaiFlag), fractalBailout);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FFFlag), fractalFunctions);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colIniFlag), coloringInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colLooFlag), coloringLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colFinFlag), coloringFinal);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(CFFlag), coloringFunctions);

	paramCollector pc = parseParameters(fullTemplateStr, fractalFormulaStr, coloringAlgorithmStr);

	this->fullCLcode = fullTemplateStr;
	if (newFiles)
		this->params = pc;
	return true;
}