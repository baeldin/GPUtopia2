#include "gradient.h"


bool pixelInImage(const int x, const int y, const int sizeX, const int sizeY)
{
	return (x >= 0 && x < sizeX && y >= 0 && y < sizeY) ? true : false;
}

int Gradient::getIndex(const std::vector<int>& v, const int K) const
{
	auto it = find(v.begin(), v.end(), K);
	return it - v.begin();
}

Gradient::Gradient() {
	nodeLocationsOld = nodeLocations;
	fillOrder = getFillOrder(nodeLocations);
	fill();
}

Gradient::Gradient(int length_, std::vector<color> colors_, std::vector<int> locations_) {
	length = length_;
	nodeColors = colors_;
	nodeLocations = locations_;
	nodeCount = locations_.size();
	nodeLocationsOld = std::vector<int>(nodeCount);
	nodeIndex = std::vector<int>(nodeCount);
	fillOrder = std::vector<int>(nodeCount);
	for (int ii = 0; ii < nodeCount; ii++)
	{
		nodeIndex[ii] = ii;
	}
	fillOrder = getFillOrder(nodeLocations);
	fill();
}

Gradient::Gradient(int length_, std::vector<color> colors_, std::vector<int> locations_,
	std::vector<int> fillOrder_) {
	length = length_;
	nodeColors = colors_;
	nodeLocations = locations_;
	nodeCount = locations_.size();
	nodeLocationsOld = std::vector<int>(nodeCount);
	nodeIndex = std::vector<int>(nodeCount);
	fillOrder = fillOrder_;
	for (int ii = 0; ii < nodeCount; ii++)
	{
		nodeIndex[ii] = ii;
	}
	fill();
}

std::vector<int> Gradient::getFillOrder(const std::vector<int>& nodeLocations)
{
	std::vector<int> fO(nodeCount);
	int ll = 0;
	for (int ii = 0; ii <= length; ii++)
	{
		for (int jj = 0; jj < nodeCount; jj++)
		{
			//cout << nodeLocations[jj] << " " << ii << "\n";
			if (nodeLocations[jj] == ii)
			{
				//cout << "fillOrder[" << ii << "] = " << ll << "\n";
				fO[jj] = ll;
				ll++;
			}
		}
	}
	return fO;
}

void Gradient::fill()
{
	// this->print();
	// fillOrder = this->getFillOrder(nodeLocation);
	fine_indices.resize(fineLength);
	fineColors.resize(fineLength);
	for (int ii = 0; ii < fineLength; ii++)
	{
		fine_indices[ii] = ii;
		fineColors[ii] = get_color_cubic((float)ii / (float)fineLength);
	}
	//print();
}

void Gradient::print() const
{
	std::cout << "Printing gradient indices and colors:\n";
	std::cout << "============== raw ==================\n";
	std::cout << "Index, Pos in Sequence, Location, (r, g, b)\n";
	for (int ii = 0; ii < nodeCount; ii++)
	{
		std::cout << nodeIndex[ii] << ", " << fillOrder[ii] << ", " << nodeLocations[ii] << ", " << nodeColors[ii] << "\n";
		int idx = idxOfNodeOrd(ii);
		int loc = locOfNodeOrd(ii);
		//std::cout << ii << ", " << idx << ", " << loc << ", " << nodeColors[idx] << "\n";
	}
	std::cout << "============== ordered ==============\n";
	for (int ii = 0; ii < nodeCount; ii++)
	{
		int idx = idxOfNodeOrd(ii);
		int loc = locOfNodeOrd(ii);
		// std::cout << idx << ", " << ii << ", " << loc << ", " << nodeColors[idx] << "\n";
		std::cout << idx << "(" << fillOrder[ii] << ", " << nodeIndex[ii] << "): (" << nodeColors[ii].r << ", " << nodeColors[ii].g << ", " << nodeColors[ii].b << ")\n";
	}
	std::cout << "=====================================\n";
}

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

void Gradient::print_fine() const
{
	cout << "Printing gradient indices and colors:\n";
	cout << "=====================================\n";
	unsigned int ii = 0;
	for (int idx : fine_indices)
	{
		cout << idx << ": (" << fineColors[ii].r << ", " << fineColors[ii].g << ", " << fineColors[ii].b << ")\n";
		ii++;
	}
	cout << "=====================================\n";
}

