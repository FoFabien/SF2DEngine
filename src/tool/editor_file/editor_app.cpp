#include "editor_app.hpp"
#include "editor.hpp"
#include "../../version.h"
#include <string>

// Execute our app
IMPLEMENT_APP(EditorApp);

bool EditorApp::OnInit()
{
    std::string app_name = "EG Engine Light v" + std::string(AutoVersion::FULLVERSION_STRING) + " " + std::string(AutoVersion::STATUS) + " - File Editor";
    Editor *frm = new Editor(wxString(app_name.c_str(),wxConvUTF8));
    frm->Show();
    return true;
}
