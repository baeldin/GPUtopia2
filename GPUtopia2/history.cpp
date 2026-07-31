#include "history.h"

#include "log.h"

void undo(clFractal& cf, clFractal& cf_old, clCore& cc, clLayerState& ls,
	std::vector<clFractalContainer>& history, int* historyIndex)
{
	if (*historyIndex > 0)
	{
		logOut(LogLevel::Trace) << "UNDO: History index = " << *historyIndex << "\n";
		*historyIndex = *historyIndex - 1;
		logOut(LogLevel::Trace) << "UNDO: -History index = " << *historyIndex << "\n";
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.outsideColoringCLFragmentFile != cf_old.outsideColoringCLFragmentFile ||
			cf.insideColoringCLFragmentFile != cf_old.insideColoringCLFragmentFile)
		{
			cf.makeCLCode();
			cc.resetCore(ls);
			cc.compileFractalKernel(ls, cf.fullCLcode);
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
		logOut(LogLevel::Trace) << (cf_old == cf) << "\n";
		logOut(LogLevel::Trace) << "UNDO: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		logOut(LogLevel::Trace) << "UNDO: History is already at index " << *historyIndex << ", no further steps to be undone.\n";
}

void redo(clFractal& cf, clFractal& cf_old, clCore& cc, clLayerState& ls,
	std::vector<clFractalContainer>& history, int* historyIndex)
{
	if (*historyIndex < history.size() - 1)
	{
		logOut(LogLevel::Trace) << "REDO: History index = " << *historyIndex << "\n";
		*historyIndex = *historyIndex + 1;
		logOut(LogLevel::Trace) << "REDO: +History index = " << *historyIndex << "\n";
		cf = clFractal(history[*historyIndex]);
		if (cf.fractalCLFragmentFile != cf_old.fractalCLFragmentFile ||
			cf.outsideColoringCLFragmentFile != cf_old.outsideColoringCLFragmentFile ||
			cf.insideColoringCLFragmentFile != cf_old.insideColoringCLFragmentFile)
		{
			cf.makeCLCode();
			cc.resetCore(ls);
			cc.compileFractalKernel(ls, cf.fullCLcode);
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
		logOut(LogLevel::Trace) << "REDO: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
	}
	else
		logOut(LogLevel::Trace) << "REDO: History index is already at " << *historyIndex << " and length of history vector is only " << history.size() << ", nothing to be redone.\n";
}

void popHistory(std::vector<clFractalContainer>& history, int* historyIndex)
{
	logOut(LogLevel::Trace) << "We are not at the end of history, but the fractal was changed:\n";
	const int historyEnd = history.size();
	for (int ii = *historyIndex + 1; ii < historyEnd; ii++)
	{
		logOut(LogLevel::Trace) << "Popping history[" << ii << "]\n";
		history.pop_back();
	}
	historyIndex--;
	logOut(LogLevel::Trace) << "POP: History index = " << *historyIndex << " and length of history vector is " << history.size() << "\n";
}



