#pragma once

#include "gradient.h"

// Built-in gradients. The definitions live in test_gradients.cpp so that the
// ~1000 lines of colour literals are compiled once instead of in every
// translation unit that needs a Gradient.

inline Gradient randomGradient(const int nNodes)
{
	std::vector<color> colors(nNodes);
	std::vector<int> locations(nNodes);
	for (int ii = 0; ii < nNodes; ii++)
	{
		colors[ii] = color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
		locations[ii] = (int)(400 * (float)rand() / RAND_MAX);
	}
	return Gradient(400, colors, locations);
}

extern const int CBR_oldhot_length;
extern const int jet_length;

extern const Gradient nice_random;
extern const Gradient test;
extern const Gradient CBR_coldhot;
extern const Gradient volcano_under_a_glacier2;
extern const Gradient jet;
extern const Gradient standard_muted;
extern const Gradient volcano_under_a_glacier;
extern const Gradient uf_default;