// check the locations of the nodes and update fillOrder if necessary
void Gradient::checkNodeOrder()
{
	bool locationsChanged = false;
	for (int ii = 0; ii < nodeCount; ii++)
	{
		if (!nodeLocations[ii] == nodeLocationsOld[ii])
		{
			locationsChanged = true;
			break;
		}
	}
	if (locationsChanged)
	{
		fillOrder = this->getFillOrder(nodeLocations);
		for (int ii = 0; ii < nodeCount - 1; ii++)
		{
			if (nodeLocations[ii] > nodeLocations[ii + 1])
			{
				std::swap(nodeLocations[ii], nodeLocations[ii + 1]);
				std::swap(nodeColors[ii], nodeColors[ii + 1]);
			}
		}
		fillOrder = this->getFillOrder(nodeLocations);
		nodeLocationsOld = nodeLocations;
	}
}

std::vector<color> Gradient::draw(const int sizeX, const int sizeY) const
{
	// fill a small image with the gradient
	std::vector<color> gradient_picture(sizeX * sizeY);
	for (int ii = 0; ii < sizeX; ii++)
	{
		color current_color = get_color((float)ii / sizeX);
		for (int jj = 0; jj < sizeY; jj++)
		{
			gradient_picture[sizeX * jj + ii] = current_color;
		}
	}
	return gradient_picture;
}

std::vector<std::vector<int>> Gradient::getClickDotCoordinates(const int sizeX, const int sizeY) const
{
	std::vector<std::vector<int>> gradientPictureClickZones = {
		std::vector<int>(sizeX * sizeY, -1),
		std::vector<int>(sizeX * sizeY, -1),
		std::vector<int>(sizeX * sizeY, -1) };
	float scaleX = (float)sizeX / length;
	for (int jj = 0; jj < nodeCount; jj++)
	{
		int dotYValues[3] = {
			std::max<int>(0, (int)((1.f - nodeColors[jj].r) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - nodeColors[jj].g) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - nodeColors[jj].b) * (sizeY - 1))) };
		int dotXValue = nodeLocations[jj] * scaleX;
		int dotHalfSize = 3;
		for (int x = -dotHalfSize; x <= dotHalfSize; x++)
		{
			for (int y = -dotHalfSize; y <= dotHalfSize; y++)
			{
				for (int kk = 0; kk < 3; kk++)
				{
					if (pixelInImage(dotXValue + x, dotYValues[kk] + y, sizeX, sizeY))
					{
						int pixelIndex = sizeX * (dotYValues[kk] + y) + dotXValue + x;
						// cout << pixelIndex << " gets click zone value " << jj << "\n";
						gradientPictureClickZones[kk][pixelIndex] = nodeIndex[jj];
					}
				}
			}
		}
	}
	return gradientPictureClickZones;
}

std::vector<std::vector<color>> Gradient::drawWithRGBlines(const int sizeX, const int sizeY) const
{
	// fill a small image with the gradient
	float scaleX = (float)sizeX / length;
	std::vector<std::vector<color>> gradientPictures = { std::vector<color>(sizeX * sizeY), std::vector<color>(sizeX * sizeY), std::vector<color>(sizeX * sizeY) };
	for (int ii = 0; ii < sizeX; ii++)
	{
		color current_color = get_color((float)ii / sizeX);
		int curveValues[3] = {
			std::max<int>(0, (int)((1.f - current_color.r) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - current_color.g) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - current_color.b) * (sizeY - 1))) };
		for (int jj = 0; jj < sizeY; jj++)
		{
			gradientPictures[0][sizeX * jj + ii] = (jj == curveValues[0]) ? (ii % 4 < 2) ? color(0.f) : color(1.f) : current_color;
			gradientPictures[1][sizeX * jj + ii] = (jj == curveValues[1]) ? (ii % 4 < 2) ? color(0.f) : color(1.f) : current_color;
			gradientPictures[2][sizeX * jj + ii] = (jj == curveValues[2]) ? (ii % 4 < 2) ? color(0.f) : color(1.f) : current_color;
		}
	}
	for (int jj = 0; jj < nodeColors.size(); jj++)
	{
		int dotYValues[3] = {
			std::max<int>(0, (int)((1.f - nodeColors[jj].r) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - nodeColors[jj].g) * (sizeY - 1))),
			std::max<int>(0, (int)((1.f - nodeColors[jj].b) * (sizeY - 1))) };
		int dotXValue = (float)nodeLocations[jj] * scaleX;
		int dotHalfSize = (nodeHighlight == jj) ? 4 : 3;
		for (int x = -dotHalfSize; x <= dotHalfSize; x++)
		{
			for (int y = -dotHalfSize; y <= dotHalfSize; y++)
			{
				color frameColor = (nodeHighlight == jj) ? ((x + y) % 2 == 0) ? color(1.f) : color(0.f) : color(0.f);
				color dotColor = (std::max<int>(std::abs(x), std::abs(y)) >= 3) ? frameColor : color(0.5f);
				for (int kk = 0; kk < 3; kk++)
				{
					if (pixelInImage(dotXValue + x, dotYValues[kk] + y, sizeX, sizeY))
					{
						int pixelIndex = sizeX * (dotYValues[kk] + y) + dotXValue + x;
						gradientPictures[kk][pixelIndex] = dotColor;
					}
				}
			}
		}
	}
	return gradientPictures;
}
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

