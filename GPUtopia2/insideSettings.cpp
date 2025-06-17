#include "insideSettings.h"



void insideSettingsWindow(clFractal& cf, clCore& cc)
{
	ImGui::Begin("Inside");
	static std::string insideColoringName = "gaussian_integer";
	if (ImGui::Button("Load Coloring Algorithm"))
	{
		bool success = false;
		std::string newInsideColoringCLFragment;
		openFileDialog(newInsideColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.insideColoringCLFragmentFileUi = newInsideColoringCLFragment;
			std::filesystem::path p(newInsideColoringCLFragment);
			insideColoringName = p.stem().string();
		}
	}
	ImGui::Text(insideColoringName.c_str());
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::SameLine();
	ImGui::Text("Parameters");
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.realParameters)
	{
		ImGui::InputDouble(key.c_str(), &val.first);
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.complexParameters)
	{
		std::string labelR = key + ".R";
		std::string labelI = key + ".I";
		ImGui::InputDouble(labelR.c_str(), &(val.first.x));
		ImGui::InputDouble(labelI.c_str(), &(val.first.y));
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.enumParameters)
	{
		std::vector<const char*> enumLabels;
		for (const auto& label : val.first.labels) {
			enumLabels.push_back(label.c_str());
		}
		ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
	}
	ImGui::End();
}