#include "cl_error_window.h"

void show_cl_error_window(clFractal& cf, const clCore& cc)
{
	ImGui::Begin("OpenCL Error");
	static std::istringstream iss;
	iss = std::istringstream(cc.kernelBuildLog);
	for (std::string line; std::getline(iss, line); )
	{
		ImGui::Text(line.c_str());
	}
	
	//ImGui::BulletText(
	//	"OpenCL compile error: %d",
	//	cc.compileError
	//);
	//static char* text = (char*)" ";
	//static uint32_t txtLen = 0;
	//if (ImGui::Button("reload")) {
	//	text = (char*)cc.kernelBuildLog.data();
	//	std::cout << text << "\n";
	//}
	//static ImGuiInputTextFlags flags = 1 << 14; 
	//ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 60), flags);
	ImGui::End();
}