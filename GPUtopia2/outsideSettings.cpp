#include "outsideSettings.h"



void outsideSettingsWindow(clFractal& cf, clCore& cc)
{
	ImGui::Begin("Outside");
	static std::string outsideColoringName = "by_iteration";
	if (ImGui::Button("Load Coloring Algorithm"))
	{
		bool success = false;
		std::string newOutsideColoringCLFragment;
		openFileDialog(newOutsideColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.outsideColoringCLFragmentFileUi = newOutsideColoringCLFragment;
			std::filesystem::path p(newOutsideColoringCLFragment);
			outsideColoringName = p.stem().string();
		}
	}
	ImGui::Text(outsideColoringName.c_str());
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::SameLine();
	ImGui::Text("Parameters");
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.realParameters)
	{
		ImGui::InputDouble(key.c_str(), &val.first);
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.complexParameters)
	{
		std::string labelR = key + ".R";
		std::string labelI = key + ".I";
		ImGui::InputDouble(labelR.c_str(), &(val.first.x));
		ImGui::InputDouble(labelI.c_str(), &(val.first.y));
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.enumParameters)
	{
		std::vector<const char*> enumLabels;
		for (const auto& label : val.first.labels) {
			enumLabels.push_back(label.c_str());
		}
		ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
	}
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}