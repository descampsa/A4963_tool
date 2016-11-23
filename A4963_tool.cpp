#include "A4963_tool.h"


RunOptionsPanel::RunOptionsPanel(A4963Tool *parent):
	wxPanel(parent, wxID_ANY), mParent(parent), mConfig(parent->config())
{
	mRunButton=new wxButton(this, wxID_ANY);
	mRunButton->Bind(wxEVT_BUTTON, &RunOptionsPanel::runButton, this);
	mRunButton->SetLabel(mConfig.isRunning()?wxT("Stop"):wxT("Run"));
	
	mBackwardCheckBox=new wxCheckBox(this, wxID_ANY, wxT("Backward direction"));
	mBackwardCheckBox->Bind(wxEVT_CHECKBOX, &RunOptionsPanel::backwardCheckBox, this);
	mBackwardCheckBox->SetValue(mConfig.runningDirection()==BACKWARD);
	
	mStopOnFailCheckBox=new wxCheckBox(this, wxID_ANY, wxT("Stop on fail"));
	mStopOnFailCheckBox->Bind(wxEVT_CHECKBOX, &RunOptionsPanel::stopOnFailCheckBox, this);
	mStopOnFailCheckBox->SetValue(mConfig.stopOnFail());
	
	mRestartCheckBox=new wxCheckBox(this, wxID_ANY, wxT("Restart when loss of sync"));
	mRestartCheckBox->Bind(wxEVT_CHECKBOX, &RunOptionsPanel::restartCheckBox, this);
	mRestartCheckBox->SetValue(mConfig.restartOnSyncLoss());
	
	mBrakeCheckBox=new wxCheckBox(this, wxID_ANY, wxT("Brake at stop"));
	mBrakeCheckBox->Bind(wxEVT_CHECKBOX, &RunOptionsPanel::brakeCheckBox, this);
	mBrakeCheckBox->SetValue(mConfig.brake());
	
	wxString modeName[4]={wxT("Indirect"), wxT("Direct"), wxT("Speed"), wxT("Current")};
	mModeChoice=new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, modeName),
	mModeChoice->Bind(wxEVT_CHOICE, &RunOptionsPanel::modeChoice, this);
	mModeChoice->SetSelection(mConfig.runningMode());
	
	wxStaticBoxSizer *run_options_sizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Run options"));
	
	run_options_sizer->Add(mRunButton, 0, wxALIGN_CENTER, 0);
	run_options_sizer->Add(mBackwardCheckBox, 0, wxALIGN_CENTER, 0);
	run_options_sizer->Add(mStopOnFailCheckBox, 0, wxALIGN_CENTER, 0);
	run_options_sizer->Add(mRestartCheckBox, 0, wxALIGN_CENTER, 0);
	run_options_sizer->Add(mBrakeCheckBox, 0, wxALIGN_CENTER, 0);
	run_options_sizer->Add(mModeChoice, 0, wxALIGN_CENTER, 0);
	
	SetSizer(run_options_sizer);
}

RunOptionsPanel::~RunOptionsPanel()
{}

void RunOptionsPanel::runButton(wxEvent& event)
{
	if(mConfig.isRunning())
		mConfig.stop();
	else
		mConfig.run();
	mParent->updateConfig(RUN_ADDRESS);
	mRunButton->SetLabel(mConfig.isRunning()?wxT("Stop"):wxT("Run"));
	mParent->updateStatus();
}

void RunOptionsPanel::backwardCheckBox(wxEvent &event)
{
	mConfig.runningDirection(mBackwardCheckBox->GetValue()?BACKWARD:FORWARD);
	mParent->updateConfig(RUN_ADDRESS);
	mBackwardCheckBox->SetValue(mConfig.runningDirection()==BACKWARD);
	mParent->updateStatus();
}

void RunOptionsPanel::stopOnFailCheckBox(wxEvent &event)
{
	mConfig.stopOnFail(mStopOnFailCheckBox->GetValue());
	mParent->updateConfig(RUN_ADDRESS);
	mStopOnFailCheckBox->SetValue(mConfig.stopOnFail());
	mParent->updateStatus();
}

void RunOptionsPanel::restartCheckBox(wxEvent &event)
{
	mConfig.restartOnSyncLoss(mRestartCheckBox->GetValue());
	mParent->updateConfig(RUN_ADDRESS);
	mRestartCheckBox->SetValue(mConfig.restartOnSyncLoss());
	mParent->updateStatus();
}

void RunOptionsPanel::brakeCheckBox(wxEvent &event)
{
	mConfig.brake(mBrakeCheckBox->GetValue());
	mParent->updateConfig(RUN_ADDRESS);
	mBrakeCheckBox->SetValue(mConfig.brake());
	mParent->updateStatus();
}
void RunOptionsPanel::modeChoice(wxEvent &event)
{
	mConfig.runningMode(RunningMode(mModeChoice->GetSelection()));
	mParent->updateConfig(RUN_ADDRESS);
	mModeChoice->SetSelection(mConfig.runningMode());
	mParent->updateStatus();
}



A4963Tool::A4963Tool(const wxString& title):
	wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(640, 480))
{
	
	mRunOptionsPanel=new RunOptionsPanel(this);
	
	mPWMSlider=new wxSlider(this, wxID_ANY, 0, 0, 255);
	mPWMSlider->Bind(wxEVT_SCROLL_CHANGED, &A4963Tool::PWMSlider, this);
	mPWMSlider->SetValue(mConfig.dutyCycle());
	
	wxBoxSizer *global_sizer = new wxBoxSizer(wxVERTICAL);
	
	global_sizer->Add(mRunOptionsPanel, 0, wxEXPAND, 0);
	global_sizer->Add(mPWMSlider, 0, wxEXPAND, 0);
	
	SetSizer(global_sizer);
	
	CreateStatusBar(1);
	updateConfig();
}

void A4963Tool::PWMSlider(wxEvent &event)
{
	mConfig.dutyCycle(mPWMSlider->GetValue());
	updateConfig(PWM_ADDRESS);
	mPWMSlider->SetValue(mConfig.dutyCycle());
	updateStatus();
}

A4963Config& A4963Tool::config()
{
	return mConfig;
}

void A4963Tool::updateConfig(uint8_t address)
{
	if(!mController.setConfig(mConfig, address) || !mController.getConfig(mConfig, address))
	{
		mConfig=A4963Config();
		mController.disconnect();
	}
	updateStatus();
}

void A4963Tool::updateStatus()
{
	if(mController.isConnected())
	{
		SetStatusText(wxT("Connected"), 0);
	}
	else
	{
		SetStatusText(wxT("Not connected"), 0);
	}
}


