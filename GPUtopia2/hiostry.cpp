#include "history.h"

void undo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<clFractalMinimal>& history, int* historyIndex)
{
	if (*historyIndex > 0)
	{
		*historyIndex = *historyIndex - 1;
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.coloringCLFragmentFile != cf_old.coloringCLFragmentFile)
		{
			cf.makeCLCode();
			// cf.buildKernel = true;
			cc.resetCore();
			cc.compileFractalKernel(cf.fullCLcode);
		}
		cf.image.resetStatus();
		cf.status.runKernel = true;
		{
			std::lock_guard guard(timingMutex);
			cf.timings.erase(cf.timings.begin(), cf.timings.end());
		}	cf.status.done = false;
		cf_old = cf;
		std::cout << "History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		std::cout << "History is already at index " << *historyIndex << ", no further steps to be undone.\n";
}

void redo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<clFractalMinimal>& history, int* historyIndex)
{
	if (*historyIndex < history.size() - 1)
	{
		*historyIndex = *historyIndex + 1;
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.coloringCLFragmentFile != cf_old.coloringCLFragmentFile)
		{
			cf.makeCLCode();
			cc.resetCore();
			cc.compileFractalKernel(cf.fullCLcode);
		}
		cf.image.resetStatus();
		cf.status.runKernel = true;
		{
			std::lock_guard guard(timingMutex);
			cf.timings.erase(cf.timings.begin(), cf.timings.end());
		}
		cf.status.done = false;
		cf_old = cf;
		std::cout << "History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		std::cout << "History index is already at " << *historyIndex << " and length of history vector is only " << history.size() << ", nothing to be redone.\n";
}

void popHistory(std::vector<clFractalMinimal>& history, int* historyIndex)
{
	std::cout << "We are not at the end of history, but the fractal was changed:\n";
	const int historyEnd = history.size();
	for (int ii = *historyIndex + 1; ii < historyEnd; ii++)
	{
		std::cout << "Popping history[" << ii << "]\n";
		history.pop_back();
	}
	historyIndex--;
	std::cout << "##################################\nHistory index = " << historyIndex << " and length of history vector is " << history.size() << "\n";
}