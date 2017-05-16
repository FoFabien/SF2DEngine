#include "editor.hpp"
#include "../../engine/mlib/mlib.hpp"
#include "../../version.h"

#include <wx/notebook.h>

#define L_TEXT_X 3
#define L_TEXT_Y 3

#define E_V1 0x31307645
#define ENTITY_VERSION 0x32307645
#define ENGINE_SFML_VERSION "SFML v2.4.1"

BEGIN_EVENT_TABLE(Editor, wxFrame)
    EVT_BUTTON(ID_BTN_AS_NEW, Editor::as_new)
    EVT_BUTTON(ID_BTN_AS_LOAD, Editor::as_load)
    EVT_BUTTON(ID_BTN_AS_SAVE, Editor::as_save)
    EVT_BUTTON(ID_BTN_AS_SAVEAS, Editor::as_saveas)

    EVT_BUTTON(ID_BTN_D_NEW, Editor::d_new)
    EVT_BUTTON(ID_BTN_D_LOAD, Editor::d_load)
    EVT_BUTTON(ID_BTN_D_SAVE, Editor::d_save)
    EVT_BUTTON(ID_BTN_D_SAVEAS, Editor::d_saveas)

    EVT_BUTTON(ID_BTN_DP_LOAD, Editor::dp_load)
    EVT_BUTTON(ID_BTN_DP_BUILD, Editor::dp_build)
    EVT_BUTTON(ID_BTN_DP_IMPORT, Editor::dp_import)
    EVT_BUTTON(ID_BTN_DP_EXPORT, Editor::dp_export)
END_EVENT_TABLE()

