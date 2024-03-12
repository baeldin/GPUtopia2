#include "display.h"


//struct color
//{
//    float r;
//    float g;
//    float b;
//    float a = 1.;
//};

// prepare texture and bind to GLuint
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

// fill vector with some colors
void drawBogusImg(std::vector<color>& img, int width, int height)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img[y * width + x].r = 0;
            img[y * width + x].g = (float)x / width;
            img[y * width + x].b = (float)y / height;
        }
    }
}

namespace mainView
{
	void mainViewPort()
	{
		ImGui::Begin("Main View");
        static GLuint textureID;
        static bool needTexture = false;
        static bool needBogusImg = false;
        static bool refreshDefaultArguments = true;
        static bool refreshFractalArguments = true;
        static ImVec2 mainViewportSize = ImGui::GetContentRegionAvail();
        static std::vector<color> textureColors((int)(mainViewportSize.x * mainViewportSize.y));
        static clFractal cf;
        static clFractal cf_old;
        static bool needCLFractal = true;
        static bool needNewKernel = true;
        static clCore core;
        if (needCLFractal)
        {
            std::cout << "Need a fractal, re-reading code and generating kernel code.\n";
            cf.fractalCLFragmentFile = "clFragments/fractalFormulas/gnarl.cl";
            cf.coloringCLFragmentFile = "clFragments/coloringAlgorithms/dist.cl";
            cf.makeCLCode();
            needCLFractal = false;
            needNewKernel = true;
        }
        if (mainViewportSize.x != ImGui::GetContentRegionAvail().x ||
            mainViewportSize.y != ImGui::GetContentRegionAvail().y)
        {
            std::cout << "Viewport size changed, resizing and remaking image.\n";
            mainViewportSize = ImGui::GetContentRegionAvail();
            //prevent x and y from going negative
            //TODO: just set min size for window, but where?
            //mainViewportSize.x < 1 ? mainViewportSize.x = 1 : mainViewportSize.x;
            mainViewportSize.y < 1 ? mainViewportSize.y = 1 : mainViewportSize.y;
            // request new img and texture for new size, resize img vector
            needTexture = true;
            needBogusImg = true;
            textureColors.resize((int)(mainViewportSize.x * mainViewportSize.y));
            cf.image.size = { (int)mainViewportSize.x, (int)mainViewportSize.y };
        }
        if (needNewKernel or cf.rebuildKernel)
        {
            std::cout << "Need a new kernel, compiling it now.\n - requesting new texture\n - requesting new image\n";
            core.compileNewKernel(cf);
            needNewKernel = false;
            needTexture = true;
            needBogusImg = true;
            cf.rebuildKernel = false;
        }
        static paramCollector params_old = cf.params;
        static clFractalImage img_settings_old = cf.image;
        static bool redraw = false;
        formulaSettingsWindow(cf);
        if (cf != cf_old) {
            std::cout << "Parameters changed, updating cf.params.\n - requesting new Texture\n - requesting new image\n";
            needTexture = true;
            needBogusImg = true;
            needCLFractal = false;
            std::cout << "Old zoom: " << cf_old.image.zoom << " new zoom: " << cf.image.zoom << std::endl;
            cf.image.updateComplexSubplane();
            core.setDefaultArguments(cf);
            core.setFractalKernelArgs(cf);
            cf_old = cf;
        }

        glViewport(0, 0, mainViewportSize.x, mainViewportSize.y);
        // first draw the image
        if (needBogusImg) {
            std::cout << "Need a new image, setting kernel args and running kernel.\n";
            // drawBogusImg(textureColors, mainViewportSize.x, mainViewportSize.y);
            //make_img(textureColors, mainViewportSize.x, mainViewportSize.y, fs);
            // make_img2(cf, textureColors, mainViewportSize.x, mainViewportSize.y, fs);
            core.setDefaultArguments(cf);
            core.setFractalKernelArgs(cf);
            core.runKernel(cf);
            core.getImg(textureColors, cf);
            needBogusImg = false;
        }
        // create texture from the image
        if (needTexture) {
            std::cout << "Need a new texture, deleting old and remaking using the new image.\n";
            glDeleteTextures(1, &textureID);
            makeTexture(textureID, mainViewportSize.x, mainViewportSize.y, textureColors);
            needTexture = false;
        }

        ImGui::End();
        ImGui::Begin("Main View");
        ImGui::Image((void*)(intptr_t)textureID, ImVec2(
            cf.image.size.x, cf.image.size.y)); // , texturesize);
        ImGui::End();
        // ImVec2 vpPos = ImGui::GetCursorScreenPos();
        // this draws the font table instead of the blue/green image
        /*ImGui::GetWindowDrawList()->AddImage(
            (void*)textureID,
            ImVec2(vpPos.x, vpPos.y),
            ImVec2(vpPos.x + mainViewportSize.x, vpPos.y + mainViewportSize.y),
            ImVec2(0, 1),
            ImVec2(1, 0));
		ImGui::End();*/
        // bind default framebuffer again
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
};