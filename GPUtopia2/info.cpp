#include "info.h"

void infoWindow(clFractal& cf, fractalNavigationParameters& nav, ImFont* font_mono)
{
	static ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Info");
	if (ImGui::TreeNode("Full OpenCL Code"))
	{
		//ImGui::TextWrapped("lolwhat"); // cf.fullCLcode.c_str());
		static char* text = (char*)" ";
		static uint32_t txtLen = 0;
		if (ImGui::Button("reload")) {
			text = cf.fullCLcode.data();
		}
		static ImGuiInputTextFlags flags = 1 << 14; // read only ImGuiInputTextFlags_AllowTabInput;
		ImGui::PushFont(font_mono);
		ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 30), flags);
		ImGui::PopFont();
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
			"- coursorPos.x = %f\n"
			"- coursorPos.y = %f\n"
			"- coursorPosIO.x = %f\n"
			"- coursorPosIO.y = %f\n"
			"- coursorPosInImage.x = %f\n"
			"- coursorPosInImage.y = %f\n"
			"- complexMousePos.x = %f\n"
			"- complexMousePos.y = %f\n"
			"Panning:\n"
			"- draggingCenter = %d \n"
			"- centerOffset.x = %f\n"
			"- centerOffset.y = %f\n"
			"Zooming:\n"
			"- draggingZoom = %d \n"
			"- imgDisplayCenter.x = %d \n"
			"- imgDisplayCenter.y = %d \n"
			"- dragCenterDistance.x = %f\n"
			"- dragCenterDistance.y = %f\n"
			"- dragZoomFactor = %f \n"
			"- newImgCenter.x = %f\n"
			"- newImgCenter.y = %f\n"
			"- nav.newSubplaneCenter.x = %f\n"
			"- nav.newSubplaneCenter.y = %f\n"
			"Rotating:"
			"- nav.mouseToCenterX = %f\n"
			"- nav.mouseToCenterY = %f\n"
			"- nav.mouseStartToCenterX = %f\n"
			"- nav.mouseStartToCenterY = %f\n"
			"- nav.startangle = %f\n"
			"- nav.dragAngle = %f\n",
			// general
			nav.dragStart.x,
			nav.dragStart.y,
			nav.dragOffset.x,
			nav.dragOffset.y,
			nav.coursorPos.x,
			nav.coursorPos.y,
			nav.MousePos.x,
			nav.MousePos.y,
			nav.MousePosInImage.x,
			nav.MousePosInImage.y,
			nav.complexMousePos.x,
			nav.complexMousePos.y,
			// pan
			nav.draggingCenter,
			nav.centerOffset.x,
			nav.centerOffset.y,
			// zoom
			nav.draggingZoom,
			nav.imgDisplayCenter.x,
			nav.imgDisplayCenter.y,
			nav.dragCenterDistance.x,
			nav.dragCenterDistance.y,
			nav.dragZoomFactor,
			nav.newImgCenter.x,
			nav.newImgCenter.y,
			nav.newSubplaneCenter.x,
			nav.newSubplaneCenter.y,
			nav.mouseToCenterX,
			nav.mouseToCenterY,
			nav.mouseStartToCenterX,
			nav.mouseStartToCenterY,
			nav.startAngle * 180.f / CL_M_PI,
			nav.dragAngle * 180.f / CL_M_PI
			);
		ImGui::TreePop();
	}


	ImGui::End();
}