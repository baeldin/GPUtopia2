#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

#include "color.h"
#include "cubic_spline.h"


bool pixelInImage(const int x, const int y, const int sizeX, const int sizeY); 

class Gradient
{
public:
	int fineLength = 400;
	std::vector<color> fineColors;
	std::vector<int> fine_indices;
	std::vector<int> nodeIndex;
	std::vector<int> nodeLocationsOld;
	// very basic function, assumes that K will always be found
	// only to be used from within the class
	int getIndex(const std::vector<int>& v, const int K) const;
	int length;
	int nodeCount;
	std::vector<color> nodeColors;
	std::vector<int> nodeLocation;
	std::vector<int> fillOrder;
	int nodeHighlight = 1; // default no node highlighted
	std::string name = "DefaultName";
	Gradient();
	Gradient(int length_, std::vector<color> colors_, std::vector<int> locations_);
	Gradient(int length_, std::vector<color> colors_, std::vector<int> locations_, std::vector<int> fillOrder_);
	color get_color(float xidx, const float density, const float offset, const bool repeat) const;
	color getColor(const int N) const { return nodeColors[N]; }
	// index from pos in draw sequence
	int idxOfNodeOrd(const int N) const { return getIndex(fillOrder, N); }
	// pos in draw sequence from index
	int ordOfNodeIdx(const int N) const { return fillOrder[N]; }
	// location in gradient from index
	int locOfNodeIdx(const int N) const { return nodeLocation[N]; }
	// index from location in gradient
	int idxOfNodeLoc(const int N) const { return getIndex(nodeIndex, N); }
	// location in gradient from pos in draw sequence
	int locOfNodeOrd(const int N) const { return locOfNodeIdx(idxOfNodeOrd(N)); }
	// pos in draw sequence from location in gradient
	int ordOfNodeLoc(const int N) const { return ordOfNodeIdx(idxOfNodeLoc(N)); }
	std::vector<int> getFillOrder(const std::vector<int>& nodeLocations);
	void fill();
	void print() const;
	//const char* printUF() const
	//{
	//	std::string outStr = "";
	//	outStr += "Printing UF Gradient Format\n";
	//	outStr += "===========================\n";
	//	outStr += name + " {\n";
	//	outStr += "gradient:\n";
	//	outStr += "title=" + name + " smooth=yes,\n";
	//	for (int ii = 0; ii < nodeLocation.size(); ii++)
	//	{
	//		outStr += "index = " + std::to_string(nodeLocation[ii]) + " color = " + std::to_string(nodeColors[ii].toInt()) + "\n";
	//	}
	//	outStr += "}\n";
	//	const char* outChar = outStr.c_str();
	//	return outChar;
	//}
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
};

inline bool operator==(const Gradient& lhs, const Gradient& rhs)
{
	return (
		lhs.nodeColors == rhs.nodeColors &&
		lhs.nodeLocation == rhs.nodeLocation &&
		lhs.fillOrder == rhs.fillOrder);
}

inline bool operator!=(const Gradient& lhs, const Gradient& rhs)
{
	return !(lhs == rhs);
}