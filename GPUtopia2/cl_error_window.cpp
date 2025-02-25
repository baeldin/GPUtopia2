#include "cl_error_window.h"

const std::vector<std::string> parseErrorMessages =
{
	"Fragment Sanity check OK.",
	"Parameter block not found, missing indicator \"__parameters:\"",
	"Init block not found, missing indicator \"__init:\"",
	"Loop block not found, missing indicator \"__loop:\"",
	"Function block not found, missing indicator \"__functions:\"",
	"Bailout block not found, missing indicator \"__bailout:\"\nConfirm that this is a fractal and not a coloring fragment file!",
	"Final block not found, missing indicator \"__final:\"\nConfirm that this is a coloring and not a fractal fragment file!",
	"Unbalanced parentheses in declaration or multi-line declaration, which is currenly not supported by the fragment parser."
};

void showErrorLogWindow(clFractal& cf, const clCore& cc, ImFont* font_mono)
{
	ImGui::Begin("Error Log");
	ImGui::PushFont(font_mono);
	ImVec4 colorErr = { 0.9f, 0.5f, 0.4f, 1.0f }; // red
	ImVec4 colorOk = { 0.0f, 0.9f, 0.0f, 1.0 }; // green
	static std::istringstream issImgKernelBuildLog;
	if (cc.imgKernel.errors.compileError != CL_SUCCESS)
		issImgKernelBuildLog = std::istringstream(cc.imgKernel.buildLog);
	else
		ImGui::TextColored(colorOk, "Img Kernel Build sucessful.");
	if (cc.imgKernel.errors.compileError != CL_SUCCESS)
	{
		ImGui::TextColored(colorErr, "cc.imgKernel.errors.compileError = %d", cc.imgKernel.errors.compileError);
		ImGui::Text("Img Kernel Build Log:");
	}
	if (cc.imgKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "cc.imgKernel.errors.kernelError = %d", cc.imgKernel.errors.kernelError);
	if (cc.imgKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "cc.imgKernel.errors.programError = %d", cc.imgKernel.errors.programError);
	for (std::string line; std::getline(issImgKernelBuildLog, line); )
	{
		ImGui::Text(line.c_str());
	}
	ImGui::Text("Fractal fragment file parsing:");
	if (cf.status.fractalFragmentStatus > 0)
	{
		ImGui::TextColored(colorErr, "Invalid fractal fragment file, error %d", cf.status.fractalFragmentStatus);
		ImGui::TextColored(colorErr, parseErrorMessages[cf.status.fractalFragmentStatus].c_str());
	}
	else
		ImGui::TextColored(colorOk, parseErrorMessages[0].c_str());
	ImGui::Text("Outside coloring fragment file parsing:");
	if (cf.status.insideColoringFragmentStatus > 0)
	{
		ImGui::TextColored(colorErr, "Invalid outside coloring fragment file, error %d", cf.status.insideColoringFragmentStatus);
		ImGui::TextColored(colorErr, parseErrorMessages[cf.status.insideColoringFragmentStatus].c_str());
	}
	else
		ImGui::TextColored(colorOk, parseErrorMessages[0].c_str());
	ImGui::Text("Inside coloring fragment file parsing:");
	if (cf.status.outsideColoringFragment_status > 0)
	{
		ImGui::TextColored(colorErr, "Invalid inside coloring fragment file, error %d", cf.status.outsideColoringFragment_status);
		ImGui::TextColored(colorErr, parseErrorMessages[cf.status.outsideColoringFragment_status].c_str());
	}
	else
		ImGui::TextColored(colorOk, parseErrorMessages[0].c_str());
	if (cc.fractalKernel.errors.compileError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "cc.fractalKernel.errors.compileError = %d", cc.fractalKernel.errors.compileError);
	if (cc.fractalKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "cc.fractalKernel.errors.kernelError = %d", cc.fractalKernel.errors.kernelError);
	if (cc.fractalKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "cc.fractalKernel.errors.programError = %d", cc.fractalKernel.errors.programError);
	if (cc.fractalKernel.errors.parseError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "Error parsing CL code, no build was attempted. Please see above.");
	static std::istringstream issFractalKernelBuildLog;
	if (cc.fractalKernel.errors.compileError != CL_SUCCESS)
	{
		issFractalKernelBuildLog = std::istringstream(cc.fractalKernel.buildLog);
		ImGui::Text("Fractal Kernel Build Log:");
	}
	if (cc.fractalKernel.errors.sum() == 0)
		ImGui::TextColored(colorOk, "Fractal Kernel Build sucessful.");
	for (std::string line; std::getline(issFractalKernelBuildLog, line); )
	{
		ImGui::Text(line.c_str());
	}
	ImGui::PopFont();
	ImGui::End();
}