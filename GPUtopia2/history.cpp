#include "history.h"

void undo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<clFractalContainer>& history, int* historyIndex)
{
	if (*historyIndex > 0)
	{
		std::cout << "UNDO: History index = " << *historyIndex << "\n";
		*historyIndex = *historyIndex - 1;
		std::cout << "UNDO: -History index = " << *historyIndex << "\n";
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.outsideColoringCLFragmentFile != cf_old.outsideColoringCLFragmentFile ||
			cf.insideColoringCLFragmentFile != cf_old.insideColoringCLFragmentFile)
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
		cf.resetCLFragmentQueue();
		cf.status.done = false;
		cf_old = cf;
		std::cout << (cf_old == cf) << "\n";
		std::cout << "UNDO: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		std::cout << "UNDO: History is already at index " << *historyIndex << ", no further steps to be undone.\n";
}

void redo(clFractal& cf, clFractal& cf_old, clCore& cc,
	std::vector<clFractalContainer>& history, int* historyIndex)
{
	if (*historyIndex < history.size() - 1)
	{
		std::cout << "REDO: History index = " << *historyIndex << "\n";
		*historyIndex = *historyIndex + 1;
		std::cout << "REDO: +History index = " << *historyIndex << "\n";
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.outsideColoringCLFragmentFile != cf_old.outsideColoringCLFragmentFile ||
			cf.insideColoringCLFragmentFile != cf_old.insideColoringCLFragmentFile)
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
		cf.resetCLFragmentQueue();
		cf.status.done = false;
		cf_old = cf;
		std::cout << "REDO: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		std::cout << "REDO: History index is already at " << *historyIndex << " and length of history vector is only " << history.size() << ", nothing to be redone.\n";
}

void popHistory(std::vector<clFractalContainer>& history, int* historyIndex)
{
	std::cout << "We are not at the end of history, but the fractal was changed:\n";
	const int historyEnd = history.size();
	for (int ii = *historyIndex + 1; ii < historyEnd; ii++)
	{
		std::cout << "Popping history[" << ii << "]\n";
		history.pop_back();
	}
	historyIndex--;
	std::cout << "POP: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
}



