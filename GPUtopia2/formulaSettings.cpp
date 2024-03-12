#include "formulaSettings.h"


void formulaSettingsWindow(clFractal& cf)
{
	static char formulaCLFragment[100] = "clFragments/fractalFormulas/gnarl.cl";
	static char coloringCLFragment[100] = "clFragments/coloringAlgorithms/dist.cl";
	ImGui::Begin("Formula");
	ImGui::InputInt("Iterations", &cf.maxIter, 1, 100);
	ImGui::InputFloat("Zoom", &cf.image.zoom, 0.1f, 10.f);
	ImGui::SliderFloat("Center Real", &cf.image.complexSubplane.x, -5.f, 5.f);
	ImGui::SliderFloat("Center Imag", &cf.image.complexSubplane.y, -5.f, 5.f);
	ImGui::SliderInt("Render Quality", &cf.image.quality, 1, 16);
	ImGui::InputText("Formula:", formulaCLFragment, 100);
	ImGui::InputText("Coloring:", coloringCLFragment, 100);
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.fractalCLFragmentFile = std::string(formulaCLFragment);
		cf.coloringCLFragmentFile = std::string(coloringCLFragment);
		cf.makeCLCode();
		cf.rebuildKernel = true;
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
	ImGui::End();
}