Editor::Editor(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 500), wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
{
    wxFrame::SetIcon(wxIcon(_T("MAINICON")));

    Out.setFile("log_file_editor.txt", true);
    Out.setOutput(false, true);
    Out = std::string("EG Engine v") + AutoVersion::STATUS + " " + AutoVersion::FULLVERSION_STRING + "\n";
    Out = std::string("File Editor Build - ") + ENGINE_SFML_VERSION + "\n";

    wxBoxSizer *sizer_vertical = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer_vertical);

    mainsizer = new wxPanel(this);
    sizer_vertical->Add(mainsizer, 10, wxEXPAND, 0);

    wxNotebook *tabs = new wxNotebook(mainsizer, -1, wxPoint(0, 0), wxSize(400, 500), wxNB_TOP | wxNB_MULTILINE, _T("TEST"));
    tabs->AddPage(new wxPanel(tabs), _T("Animated Sprite"));
    tabs->AddPage(new wxPanel(tabs), _T("Drawable"));
    tabs->AddPage(new wxPanel(tabs), _T("Build data pack"));
    tabs->SetSize(GetClientRect());
    tabs->Refresh();

    wxPanel* current_panel;
    // animated sprite tab
    current_panel = (wxPanel*)tabs->GetPage(0);
    new wxButton(current_panel, ID_BTN_AS_NEW, _T("New"), wxPoint(5,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_AS_LOAD, _T("Load"), wxPoint(60,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_AS_SAVE, _T("Save"), wxPoint(115,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_AS_SAVEAS, _T("Save as"), wxPoint(170,0), wxSize(50, 23));

    new wxStaticText(current_panel, -1, _T("File"), wxPoint(5+L_TEXT_X,25+L_TEXT_Y));
    as_file = new wxTextCtrl(current_panel, -1, _T(""), wxPoint(50,25), wxSize(330, 23));

    new wxStaticText(current_panel, -1, _T("Sprite width"), wxPoint(5+L_TEXT_X,50+L_TEXT_Y));
    as_w = new wxTextCtrl(current_panel, -1, _T("0"), wxPoint(100,50), wxSize(50, 23));
    new wxStaticText(current_panel, -1, _T("Sprite height"), wxPoint(155+L_TEXT_X,50+L_TEXT_Y));
    as_h = new wxTextCtrl(current_panel, -1, _T("0"), wxPoint(250,50), wxSize(50, 23));

    new wxStaticText(current_panel, -1, _T("Frame count"), wxPoint(5+L_TEXT_X,75+L_TEXT_Y));
    as_fc = new wxTextCtrl(current_panel, -1, _T("0"), wxPoint(100,75), wxSize(50, 23));
    new wxStaticText(current_panel, -1, _T("Frame time (ms)"), wxPoint(155+L_TEXT_X,75+L_TEXT_Y));
    as_ft = new wxTextCtrl(current_panel, -1, _T("0"), wxPoint(250,75), wxSize(50, 23));

    new wxStaticText(current_panel, -1, _T("Sprite"), wxPoint(5+L_TEXT_X,100+L_TEXT_Y));
    as_sprite = new wxTextCtrl(current_panel, -1, _T(""), wxPoint(50,100), wxSize(180, 23));

    // drawable
    current_panel = (wxPanel*)tabs->GetPage(1);
    new wxButton(current_panel, ID_BTN_D_NEW, _T("New"), wxPoint(5,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_D_LOAD, _T("Load"), wxPoint(60,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_D_SAVE, _T("Save"), wxPoint(115,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_D_SAVEAS, _T("Save as"), wxPoint(170,0), wxSize(50, 23));

    new wxStaticText(current_panel, -1, _T("File"), wxPoint(5+L_TEXT_X,25+L_TEXT_Y));
    d_file = new wxTextCtrl(current_panel, -1, _T(""), wxPoint(50,25), wxSize(330, 23));

    new wxStaticText(current_panel, -1, _T("String"), wxPoint(5+L_TEXT_X,50+L_TEXT_Y));
    d_draw = new wxTextCtrl(current_panel, -1, _T(""), wxPoint(50,50), wxSize(150, 23));

    new wxStaticText(current_panel, -1, _T("Type"), wxPoint(5+L_TEXT_X,75+L_TEXT_Y));
    d_type = new wxChoice(current_panel, -1, wxPoint(50,75), wxSize(150, 23));
    d_type->Freeze();
    d_type->Append(_T("Undefined"));
    d_type->Append(_T("RichText"));
    d_type->Append(_T("sf::Sprite"));
    d_type->Append(_T("Animated Sprite"));
    d_type->Append(_T("sf::RectangleShape"));
    d_type->Append(_T("sf::CircleShape"));
    d_type->Append(_T("sf::Text"));
    d_type->Append(_T("GameText"));
    d_type->Thaw();
    d_type->SetSelection(0);

    // data pack
    current_panel = (wxPanel*)tabs->GetPage(2);
    new wxButton(current_panel, ID_BTN_DP_LOAD, _T("Load folder"), wxPoint(0,0), wxSize(100, 23));
    new wxButton(current_panel, ID_BTN_DP_BUILD, _T("Build list"), wxPoint(105,0), wxSize(100, 23));
    new wxButton(current_panel, ID_BTN_DP_IMPORT, _T("Import"), wxPoint(210,0), wxSize(50, 23));
    new wxButton(current_panel, ID_BTN_DP_EXPORT, _T("Export"), wxPoint(265,0), wxSize(50, 23));
    dp_folder = new wxStaticText(current_panel, -1, _T(""), wxPoint(L_TEXT_X,25+L_TEXT_Y));
    dp_list = new wxTextCtrl(current_panel, -1, _T(""), wxPoint(5, 50), wxSize(375, 375), wxTE_MULTILINE);

    mainsizer->Show();
}

Editor::~Editor()
{

}

void Editor::as_new(wxCommandEvent &event)
{
    if(wxMessageBox(_T("Current data will be cleared. Continue ?"), _T("Confirm"), wxNO) == wxNO)
        return;

    as_file->SetValue(_T(""));
    as_w->SetValue(_T("0"));
    as_h->SetValue(_T("0"));
    as_fc->SetValue(_T("0"));
    as_ft->SetValue(_T("0"));
    as_sprite->SetValue(_T(""));
}

void Editor::as_load(wxCommandEvent &event)
{
    wxFileDialog openFileDialog(this, _("Load animated sprite"), _T(""), _T(""), _T(""), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL) return;

    std::string path = std::string(openFileDialog.GetPath().mb_str());
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if(!f)
    {
        wxMessageBox(_T("Load failed"));
        return;
    }

    char c;
    int32_t tmp32;
    std::string str;
    do
    {
        if(!f.good())
        {
            wxMessageBox(_T("Load failed"));
            return;
        }
        f.read(&c, 1);
        if(c != 0x00) str += c;
    }while(c != 0x00);
    as_sprite->SetValue(wxString(str.c_str(), wxConvUTF8));

    f.read((char*)&tmp32, 4);
    as_w->SetValue(wxString(mlib::int2str(tmp32).c_str(), wxConvUTF8));
    f.read((char*)&tmp32, 4);
    as_h->SetValue(wxString(mlib::int2str(tmp32).c_str(), wxConvUTF8));
    f.read((char*)&tmp32, 4);
    as_fc->SetValue(wxString(mlib::int2str(tmp32).c_str(), wxConvUTF8));
    f.read((char*)&tmp32, 4);
    as_ft->SetValue(wxString(mlib::int2str(tmp32).c_str(), wxConvUTF8));
    as_file->SetValue(wxString(path.c_str(), wxConvUTF8));
}

void Editor::as_save(wxCommandEvent &event)
{
    if(!as_stdsave(std::string(as_file->GetValue().mb_str()))) wxMessageBox(_T("Save failed"));
    else wxMessageBox(_T("Save success"));
}

void Editor::as_saveas(wxCommandEvent &event)
{
    wxFileDialog saveFileDialog(this, _T("Save animated sprite"), _T(""), _T(""), _T(""), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    if(!as_stdsave(std::string(saveFileDialog.GetPath().mb_str())))
    {
        wxMessageBox(_T("Save failed"));
    }
    else
    {
        wxMessageBox(_T("Save success"));
        as_file->SetValue(saveFileDialog.GetPath());
    }
}

bool Editor::as_stdsave(std::string path)
{
    std::ofstream f(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;

    char c;
    int32_t tmp32;
    std::string str = std::string(as_sprite->GetValue().mb_str());
    for(size_t i = 0; i < str.size(); ++i)
        f.write(&(str[i]), 1);
    c = 0;
    f.write(&c, 1);

    str = std::string(as_w->GetValue().mb_str());
    if(!mlib::isNumber(str)) tmp32 = 0;
    else tmp32 = mlib::str2long(str) % 0x100000000;
    f.write((char*)&tmp32, 4);

    str = std::string(as_h->GetValue().mb_str());
    if(!mlib::isNumber(str)) tmp32 = 0;
    else tmp32 = mlib::str2long(str) % 0x100000000;
    f.write((char*)&tmp32, 4);

    str = std::string(as_fc->GetValue().mb_str());
    if(!mlib::isNumber(str)) tmp32 = 0;
    else tmp32 = mlib::str2long(str) % 0x100000000;
    f.write((char*)&tmp32, 4);

    str = std::string(as_ft->GetValue().mb_str());
    if(!mlib::isNumber(str)) tmp32 = 0;
    else tmp32 = mlib::str2long(str) % 0x100000000;
    f.write((char*)&tmp32, 4);

    return true;
}

void Editor::d_new(wxCommandEvent &event)
{
    if(wxMessageBox(_T("Current data will be cleared. Continue ?"), _T("Confirm"), wxNO) == wxNO)
        return;

    d_file->SetValue(_T(""));
    d_draw->SetValue(_T(""));
    d_type->SetSelection(0);
}

void Editor::d_load(wxCommandEvent &event)
{
    wxFileDialog openFileDialog(this, _("Load drawable"), _T(""), _T(""), _T(""), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL) return;

    std::string path = std::string(openFileDialog.GetPath().mb_str());
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if(!f)
    {
        wxMessageBox(_T("Load failed"));
        return;
    }

    char c;
    std::string str;

    f.read(&c, 1);
    d_type->SetSelection(c);

    do
    {
        if(!f.good())
        {
            wxMessageBox(_T("Load failed"));
            return;
        }
        f.read(&c, 1);
        if(c != 0x00) str += c;
    }while(c != 0x00);
    d_draw->SetValue(wxString(str.c_str(), wxConvUTF8));

    d_file->SetValue(openFileDialog.GetPath());
}

void Editor::d_save(wxCommandEvent &event)
{
    if(!d_stdsave(std::string(d_file->GetValue().mb_str()))) wxMessageBox(_T("Save failed"));
    else wxMessageBox(_T("Save success"));
}

void Editor::d_saveas(wxCommandEvent &event)
{
    wxFileDialog saveFileDialog(this, _T("Save drawable"), _T(""), _T(""), _T(""), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    if(!d_stdsave(std::string(saveFileDialog.GetPath().mb_str())))
    {
        wxMessageBox(_T("Save failed"));
    }
    else
    {
        wxMessageBox(_T("Save success"));
        d_file->SetValue(saveFileDialog.GetPath());
    }
}

bool Editor::d_stdsave(std::string path)
{
    std::ofstream f(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;

    char c;
    std::string str;
    c = d_type->GetSelection();
    f.write(&c, 1);

    str = std::string(d_draw->GetValue().mb_str());
    for(size_t i = 0; i < str.size(); ++i)
        f.write(&(str[i]), 1);
    c = 0;
    f.write(&c, 1);

    return true;
}

void Editor::dp_load(wxCommandEvent &event)
{
    wxDirDialog dlg(NULL, _T("Choose the base directory"), wxGetCwd(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    size_t size = dlg.GetPath().size()+1;
    dp_folder->SetLabel(dlg.GetPath());
    wxArrayString array;

    wxDir::GetAllFiles(dlg.GetPath(), &array, _T(""), wxDIR_FILES|wxDIR_DIRS);

    dp_list->Freeze();
    dp_list->Clear();
    for(size_t i = 0; i < array.size(); ++i)
    {
        array[i] = array[i].substr(size, array[i].size()-size);
        for(size_t j = 0; j < array[i].size(); ++j)
            if(array[i][j] == '\\') array[i][j] = '/';
        dp_list->AppendText(array[i]);
        if(i < array.size() - 1) dp_list->AppendText(_T("\n"));
    }
    dp_list->Thaw();
}

void Editor::dp_build(wxCommandEvent &event)
{
    wxString cwd = wxGetCwd();
    wxSetWorkingDirectory(dp_folder->GetLabel());

    wxString tmp = dp_list->GetValue();
    wxString out;

    for(size_t i = 0; i < tmp.size(); ++i)
    {
        if(tmp[i] != '\n') out += tmp[i];
        else out += _T(" ");
    }
    wxString command = _T("\"") + cwd + _T("\\pack_builder.exe\" \"") + cwd + _T("\\data.pack\" ") + out;
    long errCount = wxExecute(command, wxEXEC_SYNC);
    wxSetWorkingDirectory(cwd);
    Out = std::string(command.mb_str()) + "\n";
    wxMessageBox(wxString(std::string("Done.\nError count: " + mlib::long2str(errCount)).c_str(), wxConvUTF8));
}

void Editor::dp_import(wxCommandEvent &event)
{
    wxFileDialog openFileDialog(this, _("Import file list"), _T(""), _T(""), _T(""), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(openFileDialog.ShowModal() == wxID_CANCEL) return;

    std::string path = std::string(openFileDialog.GetPath().mb_str());
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if(!f) return;

    char c;
    wxString tmp;
    do
    {
        f.read(&c, 1);
        if(c != '\n' && f.good()) tmp += c;
    }while(c != '\n' && f.good());
    dp_folder->SetLabel(tmp);
    tmp.clear();

    dp_list->Freeze();
    dp_list->Clear();
    while(f.good())
    {
        f.read(&c, 1);
        if(f.good())
        {
            if(c == '\n')
            {
                dp_list->AppendText(tmp);
                dp_list->AppendText(_T("\n"));
                tmp.clear();
            }
            else tmp += c;
        }
    }

    if(!tmp.empty()) dp_list->AppendText(tmp);
    dp_list->Thaw();
    wxMessageBox(_T("List imported"));
}

void Editor::dp_export(wxCommandEvent &event)
{
    wxFileDialog saveFileDialog(this, _T("Export file list"), _T(""), _T(""), _T(""), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    std::string path = std::string(saveFileDialog.GetPath().mb_str());
    std::ofstream f(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return;

    std::string tmp;
    tmp = std::string(dp_folder->GetLabel().mb_str());

    for(size_t i = 0; i < tmp.size(); ++i)
        f.write(&tmp[i], 1);
    f.write("\n", 1);

    tmp = std::string(dp_list->GetValue().mb_str());
    for(size_t i = 0; i < tmp.size(); ++i)
        f.write(&tmp[i], 1);
    wxMessageBox(_T("List exported"));
}
