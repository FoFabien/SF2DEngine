#ifndef EDITOR_APP_H_INCLUDED
#define EDITOR_APP_H_INCLUDED

#include <wx/wx.h>

class EditorApp : public wxApp
{
    public:
        virtual bool OnInit();
};

DECLARE_APP(EditorApp);

#endif //EDITOR_APP_H_INCLUDED
