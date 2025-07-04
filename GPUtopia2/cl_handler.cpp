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
const std::string CoFlag("//@__COLORING");
const std::string CPFlag("//@__COMPLEX");
const std::string FFFlag("//@__formulaFunctions");
const std::string FOFlag("//@__coloringOutsideFunctions");
const std::string FIFlag("//@__coloringInsideFunctions");

const std::string forIniFlag("//@__formulaInit");
const std::string forLooFlag("//@__formulaLoop");
const std::string forBaiFlag("//@__bailout");
const std::string colInOFlag("//@__coloringOutsideInit");
const std::string colLoOFlag("//@__coloringOutsideLoop");
const std::string colFiOFlag("//@__coloringOutsideFinal");
const std::string colInIFlag("//@__coloringInsideInit");
const std::string colLoIFlag("//@__coloringInsideLoop");
const std::string colFiIFlag("//@__coloringInsideFinal");
const std::string kernelArgFlag("//@__kernelArguments");

const std::string semiColon(";");
const std::string eq("=");
const std::string fPar("fPar_");
const std::string coPar("coPar_");
const std::string ciPar("ciPar_");
const std::string fVar("fVar_");
const std::string coVar("coVar_");
const std::string ciVar("ciVar_");
const std::string fFunc("fFunc_");
const std::string coFunc("coFunc_");
const std::string ciFunc("ciFunc_");
const std::vector<std::string> fPrefixes = { "fPar_", "fVar_", "fFunc_" };
const std::vector<std::string> ciPrefixes = { "ciPar_", "ciVar_", "ciFunc_" };
const std::vector<std::string> coPrefixes = { "coPar_", "coVar_", "coFunc_" };

const std::vector<std::string> protectedStrings = { "f" };

void trimTrailingCharacters(std::string& str, const char* trim = " ")
{
	str.erase(str.find_last_not_of(trim) + 1);
}

void trimLeadingCharacters(std::string& str, const char* trim = " ")
{
	str.erase(0, str.find_first_not_of(trim));
}

void splitAtPattern(const std::string& s, const std::string& pattern, std::string& A, std::string& B) {
	size_t pos = s.find(pattern);
	if (pos == std::string::npos) {
		A = s; 
		B = "";
	}
	else
	{
		A = s.substr(0, pos);
		B = s.substr(pos + pattern.length());
	}
}

bool isProtected(std::string& s)
{
	return std::find(protectedStrings.begin(), protectedStrings.end(), s) != protectedStrings.end();
}

// do some basic checks to see if it COULD be a legit CL fragment
// fragmentType = 0, 1, 2 for fractal, inside coloring, outside coloring
int clFragmentSanityCheck(std::string& fragmentStr, int fragmentType)
{
	if (!std::regex_search(fragmentStr, std::regex(par))) // check for __parameters:
		return 1;
	if (!std::regex_search(fragmentStr, std::regex(ini))) // check for __init:
		return 2;
	if (!std::regex_search(fragmentStr, std::regex(loo))) // check for __loop:
		return 3;
	// TODO: make this one optional if no functions are being used
	if (!std::regex_search(fragmentStr, std::regex(fun))) // check for __functions:
		return 4;
	if (fragmentType == 0)
		if (!std::regex_search(fragmentStr, std::regex(bai))) // check for __bailout, if fractal formula:
			return 5;
	if (fragmentType > 0)
		if (!std::regex_search(fragmentStr, std::regex(fin))) // check for __final, if coloring algorithm:
			return 6;
	return 0;
}


