#include "gradient.h"

// node methods

// gradient methods

gradient::gradient()
{
	gradient(4);
}


// extremely crude random gradient
gradient::gradient(const int N)
{
	std::vector<int> nodeLocations(N, 0);
	nodes.resize(N);
	for (int k = 0; k < N; k++)
	{
		colorNode node;
		node.index = k;
		node.position = length / N * k;
		node.color = color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1.f);
	}
}

