#include "info.h"

void infoWindow(clFractal& cf, fractalNavigationParameters& nav, ImFont* font_mono)
{
	//static ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Info");
	if (ImGui::TreeNode("Full OpenCL Code"))
	{
		// Using a static vector to hold the text buffer
		static std::vector<char> textBuffer;

		if (ImGui::Button("reload"))
		{
			std::stringstream issCode;
			uint32_t lineNumber = 0;
			std::istringstream issCodeRaw(cf.fullCLcode);
			for (std::string line; std::getline(issCodeRaw, line); )
			{
				lineNumber++;
				issCode << std::setw(5) << std::setfill(' ') << lineNumber << " " << line << "\n";
			}
			std::string strCode = issCode.str();

			// Resize buffer to fit new string, including null terminator.
			textBuffer.resize(strCode.size() + 1);
			memcpy(textBuffer.data(), strCode.c_str(), strCode.size() + 1);
		}

		// If textBuffer is empty, initialize it to an empty string.
		if (textBuffer.empty())
			textBuffer.push_back('\0');

		// If you want it to be read-only, add the read-only flag.
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

		ImGui::PushFont(font_mono);
		ImGui::InputTextMultiline("##source", textBuffer.data(), textBuffer.size(),
			ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 30), flags);
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
	if (ImGui::TreeNode("Sample Timings"))
	{
		for (double t : cf.timings)
		{
			const std::string num = std::to_string(t);
			ImGui::Text(num.c_str());
		}
		ImGui::TreePop();
	}
	ImGui::End();
}