//------------------------------------------------------------------------------
//                           SolverCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/21
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "gmatwxdefs.hpp"
#include "SolverCreatePanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SolverCreatePanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, SolverCreatePanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, SolverCreatePanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverEventPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
SolverCreatePanel::SolverCreatePanel(wxWindow *parent)
{
    Initialize();
    Setup(this);
    GetData();
}

void SolverCreatePanel::Initialize()
{

}

void SolverCreatePanel::Setup( wxWindow *parent)
{
    /*applyButton->Enable(FALSE);
    
    item0 = new wxBoxSizer( wxVERTICAL );
    item1 = new wxBoxSizer( wxHORIZONTAL );
    item2 = new wxStaticText( parent, ID_TEXT, wxT("Event Name"), wxDefaultPosition, wxDefaultSize, 0 );
    
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4 = new wxButton( parent, ID_BUTTON, wxT("Create Script"), wxDefaultPosition, wxSize(100,-1), 0 );
    item1->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5 = new wxFlexGridSizer( 4, 0, 0 );

    item6 = new wxStaticText( parent, ID_TEXT, wxT("Solver Name"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs7[] = 
    {
        wxT("ComboItem")
    };
    
    item7 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs7, wxCB_DROPDOWN );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item8 = new wxStaticText( parent, ID_TEXT, wxT("Upon Convergence"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs9[] = 
    {
        wxT("ComboItem")
    };
    
    item9 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs9, wxCB_DROPDOWN );
    item5->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    item10 = new wxStaticText( parent, ID_TEXT, wxT("Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs11[] = 
    {
        wxT("ComboItem")
    };
    
    item11 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs11, wxCB_DROPDOWN );
    item5->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item12 = new wxButton( parent, ID_BUTTON, wxT("Apply Corrections"), wxDefaultPosition, wxSize(117,-1), 0 );
    item5->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13 = new wxBoxSizer( wxHORIZONTAL );

    item14 = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item14, 0, wxALIGN_CENTRE|wxALL, 5 );

    item15 = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

    item16 = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

    item17 = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    
    item0->Fit( parent );
    item0->SetSizeHints( parent );*/
}

void SolverCreatePanel::GetData()
{    
    return;
}

void SolverCreatePanel::SetData()
{
    return;
}

void SolverCreatePanel::OnTextUpdate(wxCommandEvent& event)
{
    //applyButton->Enable(TRUE);
}

void SolverCreatePanel::OnButton(wxCommandEvent& event)
{
    /*
    if ( event.GetEventObject() == scriptButton )         
        CreateScript();
    else if ( event.GetEventObject() == okButton )      
    {     
        SetData();
        Close();
    }
    else if ( event.GetEventObject() == applyButton )
        SetData();
    else if ( event.GetEventObject() == cancelButton )
        Close();
    else
        event.Skip();
    */
}
