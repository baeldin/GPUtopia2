#include "info.h"

void infoWindow(clFractal& cf)
{
	ImGui::Begin("Info");
	if (ImGui::TreeNode("Full OpenCL Code"))
	{
		//ImGui::TextWrapped("lolwhat"); // cf.fullCLcode.c_str());
		static char* text = (char*)" ";
		if (ImGui::Button("reload")) {
			text = cf.fullCLcode.data();
		}
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
		ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
		ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags, ImGuiInputTextFlags_AllowTabInput);
		ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", &flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
		ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 60), flags);
		ImGui::TreePop();
	}
	bool ret = false;
	ImGui::BulletText(
		"Fractal Status:\n"
		"- Render Kernel Running = %d\n"
		"- Image Kernel Running = %d\n"
		"- Render Kernel Run Requested = %d\n"
		"- Image Kernel Run Requested = %d\n"
		"- Running = %d\n"
		"- Done = %d\n",
		ret,
		cf.status.kernelRunning,
		cf.status.imgKernelRunning,
		cf.status.runKernel,
		cf.status.runImgKernel,
		cf.running(),
		cf.status.done
	);
	// PROGRESS BAR
	static float progress = 0.f;
	progress = (float)cf.image.current_sample_count / (float)cf.image.target_sample_count;
	ImGui::Text("Render Progress:");
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));


	ImGui::End();
}