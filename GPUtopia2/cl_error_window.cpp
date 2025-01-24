#include "cl_error_window.h"

void show_cl_error_window(clFractal& cf, const clCore& cc, ImFont* font_mono)
{
	ImGui::Begin("OpenCL Error");
	ImGui::PushFont(font_mono);
	static std::istringstream issImgKernelBuildLog;
	if (cc.imgKernel.errors.compileError != CL_SUCCESS)
		issImgKernelBuildLog = std::istringstream(cc.imgKernel.buildLog);
	else
		ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.0f, 1.0f), "Img Kernel Build sucessful.");
	if (cc.imgKernel.errors.compileError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.imgKernel.errors.compileError = %d", cc.imgKernel.errors.compileError);
	if (cc.imgKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.imgKernel.errors.kernelError = %d", cc.imgKernel.errors.kernelError);
	if (cc.imgKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.imgKernel.errors.programError = %d", cc.imgKernel.errors.programError);
	ImGui::Text("Img Kernel Build Log:");
	for (std::string line; std::getline(issImgKernelBuildLog, line); )
	{
		ImGui::Text(line.c_str());
	}
	if (cc.fractalKernel.errors.compileError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.fractalKernel.errors.compileError = %d", cc.fractalKernel.errors.compileError);
	if (cc.fractalKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.fractalKernel.errors.kernelError = %d", cc.fractalKernel.errors.kernelError);
	if (cc.fractalKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.4f, 1.0f), "cc.fractalKernel.errors.programError = %d", cc.fractalKernel.errors.programError);
	ImGui::Text("Fractal Kernel Build Log:");
	static std::istringstream issFractalKernelBuildLog;
	if (cc.fractalKernel.errors.compileError != CL_SUCCESS)
		issFractalKernelBuildLog = std::istringstream(cc.fractalKernel.buildLog);
	else
		ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.0f, 1.0f), "Fractal Kernel Build sucessful.");
	for (std::string line; std::getline(issFractalKernelBuildLog, line); )
	{
		ImGui::Text(line.c_str());
	}	ImGui::PopFont();
	ImGui::End();
}