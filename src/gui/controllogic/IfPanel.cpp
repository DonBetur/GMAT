//$Header: 
//------------------------------------------------------------------------------
//                              IfPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------

#include "IfPanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(IfPanel, GmatPanel)
    EVT_GRID_CELL_LEFT_CLICK(IfPanel::OnCellLeftClick)
//    EVT_GRID_CELL_RIGHT_CLICK(IfPanel::OnCellRightClick)
    EVT_GRID_CELL_LEFT_DCLICK(IfPanel::OnCellDoubleLeftClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// IfPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
IfPanel::IfPanel( wxWindow *parent ) : GmatPanel(parent)
{
   Create();
   Show();
}

IfPanel::~IfPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void IfPanel::Create()
{
    Setup(this);    
}

void IfPanel::Setup( wxWindow *parent)
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    conditionGrid = new wxGrid( parent, ID_GRID, wxDefaultPosition, 
                    wxSize(454,238), wxWANTS_CHARS );
    conditionGrid->CreateGrid( 10, 4, wxGrid::wxGridSelectCells );
    conditionGrid->SetRowLabelSize(0);
    conditionGrid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    conditionGrid->EnableEditing(false);
        
    conditionGrid->SetColLabelValue(0, _T(""));
    conditionGrid->SetColSize(0, 60);
    conditionGrid->SetColLabelValue(1, _T("LHS"));
    conditionGrid->SetColSize(1, 165);
    conditionGrid->SetColLabelValue(2, _T("Condition"));
    conditionGrid->SetColSize(2, 60);
    conditionGrid->SetColLabelValue(3, _T("RHS"));
    conditionGrid->SetColSize(3, 165);
    conditionGrid->SetCellValue(0, 0, "If");
        
    item0->Add( conditionGrid, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(item0, 0, wxGROW, 5);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void IfPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void IfPanel::SaveData()
{
}

//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void IfPanel::OnCellLeftClick(wxGridEvent& event)
{
    int row = event.GetRow();
    
    if (event.GetEventObject() == conditionGrid)
    {
        conditionGrid->SelectRow(row);
    }
}

//------------------------------------------------------------------------------
// void OnCellDoubleLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void IfPanel::OnCellDoubleLeftClick(wxGridEvent& event)
{
   if (event.GetEventObject() == conditionGrid)
   {
      int row = event.GetRow();
      int col = event.GetCol();
   
      if ( (col == 1) || (col == 3))
      {
         // show dialog to select parameter
         ParameterSelectDialog paramDlg(this);
         paramDlg.ShowModal();

         if (paramDlg.IsParamSelected())
         {
            wxString newParamName = paramDlg.GetParamName();
            conditionGrid->SetCellValue(row, col, newParamName);
            theApplyButton->Enable(true);
         }
      }
      else if (col == 2)
      {
         wxString strArray[] =
         {
            wxT("="),
            wxT(">"),
            wxT("<"),
            wxT(">="),
            wxT("<="),
            wxT("!=")
         };
         
         wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
                                        _T("IfConditionDialog"), 6, strArray);
         dialog.SetSelection(0);

         if (dialog.ShowModal() == wxID_OK)
         {
            if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
            {
               conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
               theApplyButton->Enable(true);
            }
         }   
      }
   }
}

//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
//void IfPanel::OnCellRightClick(wxGridEvent& event)
//{
//    int row = event.GetRow();
//    int col = event.GetCol();
//    
//    if (event.GetEventObject() == conditionGrid)
//    {
//      // AND/OR/ENDIF 
//      if (col == 0)
//      {
//         if (row > 0)
//         {
//            wxString strArray[] =
//            {
//               wxT("&"),
//               wxT("|"),
//               wxT("~"),
//               wxT("END")
//            };
//         
//            wxSingleChoiceDialog dialog(this, _T("Condition: \n"),
//                                        _T("ConditionDialog"), 3, strArray);
//            dialog.SetSelection(0);
//
//            if (dialog.ShowModal() == wxID_OK)
//            {
//              if (dialog.GetStringSelection() != conditionGrid->GetCellValue(row, col))
//              {
//                conditionGrid->SetCellValue(row, col, dialog.GetStringSelection());
//                theApplyButton->Enable(true);
//              }   
//            }   
//         }   
//      }
//    }       
//}


