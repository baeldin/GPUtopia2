#include "cl_error_window.h"

void show_cl_error_window(clFractal& cf, const clCore& cc, ImFont* font_mono)
{
	ImGui::Begin("OpenCL Error");
	static std::istringstream iss;
	iss = std::istringstream(cc.kernelBuildLog);
	ImGui::PushFont(font_mono);
	for (std::string line; std::getline(iss, line); )
	{
		ImGui::Text(line.c_str());
	}
	ImGui::PopFont();
	ImGui::End();
}