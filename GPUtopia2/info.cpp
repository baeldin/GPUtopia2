#include "info.h"

// Helper function to compute statistics
void ComputeStatistics(const std::vector<float> timings, float& average, float& median, float& stddev)
{
	if (timings.empty())
	{
		average = median = stddev = 0.0f;
		return;
	}

	// Average
	float sum = std::accumulate(timings.begin(), timings.end(), 0.0f);
	average = sum / static_cast<float>(timings.size());

	// Median (compute using a sorted copy)
	std::vector<float> sorted = timings;
	std::sort(sorted.begin(), sorted.end());
	size_t n = sorted.size();
	if (n % 2 == 0)
	{
		median = (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0f;
	}
	else
	{
		median = sorted[n / 2];
	}

	// Standard deviation
	float variance = 0.0f;
	for (float t : timings)
	{
		variance += (t - average) * (t - average);
	}
	variance /= static_cast<float>(timings.size());
	stddev = std::sqrt(variance);
}

// Helper function to create binned histogram data
std::vector<float> CreateBinnedHistogram(const std::vector<float> timings, int numBins, float& outMin, float& outMax)
{
	std::vector<float> bins(numBins, 0.0f);
	if (timings.empty())
		return bins;

	outMin = *std::min_element(timings.begin(), timings.end());
	outMax = *std::max_element(timings.begin(), timings.end());

	// Avoid division by zero if all values are equal.
	float range = (outMax - outMin);
	if (range <= 0.0f)
	{
		// In this case, put all counts into the first bin.
		bins[0] = static_cast<float>(timings.size());
		return bins;
	}

	float binWidth = range / numBins;
	for (float t : timings)
	{
		int bin = static_cast<int>((t - outMin) / binWidth);
		if (bin >= numBins) // Just in case t == outMax
			bin = numBins - 1;
		bins[bin] += 1.0f;
	}
	return bins;
}

void infoWindow(clFractal& cf, fractalNavigationParameters& nav, ImFont* font_mono)
{
	//static ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Info");
	// 1. Progress bar (execution progress)
	{
		float progress = static_cast<float>(      cf.image.current_sample_count ) / cf.image.target_sample_count;
		ImGui::Text("Execution progress: %d / %d",cf.image.current_sample_count , cf.image.target_sample_count);
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	}
	if (ImGui::TreeNode("Benchmarking"))
	{
		// 2. Line plot for all timings
		if (!cf.timings.empty())
		{
			// Compute min and max for a proper scale.
			float min_time, max_time;
			{
				std::lock_guard guard(timingMutex);
				min_time = *std::min_element(cf.timings.begin(), cf.timings.end());
				max_time = *std::max_element(cf.timings.begin(), cf.timings.end());
			}
			ImGui::Text("Line Plot of Timings:");
			ImGui::PlotLines("Timings", (float*)cf.timings.data(), static_cast<int>(cf.timings.size()),
				0, "Time (s)", 0.f, max_time, ImVec2(0, 100), sizeof(float));
		}
		else
		{
			ImGui::Text("No timing data available for line plot.");
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// 3. Histogram of timings (binned frequency distribution)
		if (!cf.timings.empty())
		{
			int numBins = 50;  // You can adjust the number of bins as needed
			float dataMin = 0.0f, dataMax = 0.0f;
			std::vector<float> histogramBins;
			{
				std::lock_guard guard(timingMutex);
				histogramBins = CreateBinnedHistogram(cf.timings, numBins, dataMin, dataMax);
			}

			// Find the maximum count for scaling
			if (histogramBins.size() > 0)
			{
				float maxCount = *std::max_element(histogramBins.begin(), histogramBins.end());
				ImGui::Text("Histogram of cf.timings (binned):");
				ImGui::PlotHistogram("Timing Frequency", histogramBins.data(), static_cast<int>(histogramBins.size()),
					0, "Frequency", 0, maxCount, ImVec2(0, 80), sizeof(float));
			}
			float average = 0.0f, median = 0.0f, stddev = 0.0f;
			{
				std::lock_guard guard(timingMutex);
				ComputeStatistics(cf.timings, average, median, stddev);
			}
			ImGui::Text("Average: %.6f s", average);
			ImGui::Text("Median: %.6f s", median);
			ImGui::Text("Std. Dev.: %.6f s", stddev);
		}
		else
		{
			ImGui::Text("No timing data available for histogram.");
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// 4. Export statistics and timings to a text file
		if (ImGui::Button("Export Statistics"))
		{
			if (!cf.timings.empty())
			{
				float average = 0.0f, median = 0.0f, stddev = 0.0f;
				{
					std::lock_guard guard(timingMutex);
					ComputeStatistics(cf.timings, average, median, stddev);
				}
				std::string fileName;
				bool success = false;
				saveFileDialog(fileName, success);
				if (success)
				{
					// Write the statistics to a text file.
					std::ofstream ofs(fileName);
					if (ofs.is_open())
					{
						ofs << "Timings Statistics\n";
						ofs << "------------------\n";
						ofs << "Average: " << average << " s\n";
						ofs << "Median: " << median << " s\n";
						ofs << "Standard Deviation: " << stddev << " s\n";
						ofs << "\nIndividual Timings:\n";
						for (size_t i = 0; i < cf.timings.size(); ++i)
						{
							ofs << "Execution " << (i + 1) << ": " << cf.timings[i] << " s\n";
						}
						ofs.close();
					}
					else
					{
						// Optionally report an error if file cannot be opened.
						ImGui::OpenPopup("Error");
					}
				}
				else
				{
					ImGui::OpenPopup("Cannot save file.");
				}
			}
			else
			{
				// Optionally inform the user that no data is available.
				ImGui::OpenPopup("No Data");
			}
		}
		ImGui::TreePop();
	}
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