#include "gradientWindow.h"

void gradientWindow(clFractal& cf)
{
    ImGui::Begin("Gradient", nullptr);
    ImGuiIO& io = ImGui::GetIO();
    static Gradient workGradientOld = cf.gradient;
    static Gradient workGradient = cf.gradient;
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V, false) && ImGui::IsWindowFocused())
    {
        // paste gradient
        std::string jsonStr = ReadStringFromClipboard();
        std::cout << jsonStr << "\n";
        json back_json = json::parse(jsonStr);
        auto grad = back_json.get<gradientContainer>();
        workGradient = Gradient(grad);
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C, false) && ImGui::IsWindowFocused())
    {
        // copy gradient
        json json = cf.gradient.toExport();
        std::cout << "Copying gradient to clipboard:\n";
        std::cout << json.dump(4);
        CopyStringToClipboard(json.dump());
    }
    // Yellow is content region min/max
    static ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    static ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    vMin = ImGui::GetWindowContentRegionMin();
    vMax = ImGui::GetWindowContentRegionMax();
    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;
    //ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
    static int gradientImgSizeX = vMax.x - vMin.x - 21;
    static int gradientImgSizeY = 80;
    ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
    ImVec2 uv1 = ImVec2(1.f, 1.f);// UV coordinates for (32,32) in our texture
    static GLuint textureGradientImgR, textureGradientImgG, textureGradientImgB;
    static bool windowChangedSize = false;
    static color nodeColorEdit = cf.gradient.nodeColors[0];
    static int nodeLocationEdit = cf.gradient.nodeLocations[0];
    static int lastActiveNode = 0;
    static bool needRGBlinePlots = true;
    static Gradient cfOld = cf.gradient;
    static Gradient cfNew = cf.gradient;
    cfNew = cf.gradient;
    if (ImGui::TreeNode("RGB"))
    {
        static int windowX = ImGui::GetContentRegionAvail().x;
        if (ImGui::GetContentRegionAvail().x != windowX)
        {
            needRGBlinePlots = true;
        }
        if (needRGBlinePlots)
        {
            gradientImgSizeX = vMax.x - vMin.x - 21;
            glDeleteTextures(1, &textureGradientImgR);
            glDeleteTextures(1, &textureGradientImgG);
            glDeleteTextures(1, &textureGradientImgB);
            std::vector<std::vector<color>> gradientImgDataRGB = {
                std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0.f),
                std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0.f),
                std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0.f) };
            gradientImgDataRGB = workGradient.drawWithRGBlines(gradientImgSizeX, gradientImgSizeY);
            makeTexture(textureGradientImgR, gradientImgSizeX, gradientImgSizeY, gradientImgDataRGB[0]);
            makeTexture(textureGradientImgG, gradientImgSizeX, gradientImgSizeY, gradientImgDataRGB[1]);
            makeTexture(textureGradientImgB, gradientImgSizeX, gradientImgSizeY, gradientImgDataRGB[2]);
            needRGBlinePlots = false;
            windowX = ImGui::GetContentRegionAvail().x;
        }
        ImGui::ImageButton((void*)(intptr_t)textureGradientImgR, ImVec2(gradientImgSizeX, gradientImgSizeY), uv0, uv1, 0); // , texturesize);

        static bool draggingNode = false;
        static int nodeCurrentX;
        static std::vector<int> fillOrder = workGradient.fillOrder;
        if (ImGui::IsItemHovered())
        {
            // cout << imgContentMin.x << " " << imgContentMax.x << " " << imgContentMin.y << " " << imgContentMax.y << "\n";
            std::vector<std::vector<int>> gradientImgRClickZones = {
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0) };
            gradientImgRClickZones = workGradient.getClickDotCoordinates(gradientImgSizeX, gradientImgSizeY);
            int mouseImgX = io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
            int mouseImgY = gradientImgSizeY + (io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()) + 4;
            int clickedPixelIndex = 0;
            if (mouseImgX > 0 && mouseImgY > 0 && mouseImgX < gradientImgSizeX && mouseImgY < gradientImgSizeY)
                clickedPixelIndex = gradientImgSizeX * (mouseImgY)+mouseImgX;            static int dragX = 0;
            static int dragY = 0;
            static float nodeStartingX, nodeStartingY;
            static int nodeHovered, colorHovered;
            if (!draggingNode) // stop updating nodeHovered once we are dragging one of them!
                nodeHovered = gradientImgRClickZones[0][clickedPixelIndex];
            //cout << mouseImgX << " " << mouseImgY << " " << clickedPixelIndex << " " << gradientImgRGBClickZones[0][clickedPixelIndex] << "\n";
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && nodeHovered >= 0)
            {
                workGradient.nodeHighlight = nodeHovered;
                lastActiveNode = nodeHovered;
                needRGBlinePlots = true;
                nodeStartingX = (float)workGradient.nodeLocations[nodeHovered];
                nodeStartingY = (float)workGradient.nodeColors[nodeHovered].r;
                cout << "Setting nodeStartingY to " << nodeStartingY << " mouseImgY : " << mouseImgY << "\n";
            }
            if (ImGui::IsItemActive() && nodeHovered >= 0)
            {
                draggingNode = true;
                dragX = io.MousePos.x - io.MouseClickedPos[0].x;
                dragY = io.MousePos.y - io.MouseClickedPos[0].y;
                //nodeStartingX = workGradient.nodeIndices[nodeHovered];
                //nodeCurrentX = nodeStartingX + dragX / gradientImgSizeX * workGradient.length;
                cout << "Dragging: " << dragX << ", " << dragY << " " << nodeHovered << "\n";
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                cout << "Setting r to " << nodeStartingY - (float)mouseImgY / gradientImgSizeY << "\n";
                workGradient.nodeColors[nodeHovered].r = nodeStartingY - (float)dragY / gradientImgSizeY;
                //workGradient.nodeColors[nodeHovered].r = nodeStartingY;// -((float)dragY) / gradientImgSizeY;
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggingNode)
            {
                draggingNode = false;
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                workGradient.nodeColors[nodeHovered].r = nodeStartingY - ((float)dragY) / gradientImgSizeY;
                cout << "Stopped dragging, updating X to " << workGradient.nodeLocations[nodeHovered] << " and Y to " << workGradient.nodeColors[nodeHovered].r * gradientImgSizeY << "\n";
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }

        }
        ImGui::ImageButton((void*)(intptr_t)textureGradientImgG, ImVec2(gradientImgSizeX, gradientImgSizeY), uv0, uv1, 0); // , texturesize);
        if (ImGui::IsItemHovered())
        {
            // cout << imgContentMin.x << " " << imgContentMax.x << " " << imgContentMin.y << " " << imgContentMax.y << "\n";
            std::vector<std::vector<int>> gradientImgGClickZones = {
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0) };
            gradientImgGClickZones = workGradient.getClickDotCoordinates(gradientImgSizeX, gradientImgSizeY);
            int mouseImgX = io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
            int mouseImgY = gradientImgSizeY + (io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()) + 4;
            int clickedPixelIndex = 0;
            if (mouseImgX > 0 && mouseImgY > 0 && mouseImgX < gradientImgSizeX && mouseImgY < gradientImgSizeY)
                clickedPixelIndex = gradientImgSizeX * (mouseImgY)+mouseImgX;            static int dragX = 0;
            static int dragY = 0;
            static float nodeStartingX, nodeStartingY;
            static int nodeHovered, colorHovered;
            if (!draggingNode) // stop updating nodeHovered once we are dragging one of them!
                nodeHovered = gradientImgGClickZones[1][clickedPixelIndex];
            //cout << mouseImgX << " " << mouseImgY << " " << clickedPixelIndex << " " << gradientImgRGBClickZones[0][clickedPixelIndex] << "\n";
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && nodeHovered >= 0)
            {
                workGradient.nodeHighlight = nodeHovered;
                lastActiveNode = nodeHovered;
                needRGBlinePlots = true;
                nodeStartingX = (float)workGradient.nodeLocations[nodeHovered];
                nodeStartingY = (float)workGradient.nodeColors[nodeHovered].g;
                cout << "Setting nodeStartingY to " << nodeStartingY << " mouseImgY : " << mouseImgY << "\n";
            }
            if (ImGui::IsItemActive() && nodeHovered >= 0)
            {
                draggingNode = true;
                dragX = io.MousePos.x - io.MouseClickedPos[0].x;
                dragY = io.MousePos.y - io.MouseClickedPos[0].y;
                //nodeStartingX = workGradient.nodeIndices[nodeHovered];
                //nodeCurrentX = nodeStartingX + dragX / gradientImgSizeX * workGradient.length;
                cout << "Dragging: " << dragX << ", " << dragY << " " << nodeHovered << "\n";
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                cout << "Setting r to " << nodeStartingY - (float)mouseImgY / gradientImgSizeY << "\n";
                workGradient.nodeColors[nodeHovered].g = nodeStartingY - (float)dragY / gradientImgSizeY;
                //workGradient.nodeColors[nodeHovered].g = nodeStartingY;// -((float)dragY) / gradientImgSizeY;
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggingNode)
            {
                draggingNode = false;
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                workGradient.nodeColors[nodeHovered].g = nodeStartingY - ((float)dragY) / gradientImgSizeY;
                cout << "Stopped dragging, updating X to " << workGradient.nodeLocations[nodeHovered] << " and Y to " << workGradient.nodeColors[nodeHovered].g * gradientImgSizeY << "\n";
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }

        }
        ImGui::ImageButton((void*)(intptr_t)textureGradientImgB, ImVec2(gradientImgSizeX, gradientImgSizeY), uv0, uv1, 0); // , texturesize);
        if (ImGui::IsItemHovered())
        {
            // cout << imgContentMin.x << " " << imgContentMax.x << " " << imgContentMin.y << " " << imgContentMax.y << "\n";
            std::vector<std::vector<int>> gradientImgBClickZones = {
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0),
                std::vector<int>(gradientImgSizeX * gradientImgSizeY, 0) };
            gradientImgBClickZones = workGradient.getClickDotCoordinates(gradientImgSizeX, gradientImgSizeY);
            int mouseImgX = io.MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
            int mouseImgY = gradientImgSizeY + (io.MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()) + 4;
            int clickedPixelIndex = 0;
            if (mouseImgX > 0 && mouseImgY > 0 && mouseImgX < gradientImgSizeX && mouseImgY < gradientImgSizeY)
                clickedPixelIndex = gradientImgSizeX * (mouseImgY)+mouseImgX;
            static int dragX = 0;
            static int dragY = 0;
            static float nodeStartingX, nodeStartingY;
            static int nodeHovered, colorHovered;
            if (!draggingNode) // stop updating nodeHovered once we are dragging one of them!
                nodeHovered = gradientImgBClickZones[2][clickedPixelIndex];
            //cout << mouseImgX << " " << mouseImgY << " " << clickedPixelIndex << " " << gradientImgRGBClickZones[0][clickedPixelIndex] << "\n";
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && nodeHovered >= 0)
            {
                workGradient.nodeHighlight = nodeHovered;
                lastActiveNode = nodeHovered;
                needRGBlinePlots = true;
                nodeStartingX = (float)workGradient.nodeLocations[nodeHovered];
                nodeStartingY = (float)workGradient.nodeColors[nodeHovered].b;
                cout << "Setting nodeStartingY to " << nodeStartingY << " mouseImgY : " << mouseImgY << "\n";
            }
            if (ImGui::IsItemActive() && nodeHovered >= 0)
            {
                draggingNode = true;
                dragX = io.MousePos.x - io.MouseClickedPos[0].x;
                dragY = io.MousePos.y - io.MouseClickedPos[0].y;
                //nodeStartingX = workGradient.nodeIndices[nodeHovered];
                //nodeCurrentX = nodeStartingX + dragX / gradientImgSizeX * workGradient.length;
                cout << "Dragging: " << dragX << ", " << dragY << " " << nodeHovered << "\n";
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                cout << "Setting r to " << nodeStartingY - (float)mouseImgY / gradientImgSizeY << "\n";
                workGradient.nodeColors[nodeHovered].b = nodeStartingY - (float)dragY / gradientImgSizeY;
                //workGradient.nodeColors[nodeHovered].b = nodeStartingY;// -((float)dragY) / gradientImgSizeY;
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggingNode)
            {
                draggingNode = false;
                workGradient.nodeLocations[nodeHovered] = nodeStartingX + dragX * (float)workGradient.length / gradientImgSizeX;
                workGradient.nodeColors[nodeHovered].b = nodeStartingY - ((float)dragY) / gradientImgSizeY;
                cout << "Stopped dragging, updating X to " << workGradient.nodeLocations[nodeHovered] << " and Y to " << workGradient.nodeColors[nodeHovered].b * gradientImgSizeY << "\n";
                needRGBlinePlots = true;
                workGradient.checkNodeOrder();
                workGradient.fill();
            }

        }
        ImGui::TreePop();
    }
    nodeColorEdit = workGradient.nodeColors[lastActiveNode];
    nodeLocationEdit = workGradient.nodeLocations[lastActiveNode];
    static color nodeColorEditOld = nodeColorEdit;
    static int nodeLocationEditOld = nodeLocationEdit;
    ImGui::SliderInt("Active Node Location", &nodeLocationEdit, 0, workGradient.length);
    ImGui::ColorEdit3("Active Node Color", (float*)&nodeColorEdit);
    ImGui::ColorEdit3("Active Node Color HSV", (float*)&nodeColorEdit);
    static bool redrawCalled = false;
    if (nodeColorEdit != nodeColorEditOld || nodeLocationEdit != nodeLocationEditOld)
    {
        workGradient.nodeColors[lastActiveNode] = nodeColorEdit;
        workGradient.nodeLocations[lastActiveNode] = nodeLocationEdit;
        needRGBlinePlots = true;
        // gradientChanged = true;
        workGradient.checkNodeOrder();
        workGradient.fill();
        // std::cout << "redrawing: " << blockDrawing << "\n";
        //if (!blockDrawing)
        //{
        //    m.lock(); // lock vec_img_f
        //    std::cout << "redrawing in if: " << blockDrawing << "\n";
        //    blockDrawing = true;
        //    redrawCalled = true;
        //    std::cout << "starting to redraw, it is now: " << blockDrawing << "\n";
        //    tb = std::thread(redrawFromGradientIndices, std::ref(vec_img_display), std::ref(vec_grad_idx), std::ref(colors), std::ref(blockDrawing), std::ref(m2));
        //    m.unlock(); // lock vec_img_f
        //}
        nodeColorEditOld = nodeColorEdit;
        nodeLocationEditOld = nodeLocationEdit;
    }
    if (ImGui::Button("Print Gradient Details"))
    {
        workGradient.print();
    }
    static GLuint textureGradientImgH, textureGradientImgS, textureGradientImgV;
    //if (ImGui::TreeNode("HSV"))
    //{
    //    if (needHSVlinePlots)
    //    {
    //        glDeleteTextures(1, &textureGradientImgH);
    //        glDeleteTextures(1, &textureGradientImgS);
    //        glDeleteTextures(1, &textureGradientImgV);
    //        std::vector<std::vector<color>> gradientImgDataHSV = {
    //            std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0),
    //            std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0),
    //            std::vector<color>(gradientImgSizeX * gradientImgSizeY, 0) };
    //        gradientImgDataHSV = workGradient.drawWithHSVlines(gradientImgSizeX, gradientImgSizeY);
    //        makeTexture(textureGradientImgH, gradientImgSizeX, gradientImgSizeY, gradientImgDataHSV[0]);
    //        makeTexture(textureGradientImgS, gradientImgSizeX, gradientImgSizeY, gradientImgDataHSV[1]);
    //        makeTexture(textureGradientImgV, gradientImgSizeX, gradientImgSizeY, gradientImgDataHSV[2]);
    //    }
    //    ImGui::Image((void*)(intptr_t)textureGradientImgH, ImVec2(gradientImgSizeX, gradientImgSizeY)); // , texturesize);
    //    ImGui::Image((void*)(intptr_t)textureGradientImgS, ImVec2(gradientImgSizeX, gradientImgSizeY));
    //    ImGui::Image((void*)(intptr_t)textureGradientImgV, ImVec2(gradientImgSizeX, gradientImgSizeY));
    //    ImGui::TreePop();
    //}
    static int nColors = 4;
    ImGui::InputInt("Nodes in gradient", &nColors);
    if (ImGui::Button("Generate random"))
    {
        workGradient = randomGradient(nColors);
        needRGBlinePlots = true;
    }
    //static std::string gradientUF = "";
    //static int gradientUFlen = gradientUF.length();
    //static const char* gradientUFchar = workGradient.printUF();
    //strcpy(gradientUFchar, gradientUF.c_str());
    //if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
    //    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
    //        delete gradientUFchar;
    //        static const char* gradientUFchar = workGradient.printUF();
    //    }
    //}
    //ImGui::Text(gradientUFchar);
    static std::vector<color> gradientImg;
    static GLuint gradientTextureID;
    static bool needGradientTexture;
    static int colorNodeCount = 4;
    static int currentGradientIndex = 3;
    static int previousGradientIndex = -1; // make sure that this is different initially
    const char* items[] = { "UF Default", "UF Default Muted", "Volcano Under a Glacier", "Volcano Under a Glacier2", "Jet", "CBR_coldhot", "nice_random", "Test" };
    static std::vector<color> gradient_img_data(400 * 30, color(0.f));
    static std::vector<Gradient> gradients = { uf_default, standard_muted, volcano_under_a_glacier, volcano_under_a_glacier2, jet, CBR_coldhot, nice_random, test };
    if (previousGradientIndex != currentGradientIndex)
    {
        workGradient = gradients[currentGradientIndex];
        previousGradientIndex = currentGradientIndex;
    }
    ImGui::Combo("Gradent", &currentGradientIndex, items, IM_ARRAYSIZE(items));
    ImGui::DragInt("Gradient Nodes", &colorNodeCount, 1, 12);
    if (ImGui::Button("Test Gradient"))
    {
        //cf.gradient = Gradient();
        workGradient = randomGradient(colorNodeCount);
        workGradient.print();
        // grad.printNodes();
        // grad.printFine();
    }
    if (cf.gradient != workGradient)
    {
        cf.gradient = workGradient;
        cf.status.runImgKernel = true;
        glDeleteTextures(1, &gradientTextureID);
        gradientImg = cf.gradient.getGradientImg(400, 20); // cf.gradient.getGradientImg(400, 20);
        makeTexture(gradientTextureID, 400, 20, gradientImg);
        needGradientTexture = false;
    }
    ImGui::Image((void*)(intptr_t)gradientTextureID, ImVec2(400, 20));
    ImGui::End(); // Gradient
}