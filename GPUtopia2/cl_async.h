#pragma once

#include "cl_handler.h"
#include "cl_core.h"
// run openCL kernel without locking up the Ui

class asyncOpenCL
{
public:
	bool clIsRunning = false;
	bool haltExecution = false;
	void clShepherd(const clFractal& cf, const clCore& co);
};