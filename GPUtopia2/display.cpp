#include "display.h"


struct color
{
    float r;
    float g;
    float b;
    float a = 1.;
};

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
        static bool needFrameBuffer = true;
        static bool needTexture = true;
        static bool needBogusImg = true;
        static GLuint frameBufferID2 = 0;
        static GLuint textureID = 0;
        static ImVec2 mainViewportSize = ImGui::GetContentRegionAvail();
        static bool needGlewInit = true;
        static std::vector<color> textureColors((int)(mainViewportSize.x * mainViewportSize.y));
        //glfwGetCurrentContext();
        //GLFWwindow* window = glfwGetCurrentContext();

        if (mainViewportSize.x != ImGui::GetContentRegionAvail().x ||
            mainViewportSize.y != ImGui::GetContentRegionAvail().y) {
            mainViewportSize = ImGui::GetContentRegionAvail();
            // prevent x and y from going negative
            // TODO: just set min size for window, but where?
            mainViewportSize.x < 1 ? mainViewportSize.x = 1 : mainViewportSize.x;
            mainViewportSize.y < 1 ? mainViewportSize.y = 1 : mainViewportSize.y;
            // request new img and texture for new size, resize img vector
            needTexture = true;
            needBogusImg = true;
            textureColors.resize((int)(mainViewportSize.x * mainViewportSize.y));
        }
        glViewport(0, 0, mainViewportSize.x, mainViewportSize.y);
        // TODO: make a buffer to use in a compute shader and fill the image vector with data
        // from the buffer (is that even the right way of saying it??)
        if (needFrameBuffer) {
            // this was needed to prevent a crash, but it causes the main window to go black
            //if (needGlewInit) {
            //    glewInit();
            //    needGlewInit = false;
            //}
            glGenFramebuffers(1, &frameBufferID2);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID2);
            needFrameBuffer = false;
        }
        if (needBogusImg) {
            drawBogusImg(textureColors, mainViewportSize.x, mainViewportSize.y);
            needBogusImg = false;
        }
        if (needTexture) {
            glDeleteTextures(1, &textureID);
            makeTexture(textureID, mainViewportSize.x, mainViewportSize.y, textureColors);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        }
        ImVec2 vpPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddImage(
            (void*)textureID,
            ImVec2(vpPos.x, vpPos.y),
            ImVec2(vpPos.x + mainViewportSize.x, vpPos.y + mainViewportSize.y),
            ImVec2(0, 1),
            ImVec2(1, 0));
		ImGui::End();
	}
};