void Gradient::drawRline(const int sizeX, const int sizeY, std::vector<color> gradientImg) const
{
	for (int ii = 0; ii < sizeX; ii++)
	{
		color current_color = get_color((float)ii / sizeX);
		int curveValue = std::max(0, (int)((1.f - current_color.r) * (sizeY - 1)));
		color lineColor = (ii % 2 == 0) ? color(0.f) : color(1.f);
		gradientImg[curveValue * sizeX + ii] = lineColor;
		gradientImg[0] = color(1, 1, 1);
	}
}
color Gradient::get_color(float xidx) const
{
	//cout << "Getting color\n";
	unsigned int search_index = 0;
	int lower_idx = 0;
	int higher_idx = 0;
	color lower_col, higher_col;
	if (xidx > 1 || xidx < 0) {
		xidx = xidx - floor(xidx);
	}
	float xidx_scaled = (float)this->fineLength * xidx;
	if (xidx_scaled < (float)fine_indices[0])
	{
		lower_idx = fine_indices.back() - length;
		lower_col = fineColors.back();
		higher_idx = fine_indices[0];
		higher_col = fineColors[0];
	}
	else if (xidx_scaled > (float)fine_indices.back())
	{
		lower_idx = fine_indices.back();
		lower_col = fineColors.back();
		higher_idx = fine_indices[0] + length;
		higher_col = fineColors[0];
	}
	else
	{
		for (int idx : fine_indices)
		{

			if (xidx_scaled == (float)idx)
			{
				//cout << "Perfect hit, xidx_scaled: " << xidx_scaled << ", idx: " << idx << "\n";
				return this->fineColors[(const unsigned __int64)search_index];
			}
			else if ((float)idx > xidx_scaled)
			{
				lower_idx = fine_indices[search_index - 1];
				lower_col = fineColors[search_index - 1];
				higher_idx = fine_indices[search_index];
				higher_col = fineColors[search_index];
				break;
			}
			search_index++;
		}

	}
	//cout << "Linear fine indices: " << fine_indices[search_index - 1] << " " << fine_indices[search_index - 1] << "\n";
	float idx_fac = (xidx_scaled - (float)lower_idx) / ((float)higher_idx - (float)lower_idx);
	return color(
		lower_col.r + idx_fac * (higher_col.r - lower_col.r),
		lower_col.g + idx_fac * (higher_col.g - lower_col.g),
		lower_col.b + idx_fac * (higher_col.b - lower_col.b));
}

color Gradient::get_color(float xidx, const float density, const float offset, const bool repeat) const
{
	xidx = xidx * density + offset;
	if (!repeat)
		xidx = std::min(xidx, 1.f);
	return get_color(xidx);
}

