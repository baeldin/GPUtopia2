#include "flameRenderSettings.h"

void flameRenderSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Flame Render Settings");
	ImGui::InputFloat("Brightness", &cf.flameRenderSettings.x, 0.1f, 10.f);
	ImGui::InputFloat("Gamma", &cf.flameRenderSettings.y, 0.1f, 10.f);
	ImGui::InputFloat("Vibrancy", &cf.flameRenderSettings.z, 0.1f, 10.f);
	static char* pointSelection[] = { (char*)"All", (char*)"Outside", (char*)"Inside" };
	ImGui::Combo("Point Selection", &cf.flamePointSelection, pointSelection, IM_ARRAYSIZE(pointSelection));
	ImGui::InputInt("Warmup", &cf.flameWarmup, 0, 2);
	ImGui::End();
}