std::string fillKernelParamBlock(std::istringstream& paramList, std::string& fullStr, const std::string& parType)
{
	std::string kernelParamList("");
	std::string parameterLine;
	std::string kernelParameterBlock("");
	while (std::getline(paramList, parameterLine))
	{
		std::string fractalParameterLine = "    const " + std::regex_replace(
			parameterLine, std::regex("parameter "), "");
		// std::string fractalParameterLine = "    const " + parameterLine;
		fractalParameterLine = std::regex_replace(fractalParameterLine, std::regex("float"), std::string("real"));
		fractalParameterLine = std::regex_replace(fractalParameterLine, std::regex("double"), std::string("real"));
		std::string inParameterName = getFragmentPart(fractalParameterLine, parType, eq);
		// std::string inParameterValue = getFragmentPart(fractalParameterLine, eq, semiColon);
		trimTrailingCharacters(inParameterName);
		//std::string oldParameterName = "@" + inParameterName;
		//std::string newParameterName = parType + inParameterName;
		trimTrailingCharacters(fractalParameterLine, ";");
		kernelParameterBlock += fractalParameterLine + ",\n";
		// std::cout << fractalParameterLine << "\n";
		// std::cout << "Replacing " << oldParameterName << " with " << newParameterName << std::endl;
		//fullStr = std::regex_replace(fullStr, std::regex(oldParameterName), newParameterName);

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
	enumParameter& ep, int index)
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

std::vector<std::string> splitWords(const std::string& line, const char delimiter = ' ', const char* trim = ",")
{
    std::vector<std::string> words;
    std::istringstream iss(line);
    std::string word;
    while (std::getline(iss, word, delimiter))
    {
        trimTrailingCharacters(word, trim);
		if (!word.empty())
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
	// std::cout << kpb << "\n";
	std::vector<std::string> kpbLines = splitLines(kpb);
	int N = 0;
	while (N < kpbLines.size()) 
	{
		std::vector<std::string> words = splitWords(kpbLines[N]);
		parameterMaps* paramMaps = &pc.fractalParameterMaps;
		if (std::regex_search(words[2], std::regex("^co")))
			paramMaps = &pc.outsideColoringParameterMaps;
		if (std::regex_search(words[2], std::regex("^ci")))
			paramMaps = &pc.insideColoringParameterMaps;
		paramName = std::regex_replace(words[2], std::regex("(?:f[PV]ar_|ci[PV]ar_|co[PV]ar_)"), "");
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
			double y = 0.;
			if (words.size() == 6)
				y = std::stod(std::regex_replace(words[5], std::regex("\\)"), ""));
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

bool isStringInVector(const std::vector<std::string>& vec, const std::string& str) {
	// Use std::find to search for the string
	return std::find(vec.begin(), vec.end(), str) != vec.end();
}

// scan a string and remove everything within {...} or (...) by default
// optional arguments to turn off clearing of one kind of brackets
int parenthesesScanner(std::string& s, const bool clearRound = true, const bool clearCurly = true)
{
	std::vector<std::pair<int, int>> p;
	std::vector<int> counter = { 0, 0, 0 };
	unsigned N = 0;
	int roundCount = 0;
	int curlyCount = 0;
	int globalCount = 0;
	for (auto c : s)
	{
		if (clearRound)
		{
			if (c == '(')
			{
				if (counter[0] == 0)
					roundCount = N;
				counter[0]++;
				globalCount++;
			}
			if (c == ')')
			{
				if (counter[0] == 1)
					p.push_back(std::make_pair(roundCount, N));
				counter[0]--;
			}
		}
		if (clearCurly)
		{
			if (c == '{')
			{
				if (counter[1] == 0)
					curlyCount = N;
				counter[1]++;
				globalCount++;
			}
			if (c == '}')
			{
				if (counter[1] == 1)
					p.push_back(std::make_pair(curlyCount, N));
				counter[1]--;
			}
		}
		N++;
	}
	if (!counter[0] == 0 || !counter[1] == 0)
		return -1; // unbalanced parentheses in declaration
	for (auto del : p | std::views::reverse)
		if (del.second < s.length())
			s.erase(del.first, del.second - del.first + 1);
	return globalCount; // return the total number of parentheses pairs found
}

const std::vector<std::string> typeIndicators = { 
	"^char\d{0,2}\s*", "^unsigned\s+$char\d{0,2}\s*", "^uchar\d{0,2}\s*", "^int\d{0,2}\s*", 
	"^unsigned\s+$int\d{0,2}\s*", "^uint\d{0,2}\s*", "^long\d{0,2}\s*", "^unsigned\s+long\d{0,2}\s*",
	"^ulong\d{0,2}\s*", "^float\d{0,2}\s*", "^double\d{0,2}\s*", "^half\d{0,2}\s*", "^size_t\s*", 
	"^ptrdiff_t\s*", "^intptr_t\s*", "^uintptr_t\s*", "^void\s*", 
	"^real\d{0,2}\s*", "^complex\s*", "^bool\s*", "^enum\s*"};
const std::vector<std::string> qualifiers = {
	"^__global\s*", "^global\s*", "^__local\s*", "^local\s*", "^__constant\s*", "^constant\s*", "^const\s*", "^__private\s*",
	"^private\s*", "^__attribute__\s*$\\(\\(.*?\\)\\)\s*" };
const std::vector<std::string> protectedNames = { "z", "bailedout", "outColor"}; // do not touch these!

std::string removeFunctionBodies(std::string& funcStr, int* err)
{
	*err = parenthesesScanner(funcStr, true, true);
	return funcStr;
}

replacer scanString(std::string codeStr, const std::vector<std::string>& prefixes, const bool functionBlock, int* err)
{
	codeStr = functionBlock ? removeFunctionBodies(codeStr, err) : codeStr;
	std::vector<std::string> codeLines = splitLines(codeStr);
	replacer declarations;
	int declarationType = 0; // Variable
	int N = 0;
	std::string prefix;
	while (N < codeLines.size())
	{
		std::string line = codeLines[N];
		std::cout << "parsing: " << line << "\n";
		if (line == "//@__functions:")
			declarationType = 2;
		if (declarationType == 2 && line.find("=") == std::string::npos)
		{
			std::cout << "  this is not a function definition, use variable\n";
			N++;
			continue;
		}
		trimTrailingCharacters(line, " ;\n");
		trimLeadingCharacters(line, " \t");
		bool isDeclaration = false;
		int lineLength = line.length();
		for (auto pattern : qualifiers)
			line = std::regex_replace(line, std::regex(pattern), "");
		trimLeadingCharacters(line, " ");
		for (auto pattern : typeIndicators)
			std::regex_replace(line, std::regex(pattern), "");
		trimLeadingCharacters(line, " ");
		for (auto pattern2 : typeIndicators)
		{
			line = std::regex_replace(line, std::regex(pattern2), "");
		}
		trimLeadingCharacters(line, " 01234567890*");
		if (line.length() < lineLength)
		{
			std::cout << "  found: " << line << "\n";
			if (isProtected(line))
			{
				std::cout << "  WARNING: string \"" << line << "\" is protected, it will not be renamed. This could cause problems.\n";
				N++;
				continue;
			}
			*err = parenthesesScanner(line);
			if (*err < 0)
			{
				return declarations;
			}
			if (std::regex_search(line, std::regex("^parameter")))
			{
				line = std::regex_replace(line, std::regex("^parameter\s*"), "");
				trimLeadingCharacters(line, " "); // why do I need this???
				prefix = prefixes[0];
			}
			else if (functionBlock && line.find('=') != std::string::npos)
				prefix = prefixes[2];
			else
				prefix = prefixes[1];
			std::vector<std::string> words = splitWords(line, ',', " ");
			for (auto word : words)
			{
				word = std::regex_replace(word, std::regex("\s*=\s*.*"), "");
				trimTrailingCharacters(word, " "); // why do I need this???
				if (!isStringInVector(protectedNames, word))
				{

					//std::string prefix = prefixes[declarationType];
					if (declarationType < 2)
						declarations.push_back(std::make_pair("\\b" + word + "\\b", prefix + word));
				}
			}
		}
		N++;
	}
	return declarations;
}
std::string getVariableAndParameterNames(const std::string& codeStr_,
	const std::vector<std::string>& prefixes, const std::string& funcFlag, int* err = 0)
{
	std::string codeStr, funcStr;
	splitAtPattern(codeStr_, fun, codeStr, funcStr);
	std::vector<std::string> codeLines = splitLines(codeStr);
	replacer rawDeclarations = scanString(codeStr, prefixes, false, err);
	replacer funcDeclarations = scanString(funcStr, prefixes, true, err);
	if (!*err == 0)
		return "";
	for (auto repl : rawDeclarations)
		codeStr = std::regex_replace(codeStr, std::regex(repl.first), repl.second);
	codeStr = std::regex_replace(codeStr, std::regex("@"), "");
	for (auto repl : funcDeclarations)
	{
		funcStr = std::regex_replace(funcStr, std::regex(repl.first), repl.second);
		codeStr = std::regex_replace(codeStr, std::regex(repl.first), repl.second);
	}
	std::string ret = codeStr + fun + funcStr;
	return codeStr + fun + funcStr;
}


paramCollector parseParameters(std::string& fullStr, const std::string& fractalFormula, 
	const std::string& coloringInsideAlgorithm, const std::string& coloringOutsideAlgorithm)
{

	std::string formulaParamStr = getFragmentPart(fractalFormula, par, ini);
	std::string coloringInsideParamStr = getFragmentPart(coloringInsideAlgorithm, par, ini);
	std::string coloringOutsideParamStr = getFragmentPart(coloringOutsideAlgorithm, par, ini);
	std::istringstream sf(formulaParamStr.c_str());
	std::istringstream sci(coloringInsideParamStr.c_str());
	std::istringstream sco(coloringOutsideParamStr.c_str());
	std::string parameterLine;
	std::string kernelParameterBlock = fillKernelParamBlock(sf, fullStr, fPar);
	kernelParameterBlock += fillKernelParamBlock(sci, fullStr, ciPar);
	kernelParameterBlock += fillKernelParamBlock(sco, fullStr, coPar);
	trimTrailingCharacters(kernelParameterBlock, ",\n");
	paramCollector pc = parseKernelParameterBlock(kernelParameterBlock);
	fullStr = std::regex_replace(fullStr, std::regex(kernelArgFlag), kernelParameterBlock);
	return pc;
}

bool clFractal::makeCLCode(const bool newFiles)
{
	this->resetParseStatus();
	std::string full_template("clFragments/full_template.cl");
	std::string fullTemplateStr = readCLFragmentFromFile(full_template);
	std::string fractalFile = this->fractalCLFragmentFile;
	if (!this->pathOK(this->fractalCLFragmentFile, std::string("fractalFormulas")))
		return false;
	if (!this->pathOK(this->insideColoringCLFragmentFile, std::string("coloringInsideAlgorithms")))
		return false;
	if (!this->pathOK(this->outsideColoringCLFragmentFile, std::string("coloringOutsideAlgorithms")))
		return false;
	std::string fractalFormulaStr = readCLFragmentFromFile(fractalCLFragmentFile) + eof;
	std::string insideColoringAlgorithmStr = readCLFragmentFromFile(insideColoringCLFragmentFile) + eof;
	std::string outsideColoringAlgorithmStr = readCLFragmentFromFile(outsideColoringCLFragmentFile) + eof;
	this->status.fractalFragmentStatus = clFragmentSanityCheck(fractalFormulaStr, 0);
	this->status.insideColoringFragmentStatus = clFragmentSanityCheck(insideColoringAlgorithmStr, 1);
	this->status.outsideColoringFragment_status = clFragmentSanityCheck(outsideColoringAlgorithmStr, 2);
	if (this->status.fractalFragmentStatus + this->status.insideColoringFragmentStatus +
		this->status.outsideColoringFragment_status > 0) 
		return false;
	std::string antiAliasing("clFragments/full_template.cl");
	std::string antiAliasingStr = readCLFragmentFromFile("clFragments/anti_aliasing.cl");
	std::string colorStr = readCLFragmentFromFile("clFragments/colors.cl");
	std::string complexStr = readCLFragmentFromFile("clFragments/complex.cl");

	int parseError = 0;
	fractalFormulaStr = getVariableAndParameterNames(fractalFormulaStr, fPrefixes, FFFlag, &parseError);
	if (parseError > 0)
	{
		this->status.fractalFragmentStatus = parseError;
		return false;
	}
	insideColoringAlgorithmStr = getVariableAndParameterNames(insideColoringAlgorithmStr, ciPrefixes, FIFlag, &parseError);
	if (parseError > 0)
	{
		this->status.insideColoringFragmentStatus = parseError;
		return false;
	}
	outsideColoringAlgorithmStr = getVariableAndParameterNames(outsideColoringAlgorithmStr, coPrefixes, FOFlag, &parseError);
	if (parseError > 0)
	{
		this->status.outsideColoringFragment_status = parseError;
		return false;
	}
	// UF-style separation
	std::string fractalInit = getFragmentPart(fractalFormulaStr, ini, loo);
	std::string fractalLoop = getFragmentPart(fractalFormulaStr, loo, bai);
	std::string fractalBailout = getFragmentPart(fractalFormulaStr, bai, fun);
	std::string fractalFunctions = getFragmentPart(fractalFormulaStr, fun, eof);
	std::string insideColoringInit = getFragmentPart(insideColoringAlgorithmStr, ini, loo); 
	std::string insideColoringLoop = getFragmentPart(insideColoringAlgorithmStr, loo, fin);
	std::string insideColoringFinal = getFragmentPart(insideColoringAlgorithmStr, fin, fun);
	std::string insideColoringFunctions = getFragmentPart(insideColoringAlgorithmStr, fun, eof);
	std::string outsideColoringInit = getFragmentPart(outsideColoringAlgorithmStr, ini, loo);
	std::string outsideColoringLoop = getFragmentPart(outsideColoringAlgorithmStr, loo, fin);
	std::string outsideColoringFinal = getFragmentPart(outsideColoringAlgorithmStr, fin, fun);
	std::string outsideColoringFunctions = getFragmentPart(outsideColoringAlgorithmStr, fun, eof);

	// std::cout << "BEFORE:\n" << fullTemplateStr << std::endl;
	if (this->useDouble)
		fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(DPFlag), DPString);
	if (this->mode == 1)
		fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FLFlag), FLString);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(AAFlag), antiAliasingStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(CoFlag), colorStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(CPFlag), complexStr);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forIniFlag), fractalInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forLooFlag), fractalLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(forBaiFlag), fractalBailout);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FFFlag), fractalFunctions);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colInIFlag), insideColoringInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colLoIFlag), insideColoringLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colFiIFlag), insideColoringFinal);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FOFlag), insideColoringFunctions);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colInOFlag), outsideColoringInit);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colLoOFlag), outsideColoringLoop);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(colFiOFlag), outsideColoringFinal);
	fullTemplateStr = std::regex_replace(fullTemplateStr, std::regex(FIFlag), outsideColoringFunctions);

	paramCollector pc = parseParameters(fullTemplateStr, fractalFormulaStr, insideColoringAlgorithmStr, outsideColoringAlgorithmStr);

	// std::cout << fullTemplateStr << "\n";
	this->fullCLcode = fullTemplateStr;
	if (newFiles)
		this->params = pc;
	return true;
}