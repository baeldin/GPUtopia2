#pragma once

#include <vector>

#include "GL/glew.h"

#include "app_state.h"
#include "color.h"

// GL texture helpers for the viewport image.
void prepTexture(GLuint& texture);
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData);

namespace compositor
{
	// Read back every layer that finished rendering, composite the visible ones
	// back-to-front by opacity, and upload the result to the viewport texture.
	void readbackCompositeAndUpload(AppState& app);
}
