#include "flameRenderSettings.h"

void flameRenderSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Flame Render Settings");
	ImGui::InputFloat("Brightness", &cf.flameRenderSettings.x, 0.1f, 10.f);
	ImGui::InputFloat("Gamma", &cf.flameRenderSettings.y, 0.1f, 10.f);
	ImGui::InputFloat("Vibrancy", &cf.flameRenderSettings.z, 0.1f, 10.f);
	ImGui::InputInt("Point Selection", &cf.flamePointSelection, 0, 2);
	ImGui::InputInt("Warmup", &cf.flameWarmup, 0, 2);
	ImGui::End();
}