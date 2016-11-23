#pragma once

#include <wx/wx.h>
#include "A4963_config.h"
#include "controller.h"

class A4963Tool;

class RunOptionsPanel:public wxPanel
{
	public:
	
	RunOptionsPanel(A4963Tool *parent);
	~RunOptionsPanel();
	
	private:
	
	wxButton *mRunButton;
	wxCheckBox *mBackwardCheckBox, *mStopOnFailCheckBox, *mRestartCheckBox, *mBrakeCheckBox;
	wxChoice *mModeChoice;
	
	// ui callbacks
	void runButton(wxEvent &event);
	void backwardCheckBox(wxEvent &event);
	void stopOnFailCheckBox(wxEvent &event);
	void restartCheckBox(wxEvent &event);
	void brakeCheckBox(wxEvent &event);
	void modeChoice(wxEvent &event);
	
	A4963Tool *mParent;
	A4963Config &mConfig;
};

class A4963Tool:public wxFrame
{
	public:
	A4963Tool(const wxString& title);
	
	A4963Config& config();
	void updateConfig(uint8_t address=ALL_ADDRESS);
	void updateStatus();
	
	private:
	
	// ui callbacks
	void PWMSlider(wxEvent &event);
	
	wxPanel *mRunOptionsPanel;
	
	wxSlider *mPWMSlider;
	
	A4963Config mConfig;
	Controller mController;
};
