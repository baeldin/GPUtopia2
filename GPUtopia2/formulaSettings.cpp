#include "formulaSettings.h"



void formulaSettingsWindow(clFractal& cf, clCore& cc)
{
	ImGui::Begin("Formulas");
	static std::string formulaName = "Mandelbrot";
	if (ImGui::Button("Load Formula"))
	{
		bool success = false;
		std::string newFractalCLFragment;
		openFileDialog(newFractalCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.fractalCLFragmentFileUi = newFractalCLFragment;
			std::filesystem::path p(newFractalCLFragment);
			formulaName = p.stem().string();
		}
	}
	ImGui::Text(formulaName.c_str());
	// coloring CL fragment
	static std::string coloringName = "by_iteration";
	if (ImGui::Button("Load Coloring"))
	{
		bool success = false;
		std::string newColoringCLFragment;
		openFileDialog(newColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.coloringCLFragmentFileUi = newColoringCLFragment;
			std::filesystem::path p(newColoringCLFragment);
			coloringName = p.stem().string();
		}
	}
	ImGui::Text(coloringName.c_str());
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load AA diag"))
	{
		cf.fractalCLFragmentFileUi = "clFragments/diag_AA.cl";
		cf.coloringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
	}
	static bool useDouble = cf.useDouble;
	ImGui::Checkbox("Double Precision", &useDouble);
	if (!useDouble == cf.useDouble)
	{
		cf.useDouble = useDouble;
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
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
	static Gradient gradUpdate;
	static int colorNodeCount = 4;
	static int currentGradientIndex = 2;
	static int previousGradientIndex = -1; // make sure that this is different initially
	const char* items[] = { "UF Default", "UF Default Muted", "Volcano Under a Glacier", "Volcano Under a Glacier2", "Jet", "CBR_coldhot", "nice_random", "Default" , "Test"};
	static std::vector<color> gradient_img_data(400 * 30, color(0.f));
	static std::vector<Gradient> gradients = { uf_default, standard_muted, volcano_under_a_glacier, volcano_under_a_glacier2, jet, CBR_coldhot, nice_random, Gradient(), test };
	if (previousGradientIndex != currentGradientIndex)
	{
		gradUpdate = gradients[currentGradientIndex];
		previousGradientIndex = currentGradientIndex;
	}
	ImGui::Combo("Gradent", &currentGradientIndex, items, IM_ARRAYSIZE(items));
	ImGui::DragInt("Gradient Nodes", &colorNodeCount, 1, 12);
	if (ImGui::Button("Test Gradient"))
	{
		//cf.gradient = Gradient();
		gradUpdate = randomGradient(colorNodeCount);
		gradUpdate.print();
		// grad.printNodes();
		// grad.printFine();
	}
	if (cf.gradient != gradUpdate)
	{
		cf.gradient = gradUpdate;
		cf.status.runImgKernel = true;
		glDeleteTextures(1, &gradientTextureID);
		gradientImg = cf.gradient.getGradientImg(400, 20); // cf.gradient.getGradientImg(400, 20);
		makeTexture(gradientTextureID, 400, 20, gradientImg);
		needGradientTexture = false;
	}
	ImGui::Image((void*)(intptr_t)gradientTextureID, ImVec2(400, 20));
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}