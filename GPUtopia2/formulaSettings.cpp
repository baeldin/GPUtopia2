#include "formulaSettings.h"



void formulaSettingsWindow(clFractal& cf, clCore& cc)
{
	ImGui::Begin("Formulas");
	// formula CL fragment
	static std::string formulaCLFragment = cf.fractalCLFragmentFile; 
	// formulaCLFragment = "clFragments/fractalFormulas/mandelbrot.cl";
	if (ImGui::Button("Load Formula"))
	{
		openFileDialog(formulaCLFragment);
		std::cout << "Setting formula fragment to " << formulaCLFragment << "\n";
		// cf.fractalCLFragmentFile = formulaCLFragment;
	}
	ImGui::Text("##formulaCLFragment", (char*)formulaCLFragment.c_str());
	// coloring CL fragment
	static std::string coloringCLFragment = cf.coloringCLFragmentFile; 
	if (ImGui::Button("Load Coloring"))
	{
		openFileDialog(coloringCLFragment);
	}
	ImGui::InputText("##coloringCLFragment", (char*)coloringCLFragment.c_str(), 100);
	if (ImGui::Button("Reload all CL Fragments"))
	{
		cf.fractalCLFragmentFile = formulaCLFragment;
		cf.coloringCLFragmentFile = coloringCLFragment;
		cf.makeCLCode();
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load AA diag"))
	{
		cf.fractalCLFragmentFile = "clFragments/diag_AA.cl";
		cf.coloringCLFragmentFile = "clFragments/diag_AA_col.cl";
		cf.makeCLCode();
		cf.buildKernel = true;
		cc.resetCore();
	}
	static bool useDouble = cf.useDouble;
	ImGui::Checkbox("Double Precision", &useDouble);
	if (!useDouble == cf.useDouble)
	{
		cf.useDouble = useDouble;
		cf.makeCLCode();
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
	static Gradient gradOld;
	static int colorNodeCount = 4;
	static int currentGradientIndex = 2;
	static int previousGradientIndex = -1; // make sure that this is different initially
	const char* items[] = { "UF Default", "UF Default Muted", "Volcano Under a Glacier", "Volcano Under a Glacier2", "Jet", "CBR_coldhot", "nice_random", "Default" , "Test"};
	static std::vector<color> gradient_img_data(400 * 30, color(0));
	static std::vector<Gradient> gradients = { uf_default, standard_muted, volcano_under_a_glacier, volcano_under_a_glacier2, jet, CBR_coldhot, nice_random, Gradient(), test };
	cf.gradient = gradients[currentGradientIndex];
	ImGui::Combo("Gradent", &currentGradientIndex, items, IM_ARRAYSIZE(items));
	ImGui::DragInt("Gradient Nodes", &colorNodeCount, 1, 12);
	if (ImGui::Button("Test Gradient"))
	{
		//cf.gradient = Gradient();
		cf.gradient = randomGradient(colorNodeCount);
		// grad.printNodes();
		// grad.printFine();
		glDeleteTextures(1, &gradientTextureID);
		gradientImg = cf.gradient.getGradientImg(400, 20);
		makeTexture(gradientTextureID, 400, 20, gradientImg);
		needGradientTexture = false;
	}
	ImGui::Image((void*)(intptr_t)gradientTextureID, ImVec2(400, 20));
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	if (cf.fractalCLFragmentFile != formulaCLFragment)
	{
		cf.fractalCLFragmentFile = formulaCLFragment;
		cf.makeCLCode();
		cf.buildKernel = true;
		cc.resetCore();
	}
	if (cf.coloringCLFragmentFile != coloringCLFragment)
	{
		cf.coloringCLFragmentFile = coloringCLFragment;
		cf.makeCLCode();
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::End();
}