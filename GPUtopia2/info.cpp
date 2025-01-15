#include "info.h"

void infoWindow(clFractal& cf, fractalNavigationParameters& nav)
{
	static ImGuiIO& io = ImGui::GetIO();
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
	if (ImGui::TreeNode("Fractal Status"))
	{
		ImGui::BulletText(
		"Fractal Status:\n"
		"- Render kernel build requested = %d \n"
		"- Render kernel run requested = %d\n"
		"- Render kernel running = %d\n"
		"- Image kernel run requested = %d\n"
		"- Image kernel running = %d\n"
		"- Update Image = %d\n"
		"- Running = %d\n"
		"- Done = %d\n\n"
		"Image Status:\n"
		"- Image targetQuality= %d\n"
		"- Image currentQuality = %d\n"
		"- Image current_sample_count = %d\n"
		"- Image next_update_sample_count = %d\n"
		"- Image target_sample_count = %d",
		cf.buildKernel,
		cf.status.runKernel,
		cf.status.kernelRunning,
		cf.status.runImgKernel,
		cf.status.imgKernelRunning,
		cf.status.updateImage,
		cf.running(),
		cf.status.done,
		cf.image.targetQuality,
		cf.image.currentQuality,
		cf.image.current_sample_count,
		cf.image.next_update_sample_count,
		cf.image.target_sample_count
		);
		ImGui::TreePop();
	}
	// PROGRESS BAR
	static float progress = 0.f;
	progress = (float)cf.image.current_sample_count / (float)cf.image.target_sample_count;
	ImGui::Text("Render Progress:");
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	if (ImGui::TreeNode("Fractal Navigation"))
	{
		ImGui::BulletText(
			"General:\n"
			"- dragStart.x = %d\n"
			"- dragStart.y = %d\n"
			"- dragOffset.x = %d\n"
			"- dragOffset.y = %d\n"
			"- coursorPos.x = %d\n"
			"- coursorPos.y = %d\n"
			"- coursorPosIO.x = %d\n"
			"- coursorPosIO.y = %d\n"
			"Panning:\n"
			"- draggingCenter = %d \n"
			"- centerOffset.x = %f\n"
			"- centerOffset.y = %f\n"
			"Panning:\n"
			"- draggingZoom = %d \n"
			"- imgDisplayCenter.x = %d \n"
			"- imgDisplayCenter.y = %d \n"
			"- dragZoomFactor = %f \n",
			// genral
			nav.dragStart.x,
			nav.dragStart.y,
			nav.dragOffset.x,
			nav.dragOffset.y,
			nav.coursorPos.x,
			nav.coursorPos.y,
			io.MousePos.x,
			io.MousePos.y,
			// pan
			nav.draggingCenter,
			nav.centerOffset.x,
			nav.centerOffset.y,
			// zoom
			nav.draggingZoom,
			nav.imgDisplayCenter.x,
			nav.imgDisplayCenter.y,
			nav.dragZoomFactor
		);
		ImGui::TreePop();
	}


	ImGui::End();
}