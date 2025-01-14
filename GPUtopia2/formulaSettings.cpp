#include "formulaSettings.h"


void formulaSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Formulas");

	static char formulaCLFragment[100] = "clFragments/fractalFormulas/mandelbrot.cl";
	static char coloringCLFragment[100] = "clFragments/coloringAlgorithms/by_iteration.cl";
	ImGui::InputText("Formula:", formulaCLFragment, 100);
	ImGui::InputText("Coloring:", coloringCLFragment, 100);
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.fractalCLFragmentFile = std::string(formulaCLFragment);
		cf.coloringCLFragmentFile = std::string(coloringCLFragment);
		cf.makeCLCode();
		cf.buildKernel = true;
	}
	ImGui::DragFloat("Bailout", &cf.bailout, 4., 20000.);
	ImGui::Text("Formula Parameters:");
	for (auto& [key, val] : cf.params.fractalParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &(val.first), 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.fractalParameterMaps.floatParameters)
	{
		ImGui::DragFloat(key.c_str(), &(val.first), 0.01f, -100.f, 100.f);
	}
	ImGui::Text("ColoringParameters");
	for (auto& [key, val] : cf.params.coloringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.coloringParameterMaps.floatParameters)
	{
		ImGui::DragFloat(key.c_str(), &val.first, 0.01f, -100.f, 100.f);
	}
	static std::vector<color> gradientImg;
	static GLuint gradientTextureID;
	static bool needGradientTexture;
	static Gradient gradOld;
	static int colorNodeCount = 4;
	static int currentGradientIndex = 1;
	static int previousGradientIndex = -1; // make sure that this is different initially
	const char* items[] = { "UF Default", "UF Default Muted", "Volcano Under a Glacier", "Volcano Under a Glacier2", "Jet", "CBR_coldhot", "Default" , "Test" };
	static std::vector<color> gradient_img_data(400 * 30, color(0));
	static std::vector<Gradient> gradients = { uf_default, standard_muted, volcano_under_a_glacier, volcano_under_a_glacier2, jet, CBR_coldhot, Gradient(), test };
	cf.gradient = gradients[currentGradientIndex];
	ImGui::Combo("Gradent", &currentGradientIndex, items, IM_ARRAYSIZE(items));
	ImGui::DragInt("Gradient Nodes", &colorNodeCount, 1, 12);
	if (ImGui::Button("Test Gradient"))
	{
		cf.gradient = Gradient();
		// grad.printNodes();
		// grad.printFine();
		glDeleteTextures(1, &gradientTextureID);
		gradientImg = cf.gradient.getGradientImg(400, 20);
		makeTexture(gradientTextureID, 400, 20, gradientImg);
		needGradientTexture = false;
	}
	ImGui::Image((void*)(intptr_t)gradientTextureID, ImVec2(400, 20));
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}