//$Header$
//------------------------------------------------------------------------------
//                              ManeuverSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2003/12/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ManeuverSetupPanel_hpp
#define ManeuverSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Burn.hpp"

#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

class ManeuverSetupPanel : public wxPanel
{
public:
    // constructors
    ManeuverSetupPanel( wxWindow *parent);
    
private:
    // member functions
    void CreateManeuver(wxWindow *parent);
    void CreateFinite(wxWindow *parent);

    void AddVector(wxWindow *parent);
    void CartesianVector();
    void SphericalVector();
    void OnVectorChange();
    void UpdateValues();

    void OnScript();
    void OnApply();
    void OnOK();
    void OnCancel();

    // member data
    GuiInterpreter *theGuiInterpreter;
    Command *theManeuver;
    Burn *theBurn;

    wxButton *okButton;
    wxButton *applyButton;
    wxButton *cancelButton;
    wxButton *helpButton;
    wxButton *scriptButton;

    wxComboBox *burnCB;
    wxComboBox *vectorCB;
    wxComboBox *coordCB;
    wxComboBox *satCB;
    
    wxNotebook *mainNotebook;
    wxNotebookSizer * sizer;

    wxPanel *impulsivePanel;
    wxPanel *finitePanel;
    wxPanel *vectorPanel;

    wxStaticText *description1;
    wxStaticText *description2;
    wxStaticText *description3;
    wxStaticText *label1;
    wxStaticText *label2;
    wxStaticText *label3;
    
    wxStaticBox *vectorBox;
    wxStaticBoxSizer *vectorSizer;

    wxTextCtrl *nameField;
    wxTextCtrl *textCtrl1;
    wxTextCtrl *textCtrl2;
    wxTextCtrl *textCtrl3;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_BUTTON_OK = 80000,
        ID_BUTTON_APPLY,
        ID_BUTTON_CANCEL,
        ID_BUTTON_HELP,
        ID_BUTTON_SCRIPT,

        ID_RADIOBUTTON_1,
        ID_RADIOBUTTON_2,
        ID_RADIOBOX,

        ID_COMBO,
        ID_CB_VECTOR,
        ID_CB_COORDINATE,

        ID_NOTEBOOK,
        
        ID_STATIC_ORBIT,
        ID_STATIC_COORD,
        ID_STATIC_VECTOR,

        ID_TEXT,
        ID_TEXTCTRL
    };
};

#endif // ManeuverSetupPanel_hpp
