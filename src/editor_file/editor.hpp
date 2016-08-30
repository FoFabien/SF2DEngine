#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/grid.h>
#include <wx/listbox.h>
#include <wx/dirdlg.h>
#include <wx/dir.h>

enum
{
    ID_BTN_AS_NEW,
    ID_BTN_AS_LOAD,
    ID_BTN_AS_SAVE,
    ID_BTN_AS_SAVEAS,

    ID_BTN_D_NEW,
    ID_BTN_D_LOAD,
    ID_BTN_D_SAVE,
    ID_BTN_D_SAVEAS,

    ID_BTN_DP_LOAD,
    ID_BTN_DP_BUILD,
    ID_BTN_DP_IMPORT,
    ID_BTN_DP_EXPORT
};

class Editor : public wxFrame
{
    public:
        Editor(const wxString& title);
        virtual ~Editor();
    private:
        wxPanel* mainsizer;

        // animated sprite
        wxTextCtrl *as_file, *as_w, *as_h, *as_fc, *as_ft, *as_sprite;

        void as_new(wxCommandEvent &event);
        void as_load(wxCommandEvent &event);
        void as_save(wxCommandEvent &event);
        void as_saveas(wxCommandEvent &event);
        bool as_stdsave(std::string path);

        // drawable
        wxTextCtrl *d_file, *d_draw;
        wxChoice *d_type;

        void d_new(wxCommandEvent &event);
        void d_load(wxCommandEvent &event);
        void d_save(wxCommandEvent &event);
        void d_saveas(wxCommandEvent &event);
        bool d_stdsave(std::string path);

        // data pack
        wxStaticText *dp_folder;
        wxTextCtrl *dp_list;

        void dp_load(wxCommandEvent &event);
        void dp_build(wxCommandEvent &event);
        void dp_import(wxCommandEvent &event);
        void dp_export(wxCommandEvent &event);

        DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
