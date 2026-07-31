#pragma once

// The three fragment settings windows (formula / inside coloring / outside
// coloring). They share their layout via helpers in settingsPanels.cpp and
// differ only in which fragment file and parameter set they edit.

class clFractal;
class clCore;
struct clLayerState;

void formulaSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls);
void insideSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls);
void outsideSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls);
