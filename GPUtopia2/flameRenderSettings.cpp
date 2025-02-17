#include "flameRenderSettings.h"

void flameRenderSettingsWindow(clFractal& cf)
{
	ImGui::Begin("Render Settings");
	ImGui::InputFloat("Brightness", &cf.frs.brightness, 0.1f, 10.f);
	ImGui::InputFloat("Gamma", &cf.frs.gamma, 0.1f, 10.f);
	ImGui::InputFloat("Vibrancy", &cf.frs.vibrancy, 0.1f, 10.f);
	static char* pointSelection[] = { (char*)"All", (char*)"Outside", (char*)"Inside" };
	ImGui::Combo("Point Selection", &cf.flamePointSelection, pointSelection, IM_ARRAYSIZE(pointSelection));
	ImGui::InputInt("Warmup", &cf.flameWarmup, 0, 2);
	ImGui::DragInt("Samples per kernel call", &cf.samples_per_kernel_run, 1, 1, cf.image.target_sample_count);
	ImGui::End();
}