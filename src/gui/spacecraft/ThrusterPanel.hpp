//$Header:
//------------------------------------------------------------------------------
//                            ThrusterPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#ifndef ThrusterPanel_hpp
#define ThrusterPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"

#include "gmatdefs.hpp"
#include "Spacecraft.hpp"
#include "Thruster.hpp"

class ThrusterPanel: public wxPanel
{
public:
    ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft,
                  wxButton *theApplyButton);
    ~ThrusterPanel();
    void SaveData();
    void LoadData();
    
private:
    
    void Create();
    
    // Event Handling
    DECLARE_EVENT_TABLE();
    void OnTextChange();
    
    Spacecraft *theSpacecraft;
    Thruster *theThruster;
    
    Integer thrusterCount;
    Integer tankCount;
    Integer coordsysCount;
    
    wxButton *theApplyButton;
    wxButton *addButton;
    wxButton *removeButton;
    wxButton *cCoefButton;
    wxButton *kCoefButton;
    
    wxComboBox *tankComboBox;
    wxComboBox *coordsysComboBox;
    
    wxTextCtrl *XTextCtrl;
    wxTextCtrl *YTextCtrl;
    wxTextCtrl *ZTextCtrl;
    
    wxStaticText *tankStaticText;
    wxStaticText *coordsysStaticText;
    wxStaticText *XStaticText;
    wxStaticText *YStaticText;
    wxStaticText *ZStaticText;
    
    wxListBox *thrusterListBox;
        
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 30250,
        ID_TEXTCTRL,
        ID_LISTBOX,
        ID_BUTTON,
        ID_COMBO,
    };
};
#endif

