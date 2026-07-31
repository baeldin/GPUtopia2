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

namespace
{
	// The screen applies the sRGB curve to whatever value it is handed, so the
	// light a pixel actually emits is sRGBtoLinear(value) no matter which tone
	// curve produced it - the escape-time linearToSRGB or the flame gamma.
	// Blending has to happen on those decoded values.
	//
	// color.h's conversions are pow()-based and the compositor runs a few
	// million of them per re-blend, so go through a table. 8192 steps keeps the
	// worst-case error, in the toe where the curve is steepest, below half of an
	// 8-bit code value.
	constexpr int transferLUTSize = 8192;

	struct TransferLUT
	{
		float toSRGB[transferLUTSize];
		float toLinear[transferLUTSize];
		TransferLUT()
		{
			for (int i = 0; i < transferLUTSize; i++)
			{
				const float x = (float)i / (float)(transferLUTSize - 1);
				toSRGB[i] = linearToSRGB(x);
				toLinear[i] = sRGBtoLinear(x);
			}
		}
	};
	const TransferLUT transferLUT;

	inline int lutIndex(const float x)
	{
		const float c = (x < 0.f) ? 0.f : (x > 1.f) ? 1.f : x;
		return (int)(c * (float)(transferLUTSize - 1) + 0.5f);
	}

	// Alpha is coverage, not a colour channel: it is left alone by both.
	inline color toLinear(const color& c)
	{
		return color(transferLUT.toLinear[lutIndex(c.r)],
			transferLUT.toLinear[lutIndex(c.g)],
			transferLUT.toLinear[lutIndex(c.b)], c.a);
	}

	inline color toSRGB(const color& c)
	{
		return color(transferLUT.toSRGB[lutIndex(c.r)],
			transferLUT.toSRGB[lutIndex(c.g)],
			transferLUT.toSRGB[lutIndex(c.b)], c.a);
	}
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
				const int pixels = li.cf.image.size.x * li.cf.image.size.y;
				li.layerImage.resize(pixels);
				app.core.getImg(li.kernelState, li.layerImage, li.cf);
				// Decode once here rather than once per re-blend: the composite
				// also runs when only an opacity slider moved.
				li.layerImageLinear.resize(pixels);
				for (int p = 0; p < pixels; p++)
					li.layerImageLinear[p] = toLinear(li.layerImage[p]);
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": readback done\n";
				anyUpdated = true;
			}
		}
		if (anyUpdated || app.compositeDirty) {
			int pixelCount = app.layers[0]->cf.image.size.x * app.layers[0]->cf.image.size.y;
			app.textureColors.resize(pixelCount);
			for (auto& p : app.textureColors) p = color(0.f, 0.f, 0.f, 0.f);
			// Back-to-front source-over compositing, in linear light.
			for (auto& lp : app.layers) {
				if (!lp->visible || (int)lp->layerImageLinear.size() != pixelCount) continue;
				const float opacity = lp->opacity;
				for (int p = 0; p < pixelCount; p++) {
					const color& src = lp->layerImageLinear[p];
					color& dst = app.textureColors[p];
					// Coverage is the layer's own alpha scaled by its opacity;
					// using opacity alone would let transparent parts of a flame
					// layer occlude everything underneath.
					const float w = src.a * opacity;
					const float invW = 1.f - w;
					dst.r = src.r * w + dst.r * invW;
					dst.g = src.g * w + dst.g * invW;
					dst.b = src.b * w + dst.b * invW;
					dst.a = w + dst.a * invW;
				}
			}
			// Back to display space for the texture, the PNG export and the
			// navigation preview, all of which expect encoded values.
			for (auto& p : app.textureColors) p = toSRGB(p);
			app.compositeDirty = false;
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