// ONLY USED FOR INTIAL FILL
color Gradient::get_color_cubic(float xidx) const
{
	xidx = (xidx > 1 || xidx < 0) ? xidx - floor(xidx) : xidx;
	float xidx_scaled = (float)this->length * xidx;
	float firstNodeLocation = (float)locOfNodeOrd(0); // pos of first node in draw order
	float lastNodeLocation = (float)locOfNodeOrd(nodeCount - 1); // pos of last node in draw order
	unsigned int nextIndex = 0;
	if (xidx_scaled > firstNodeLocation && xidx_scaled < lastNodeLocation)
	{
		for (int ii = 0; ii < nodeCount; ii++)
		{
			float idx = (float)locOfNodeOrd(ii);
			if (xidx_scaled == idx)
			{
				// skip interpolation if index is hit exactly
				return nodeColors[this->getIndex(fillOrder, nextIndex)];
			}
			else if (idx > xidx_scaled)
			{
				break;
			}
			nextIndex++;
		}

	}
	int peviousIndex = (nextIndex < 1) ? nextIndex + nodeCount - 1 : nextIndex - 1;
	std::vector<float> spline_indices(4, 0.f);
	std::vector<color> spline_colors(4, 0.f);
	for (int jj = 0; jj < 4; jj++)
	{
		int fetch_index = nextIndex - 2 + jj;
		fetch_index = (fetch_index < 0) ? fetch_index + nodeCount : (fetch_index >= nodeCount) ? fetch_index - nodeCount : fetch_index;
		spline_indices[jj] = (float)nodeLocations[this->getIndex(fillOrder, fetch_index % nodeCount)];
		spline_colors[jj] = nodeColors[this->getIndex(fillOrder, fetch_index % nodeCount)];
		if (jj > 0) // make sure to cycle if the 4 indices contain the end and beginning
			spline_indices[jj] = (spline_indices[jj - 1] > spline_indices[jj]) ? spline_indices[jj] + this->length : spline_indices[jj];
	}
	if (spline_indices[1] > xidx_scaled)
		for (auto& element : spline_indices) { element -= this->length; }
	color ret = splined_color(spline_indices, spline_colors, xidx_scaled);
	return ret;
}

std::vector<color> Gradient::getGradientImg(const int width, const int height)
{
	std::vector<color> img(width * height, 0.f);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const int pixelIndex = width * y + x;
			const float colorIndex = float(x) / float(width);
			img[pixelIndex] = this->get_color_cubic(colorIndex);
		}
	}
	return img;
}



