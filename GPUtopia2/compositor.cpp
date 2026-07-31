#include "compositor.h"

#include <iostream>

#include "log.h"

void prepTexture(GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
}

// color the texture
void makeTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData)
{
	prepTexture(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeX, sizeY, 0, GL_RGBA, GL_FLOAT, imgData.data());
}

// refresh image
void refreshTexture(GLuint& texture, const int sizeX, const int sizeY, std::vector<color>& imgData)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sizeX, sizeY, 0, GL_RGBA, GL_FLOAT, imgData.data());
}

namespace compositor
{
	void readbackCompositeAndUpload(AppState& app)
	{
		// Per-layer image readback
		bool anyUpdated = false;
		for (auto& lp : app.layers) {
			FractalLayer& li = *lp;
			if (li.cf.status.updateImage and !li.cf.running())
			{
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": readback image, size=" << li.cf.image.size.x << "x" << li.cf.image.size.y << "\n";
				li.layerImage.resize(li.cf.image.size.x * li.cf.image.size.y);
				app.core.getImg(li.kernelState, li.layerImage, li.cf);
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": readback done\n";
				anyUpdated = true;
			}
		}
		if (anyUpdated) {
			int pixelCount = app.layers[0]->cf.image.size.x * app.layers[0]->cf.image.size.y;
			app.textureColors.resize(pixelCount);
			for (auto& p : app.textureColors) p = color(0.f, 0.f, 0.f, 0.f);
			// Back-to-front alpha compositing
			for (auto& lp : app.layers) {
				if (!lp->visible || (int)lp->layerImage.size() != pixelCount) continue;
				float a = lp->opacity;
				float oneMinusA = 1.f - a;
				for (int p = 0; p < pixelCount; p++)
					app.textureColors[p] = lp->layerImage[p] * a + app.textureColors[p] * oneMinusA;
			}
			app.updateTexture = true;
		}
		// create texture from the image
		if (app.updateTexture) {
			const clFractalImage& img = app.active().cf.image;
			glDeleteTextures(1, &app.textureID);
			makeTexture(app.textureID, img.size.x, img.size.y, app.textureColors);
			app.updateTexture = false;
		}
	}
}
