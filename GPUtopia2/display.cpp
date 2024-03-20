#include "display.h"

void runKernelAsync(clFractal& cf, clCore& cc, bool& running)
{
    //int ii = 0;
    //while (ii < fibonacci_number(cf.image.quality) && plsProceed)
    //{
    //    cl_int3 sampling = { ii, ii + 1, fibonacci_number(cf.image.quality) };
    //    cl_int err = cc.setKernelArg(4, sampling, "sampling_info");
    running = true;
    cc.runKernel(cf);
    running = false;
    //    ii++;
    //}
}

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
        ImGui::Begin("Main View", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        static GLuint textureID;
        static bool needTexture = false;
        static bool needImg = false;
        static bool refreshDefaultArguments = true;
        static bool refreshFractalArguments = true;
        static ImVec2 mainViewportSize = ImGui::GetContentRegionAvail();
        static clFractal cf;
        static std::vector<color> textureColors(cf.image.size.x * cf.image.size.y);
        static clFractal cf_old;
        static bool needCLFractal = true;
        static bool needNewKernel = true;
        static clCore core;
        // static asyncOpenCL asc;
        // static std::jthread jt;
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
            mainViewportSize = ImGui::GetContentRegionAvail();
            mainViewportSize.y < 1 ? mainViewportSize.y = 1 : mainViewportSize.y;
        }
        if (cf_old.image.size != cf.image.size)
        {
            needImg = true;
            textureColors.resize(cf.image.size.x * cf.image.size.y);
            //cf.image.size = { cf.image.size.x, cf.image.size.y };
            cf_old = cf;

        }
        if (needNewKernel or cf.rebuildKernel)
        {
            std::cout << "Need a new kernel, compiling it now.\n - requesting new texture\n - requesting new image\n";
            core.compileNewKernel(cf);
            needNewKernel = false;
            needImg = true;
            cf.rebuildKernel = false;
        }
        static paramCollector params_old = cf.params;
        static clFractalImage img_settings_old = cf.image;
        static bool redraw = false;
        formulaSettingsWindow(cf);
        imageSettingsWindow(cf);
        static bool runKernel = true;
        static int waitCounter = 0;
        if (cf != cf_old && waitCounter == 0) {
            std::cout << "Parameters changed, updating cf.params.\n - requesting new Texture\n - requesting new image\n";
            needImg = true;
            needCLFractal = false;
            std::cout << "Old zoom: " << cf_old.image.zoom << " new zoom: " << cf.image.zoom << std::endl;
            cf.image.updateComplexSubplane();
            core.setDefaultArguments(cf);
            core.setFractalKernelArgs(cf);
            cf_old = cf;
            runKernel = true;
        }

        glViewport(0, 0, mainViewportSize.x, mainViewportSize.y);
        // first draw the image
        static bool running = false;
        static std::jthread jt;
        if (runKernel and !running) {
            running = true; // set this here to prevent another img read before the called function sets this to true
            std::cout << "Need a new image, setting kernel args and running kernel.\n";
            core.setDefaultArguments(cf);
            core.setFractalKernelArgs(cf);
            jt = std::jthread(&runKernelAsync, std::ref(cf), std::ref(core), std::ref(running));
            jt.detach();
            needImg = true;
            runKernel = false;
        }
        if (running) {
            std::cout << "Am I running? " << running << std::endl;
            std::cout << "  My thread ID is " << jt.get_id() << std::endl;
        }
        if (needImg && !running)
        {
            waitCounter++;
            if (waitCounter > 1) // wait 1 frame before continuing
            {
                textureColors.resize(cf.image.size.x * cf.image.size.y);
                core.getImg(textureColors, cf);
                needImg = false;
                needTexture = true;
                waitCounter = 0;
            }
        }
        // create texture from the image
        if (needTexture) {
            std::cout << "Need a new texture, deleting old and remaking using the new image.\n";
            glDeleteTextures(1, &textureID);
            makeTexture(textureID, cf.image.size.x, cf.image.size.y, textureColors);
            needTexture = false;
        }

        ImGui::End();
        static char mainViewStr[] = "Main View";
        ImGui::Begin(mainViewStr, nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Image((void*)(intptr_t)textureID, ImVec2(
            cf.image.size.x, cf.image.size.y)); // , texturesize);
        ImGui::End();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
};