// OLD FILE CONTENS !!!! //
//#include "gradient.h"
//
//// node methods
//
//// gradient methods
//
//float lerp(const float x, const float x0, const float x1,
//	const float y0, const float y1)
//{
//	return ((x - x0) * y1 + (x1 - x) * y0) / (x1 - x0);
//}
//
//color lerp(const float x, const float x0, const float x1,
//	const color& c0, const color& c1)
//{
//	return color(
//		lerp(x, x0, x1, c0.r, c1.r),
//		lerp(x, x0, x1, c0.g, c1.g),
//		lerp(x, x0, x1, c0.b, c1.b),
//		lerp(x, x0, x1, c0.a, c1.a));
//}
//
//Gradient::Gradient()
//{
//	Gradient(4);
//}
//
//
//// extremely crude random gradient
//Gradient::Gradient(const int N)
//{
//	std::vector<int> nodeLocations(N, 0);
//	nNodes = N;
//	nodes.resize(N);
//	for (int k = 0; k < N; k++)
//	{
//		colorNode node;
//		fineColors.resize(fineLength);
//		node.index = k;
//		node.position = length / N * k;
//		node.color = color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.f);
//		//std::cout << "Set color of node " << k << " to (" << node.color.r << ", " << node.color.g << ", " <<
//		//	node.color.b << ")\n";
//		nodes[k] = node;
//	}
//	fillFine();
//}
//
//void Gradient::fillFine()
//{
//	// normalize to float indices first
//	if (nNodes == 1)
//	{
//		for (int idxFine = 0; idxFine < fineLength; idxFine++)
//			fineColors[idxFine] = nodes[0].color;
//	}
//	std::vector<float> floatNodePositions(nNodes);
//	const float invLength = 1.f / float(length);
//	const float invFineLen = 1.f / float(fineLength);
//	for (int nidx = 0; nidx < nNodes; nidx++)
//		floatNodePositions[nidx] = float(nodes[nidx].position) / float(length);
//	colorNode nodeBefore = (nodes[0].position > 0) ? nodes.back() : nodes[0]; // last node
//	colorNode nodeAfter = (nodes[0].position > 0) ? nodes[0] : nodes[1];
//	float floatIdxBefore = float(nodeBefore.position) * invLength;
//	float floatIdxAfter = float(nodeAfter.position) * invLength;
//	bool firstLeg = true;
//	if (floatIdxBefore > floatIdxAfter && firstLeg)
//		floatIdxBefore -= 1.f;
//	if (floatIdxBefore > floatIdxAfter && !firstLeg)
//		floatIdxAfter += 1.f;
//	//std::cout << "Node before now at " << floatIdxBefore << " with color (" <<
//	//	nodeBefore.color.r << ", " << nodeBefore.color.g << ", " << nodeBefore.color.b << ")" << std::endl;
//	//std::cout << "Node after now at " << floatIdxAfter << " with color (" <<
//	//	nodeAfter.color.r << ", " << nodeAfter.color.g << ", " << nodeAfter.color.b << ")" << std::endl;	
//	float fFine = 0.f;
//	for (int idxFine = 0; idxFine < fineLength; idxFine++)
//	{
//		// check if we passed a node
//		if (fFine > floatIdxAfter)
//		{
//			// std::cout << "Swapping nodes at fFine = " << fFine << ".\n";
//			int nextIdx = (nodeAfter.index == nNodes - 1) ? 0 : nodeAfter.index + 1;
//			nodeBefore = nodeAfter;
//			nodeAfter = nodes[nextIdx];
//			firstLeg = false;
//			floatIdxBefore = float(nodeBefore.position) * invLength;
//			floatIdxAfter = float(nodeAfter.position) * invLength;
//			if (floatIdxBefore > floatIdxAfter && firstLeg)
//				floatIdxBefore -= 1.f;
//			if (floatIdxBefore > floatIdxAfter && !firstLeg)
//				floatIdxAfter += 1.f;
//			//std::cout << "Node before now at " << floatIdxBefore << " with color (" <<
//			//	nodeBefore.color.r << ", " << nodeBefore.color.g << ", " << nodeBefore.color.b << ")" << std::endl;
//			//std::cout << "Node after now at " << floatIdxAfter << " with color (" <<
//			//	nodeAfter.color.r << ", " << nodeAfter.color.g << ", " << nodeAfter.color.b << ")" << std::endl;
//		}
//		fFine = float(idxFine) * invFineLen;
//		fineColors[idxFine] = lerp(fFine, floatIdxBefore, floatIdxAfter, nodeBefore.color, nodeAfter.color);
//	}
//}
//
//void Gradient::printNodes()
//{
//	std::cout << "Printing gradient nodes\n";
//	std::cout << "=======================================\n";
//	for (colorNode node : nodes)
//	{
//		std::cout << "Node " << node.index << " has position " << node.position << " and color (" <<
//			node.color.r << ", " << node.color.g << ", " << node.color.b << ")\n";
//	}
//	std::cout << "=======================================\n";
//}
//
//void Gradient::printFine()
//{
//	std::cout << "Printing gradient fine colors\n";
//	std::cout << "=======================================\n";
//	int ii = 0;
//	for (color col : fineColors)
//	{
//		float fFine = float(ii) / float(fineLength);
//		std::cout << "Color " << ii << " at fFine " << fFine << ": (" << col.r << ", " << col.g << ", " << col.b << ")\n";
//		ii++;
//	}
//	std::cout << "=======================================\n";
//}
//
//color Gradient::getColor(const float idxIn)
//{
//	const float idx = fract(idxIn);
//	const int idxInt = int(idx * fineLength);
//	return fineColors[idxInt];
//}
//
//std::vector<color> Gradient::getGradientImg(const int width, const int height)
//{
//	std::vector<color> img(width * height, 0);
//	for (int y = 0; y < height; y++)
//	{
//		for (int x = 0; x < width; x++)
//		{
//			const int pixelIndex = width * y + x;
//			const float colorIndex = float(x) / float(width);
//			img[pixelIndex] = this->getColor(colorIndex);
//		}
//	}
//	return img;
//}