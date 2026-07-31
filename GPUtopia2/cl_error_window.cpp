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

ImVec4 operator*(const float lhs, const ImVec4 rhs)
{
	return ImVec4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
}

void showErrorLogWindow(clFractal& cf, const clLayerState& ls, ImFont* font_mono, bool hasError)
{
	static bool undoStyleChange = false;
	// Base colors
	ImVec4 colorErr = { 0.9f, 0.5f, 0.4f, 1.0f }; // red
	ImVec4 colorOk = { 0.3f, 0.8f, 0.5f, 1.0f }; // green
	ImVec4 colorTab;

	// Flash intensity, oscillates between 0 and 1
	float time = ImGui::GetTime();
	float flashIntensity = 0.5f * (std::sin(time * 5.0f) + 1.0f); // speed = 5.0, tweak as needed

	if (hasError) {
		// Interpolate between colorOk and colorErr
		colorTab.x = colorOk.x * (1.0f - flashIntensity) + colorErr.x * flashIntensity;
		colorTab.y = colorOk.y * (1.0f - flashIntensity) + colorErr.y * flashIntensity;
		colorTab.z = colorOk.z * (1.0f - flashIntensity) + colorErr.z * flashIntensity;
		colorTab.w = 1.0f;
	}
	else {
		colorTab = colorOk;
	}

	// Apply flashing color to tab states
	ImGui::PushStyleColor(ImGuiCol_Tab, 0.8f * colorTab);
	ImGui::PushStyleColor(ImGuiCol_TabActive, 0.9f * colorTab);
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, 0.7f * colorTab);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, colorTab);
	ImGui::Begin("Error Log");
	ImGui::PushFont(font_mono);
	static std::istringstream issImgKernelBuildLog;
	ImGui::Text("Img Kernel:");
	if (ls.imgKernel.errors.compileError != CL_SUCCESS)
		issImgKernelBuildLog = std::istringstream(ls.imgKernel.buildLog);
	else
		ImGui::TextColored(colorOk, "Img Kernel Build sucessful.");
	if (ls.imgKernel.errors.compileError != CL_SUCCESS)
	{
		ImGui::TextColored(colorErr, "ls.imgKernel.errors.compileError = %d", ls.imgKernel.errors.compileError);
		ImGui::Text("Img Kernel Build Log:");
	}
	if (ls.imgKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "ls.imgKernel.errors.kernelError = %d", ls.imgKernel.errors.kernelError);
	if (ls.imgKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "ls.imgKernel.errors.programError = %d", ls.imgKernel.errors.programError);
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
	if (cf.status.outsideColoringFragmentStatus > 0)
	{
		ImGui::TextColored(colorErr, "Invalid inside coloring fragment file, error %d", cf.status.outsideColoringFragmentStatus);
		ImGui::TextColored(colorErr, parseErrorMessages[cf.status.outsideColoringFragmentStatus].c_str());
	}
	else
		ImGui::TextColored(colorOk, parseErrorMessages[0].c_str());
	if (ls.fractalKernel.errors.compileError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "ls.fractalKernel.errors.compileError = %d", ls.fractalKernel.errors.compileError);
	if (ls.fractalKernel.errors.kernelError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "ls.fractalKernel.errors.kernelError = %d", ls.fractalKernel.errors.kernelError);
	if (ls.fractalKernel.errors.programError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "ls.fractalKernel.errors.programError = %d", ls.fractalKernel.errors.programError);
	if (ls.fractalKernel.errors.parseError != CL_SUCCESS)
		ImGui::TextColored(colorErr, "Error parsing CL code, no build was attempted. Please see above.");
	static std::istringstream issFractalKernelBuildLog;
	ImGui::Text("Fractal Kernel:");
	if (ls.fractalKernel.errors.compileError != CL_SUCCESS)
	{
		issFractalKernelBuildLog = std::istringstream(ls.fractalKernel.buildLog);
		ImGui::Text("Fractal Kernel Build Log:");
	}
	if (ls.fractalKernel.errors.sum() == 0)
		ImGui::TextColored(colorOk, "Fractal Kernel Build sucessful.");
	for (std::string line; std::getline(issFractalKernelBuildLog, line); )
	{
		ImGui::Text(line.c_str());
	}
	ImGui::PopFont();
	ImGui::End();
	ImGui::PopStyleColor(4);
}