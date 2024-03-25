#include "gradient.h"

// node methods

// gradient methods

float lerp(const float x, const float x0, const float x1,
	const float y0, const float y1)
{
	return ((x - x0) * y1 + (x1 - x) * y0) / (x1 - x0);
}

color lerp(const float x, const float x0, const float x1,
	const color& c0, const color& c1)
{
	return color(
		lerp(x, x0, x1, c0.r, c1.r),
		lerp(x, x0, x1, c0.g, c1.g),
		lerp(x, x0, x1, c0.b, c1.b),
		lerp(x, x0, x1, c0.a, c1.a));
}

Gradient::Gradient()
{
	Gradient(4);
}


// extremely crude random gradient
Gradient::Gradient(const int N)
{
	std::vector<int> nodeLocations(N, 0);
	nNodes = N;
	nodes.resize(N);
	for (int k = 0; k < N; k++)
	{
		colorNode node;
		fineColors.resize(fineLength);
		node.index = k;
		node.position = length / N * k;
		node.color = color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.f);
		//std::cout << "Set color of node " << k << " to (" << node.color.r << ", " << node.color.g << ", " <<
		//	node.color.b << ")\n";
		nodes[k] = node;
	}
	fillFine();
}

void Gradient::fillFine()
{
	// normalize to float indices first
	if (nNodes == 1)
	{
		for (int idxFine = 0; idxFine < fineLength; idxFine++)
			fineColors[idxFine] = nodes[0].color;
	}
	std::vector<float> floatNodePositions(nNodes);
	const float invLength = 1.f / float(length);
	const float invFineLen = 1.f / float(fineLength);
	for (int nidx = 0; nidx < nNodes; nidx++)
		floatNodePositions[nidx] = float(nodes[nidx].position) / float(length);
	colorNode nodeBefore = (nodes[0].position > 0) ? nodes.back() : nodes[0]; // last node
	colorNode nodeAfter = (nodes[0].position > 0) ? nodes[0] : nodes[1];
	float floatIdxBefore = float(nodeBefore.position) * invLength;
	float floatIdxAfter = float(nodeAfter.position) * invLength;
	bool firstLeg = true;
	if (floatIdxBefore > floatIdxAfter && firstLeg)
		floatIdxBefore -= 1.f;
	if (floatIdxBefore > floatIdxAfter && !firstLeg)
		floatIdxAfter += 1.f;
	//std::cout << "Node before now at " << floatIdxBefore << " with color (" <<
	//	nodeBefore.color.r << ", " << nodeBefore.color.g << ", " << nodeBefore.color.b << ")" << std::endl;
	//std::cout << "Node after now at " << floatIdxAfter << " with color (" <<
	//	nodeAfter.color.r << ", " << nodeAfter.color.g << ", " << nodeAfter.color.b << ")" << std::endl;	
	float fFine = 0.f;
	for (int idxFine = 0; idxFine < fineLength; idxFine++)
	{
		// check if we passed a node
		if (fFine > floatIdxAfter)
		{
			// std::cout << "Swapping nodes at fFine = " << fFine << ".\n";
			int nextIdx = (nodeAfter.index == nNodes - 1) ? 0 : nodeAfter.index + 1;
			nodeBefore = nodeAfter;
			nodeAfter = nodes[nextIdx];
			firstLeg = false;
			floatIdxBefore = float(nodeBefore.position) * invLength;
			floatIdxAfter = float(nodeAfter.position) * invLength;
			if (floatIdxBefore > floatIdxAfter && firstLeg)
				floatIdxBefore -= 1.f;
			if (floatIdxBefore > floatIdxAfter && !firstLeg)
				floatIdxAfter += 1.f;
			//std::cout << "Node before now at " << floatIdxBefore << " with color (" <<
			//	nodeBefore.color.r << ", " << nodeBefore.color.g << ", " << nodeBefore.color.b << ")" << std::endl;
			//std::cout << "Node after now at " << floatIdxAfter << " with color (" <<
			//	nodeAfter.color.r << ", " << nodeAfter.color.g << ", " << nodeAfter.color.b << ")" << std::endl;
		}
		fFine = float(idxFine) * invFineLen;
		fineColors[idxFine] = lerp(fFine, floatIdxBefore, floatIdxAfter, nodeBefore.color, nodeAfter.color);
	}
}

void Gradient::printNodes()
{
	std::cout << "Printing gradient nodes\n";
	std::cout << "=======================================\n";
	for (colorNode node : nodes)
	{
		std::cout << "Node " << node.index << " has position " << node.position << " and color (" <<
			node.color.r << ", " << node.color.g << ", " << node.color.b << ")\n";
	}
	std::cout << "=======================================\n";
}

void Gradient::printFine()
{
	std::cout << "Printing gradient fine colors\n";
	std::cout << "=======================================\n";
	int ii = 0;
	for (color col : fineColors)
	{
		float fFine = float(ii) / float(fineLength);
		std::cout << "Color " << ii << " at fFine " << fFine << ": (" << col.r << ", " << col.g << ", " << col.b << ")\n";
		ii++;
	}
	std::cout << "=======================================\n";
}

color Gradient::getColor(const float idxIn)
{
	const float idx = fract(idxIn);
	const int idxInt = int(idx * fineLength);
	return fineColors[idxInt];
}

std::vector<color> Gradient::getGradientImg(const int width, const int height)
{
	std::vector<color> img(width * height, 0);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const int pixelIndex = width * y + x;
			const float colorIndex = float(x) / float(width);
			img[pixelIndex] = this->getColor(colorIndex);
		}
	}
	return img;
}