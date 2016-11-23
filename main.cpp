
#include <wx/wx.h>
#include "A4963_tool.h"

class A4963App:public wxApp
{
	public:
	virtual bool OnInit()
	{
		A4963Tool *main_window = new A4963Tool("A4963 config tool");
		main_window->Show(true);
		return true;
	}
};

IMPLEMENT_APP(A4963App)
