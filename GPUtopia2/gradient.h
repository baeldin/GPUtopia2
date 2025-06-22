#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

#include "json.hpp"
using json = nlohmann::ordered_json;

#include "color.h"
#include "cubic_spline.h"

bool pixelInImage(const int x, const int y, const int sizeX, const int sizeY); 

class gradientContainer
{
public:
	std::string name = "Unnamed Gradient";
	int length = 400;
	std::vector<int> colors;
	std::vector<int> indices;
	gradientContainer() {}
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(gradientContainer,
		name, length, colors, indices);
};

class Gradient
{
public:
	int length = 400;
	int nodeCount = 4;
	std::vector<color> nodeColors; // = { color(0,0,0), color(1, 0,0), color(1,1,1), color(0.5f,0.5f,0.9f) };
	std::vector<int> nodeIndex; // = { 1, 2, 3, 4 };
	std::vector<int> nodeLocations; // = { 0, 10, 20, 30 };
	int fineLength = 400;
	std::vector<color> fineColors;
	std::vector<int> fine_indices;
	std::vector<int> nodeLocationsOld;
	// very basic function, assumes that K will always be found
	// only to be used from within the class
	int getIndex(const std::vector<int>& v, const int K) const;
	std::vector<int> fillOrder;
	int nodeHighlight = 1; // default no node highlighted
	std::string name = "Unnamed Gradient";
	bool isNew = false;
	Gradient() {}
	Gradient(const gradientContainer& gc);
	Gradient(int length_, std::vector<color> colors_, std::vector<int> locations_);
	color get_color(float xidx, const float density, const float offset, const bool repeat) const;
	color getColor(const int N) const { return nodeColors[N]; }
	// index from pos in draw sequence
	int idxOfNodeOrd(const int N) const { return getIndex(fillOrder, N); }
	// pos in draw sequence from index
	int ordOfNodeIdx(const int N) const { return fillOrder[N]; }
	// location in gradient from index
	int locOfNodeIdx(const int N) const { return nodeLocations[N]; }
	// index from location in gradient
	int idxOfNodeLoc(const int N) const { return getIndex(nodeIndex, N); }
	// location in gradient from pos in draw sequence
	int locOfNodeOrd(const int N) const { return locOfNodeIdx(idxOfNodeOrd(N)); }
	// pos in draw sequence from location in gradient
	int ordOfNodeLoc(const int N) const { return ordOfNodeIdx(idxOfNodeLoc(N)); }
	std::vector<int> getFillOrder(const std::vector<int>& nodeLocations);
	void fill();
	void print() const;
	const char* printUF() const
	{
		std::string outStr = "";
		outStr += "Printing UF Gradient Format\n";
		outStr += "===========================\n";
		outStr += name + " {\n";
		outStr += "gradient:\n";
		outStr += "title=" + name + " smooth=yes,\n";
		for (int ii = 0; ii < nodeLocations.size(); ii++)
		{
			outStr += "index = " + std::to_string(nodeLocations[ii]) + " color = " + std::to_string(nodeColors[ii].toUFint()) + "\n";
		}
		outStr += "}\n";
		const char* outChar = outStr.c_str();
		return outChar;
	}
	void print_fine() const;
	// check the locations of the nodes and update fillOrder if necessary
	void checkNodeOrder();
	std::vector<color> draw(const int sizeX, const int sizeY) const;
	std::vector<std::vector<int>> getClickDotCoordinates(const int sizeX, const int sizeY) const;
	std::vector<std::vector<color>> drawWithRGBlines(const int sizeX, const int sizeY) const;
	//std::vector<std::vector<color>> drawWithHSVlines(const int sizeX, const int sizeY) const
	//{
	//	// fill a small image with the gradient
	//	std::vector<std::vector<color>> gradientPictures = { std::vector<color>(sizeX * sizeY), std::vector<color>(sizeX * sizeY), std::vector<color>(sizeX * sizeY) };
	//	for (int ii = 0; ii < sizeX; ii++)
	//	{
	//		color current_color = get_color((float)ii / sizeX);
	//		colorHSV currentHSV = colorHSV(current_color);
	//		int curveValues[3] = {
	//			std::max<float>(0, (int)((1.f - currentHSV.h / 360.f) * (sizeY - 1))),
	//			std::max<float>(0, (int)((1.f - currentHSV.s) * (sizeY - 1))),
	//			std::max<float>(0, (int)((1.f - currentHSV.v) * (sizeY - 1))) };
	//		for (int jj = 0; jj < sizeY; jj++)
	//		{
	//			gradientPictures[0][sizeX * jj + ii] = (jj == curveValues[0]) ? (ii % 4 < 2) ? color(0) : color(1) : current_color;
	//			gradientPictures[1][sizeX * jj + ii] = (jj == curveValues[1]) ? (ii % 4 < 2) ? color(0) : color(1) : current_color;
	//			gradientPictures[2][sizeX * jj + ii] = (jj == curveValues[2]) ? (ii % 4 < 2) ? color(0) : color(1) : current_color;
	//		}
	//	}
	//	return gradientPictures;
	//}
	void drawRline(const int sizeX, const int sizeY, std::vector<color> gradientImg) const;
	color get_color(float xidx) const;
	// ONLY USED FOR INTIAL FILL
	color get_color_cubic(float xidx) const;
	std::vector<color> getGradientImg(const int width, const int height);
	gradientContainer toExport() const;
	// NLOHMANN_DEFINE_TYPE_INTRUSIVE(Gradient, 
	// 	name, length, nodeColors, nodeLocations);
};

inline bool operator==(const Gradient& lhs, const Gradient& rhs)
{
	//std::cout << "lhs.nodeColors == rhs.nodeColors " << (lhs.nodeColors == rhs.nodeColors) << "\n";
	//std::cout << "lhs.nodeLocations == rhs.nodeLocations " << (lhs.nodeLocations == rhs.nodeLocations) << "\n";
	// std::cout << "lhs.fillOrder == rhs.fillOrder " << (lhs.fillOrder == rhs.fillOrder) << "\n";
	return (
		lhs.nodeColors == rhs.nodeColors &&
		lhs.nodeLocations == rhs.nodeLocations); //&&
		//lhs.fillOrder == rhs.fillOrder);
}

inline bool operator!=(const Gradient& lhs, const Gradient& rhs)
{
	return !(lhs == rhs);
}