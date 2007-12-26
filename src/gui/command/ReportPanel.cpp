//$Id$
//------------------------------------------------------------------------------
//                              ReportPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/08/10
/**
 * Implements ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#include "ReportPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "ParameterInfo.hpp"            // for GetDepObjectType()
#include "ReportFile.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REPORT_PANEL 1
//#define DEBUG_REPORT_PANEL_SAVE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ReportPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BUTTON, ReportPanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBOBOX, ReportPanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
    
//------------------------------------------------------------------------------
// ReportPanel(wxWindow *parent, const GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs ReportPanel object.
 *
 * @param <parent> input parent.
 * @param <cmd> input command.
 */
//------------------------------------------------------------------------------
ReportPanel::ReportPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel() entering...\n");
   MessageInterface::ShowMessage("ReportPanel() cmd = " +
                                 cmd->GetTypeName() + "\n");
   #endif
   
   theCommand = cmd;
   mHasReportFileChanged = false;
   mHasParameterChanged = false;
   
   if (theCommand != NULL)
   {
      mObjectTypeList.Add("Spacecraft");
      mObjectTypeList.Add("ImpulsiveBurn");
      Create();
      Show();
      EnableUpdate(false);
   }
}


//------------------------------------------------------------------------------
// ~ReportPanel()
//------------------------------------------------------------------------------
ReportPanel::~ReportPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("ReportFile", mReportFileComboBox);   
}


//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void ReportPanel::Create()
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::Create() entering...\n");
   #endif
   
   Integer bsize = 2; // border size
   wxArrayString emptyList;   
   
   //------------------------------------------------------
   // report file
   //------------------------------------------------------
   wxStaticText *reportFileLabel =
      new wxStaticText(this, -1, wxT("ReportFile: "), wxDefaultPosition,
                       wxDefaultSize, 0);
   
   mReportFileComboBox = theGuiManager->
      GetReportFileComboBox(this, ID_COMBOBOX, wxSize(200, 20));
   
   wxBoxSizer *reportFileSizer = new wxBoxSizer(wxHORIZONTAL);
   reportFileSizer->Add(reportFileLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   reportFileSizer->Add(mReportFileComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // selected parameter list
   //-------------------------------------------------------   
   mSelectedListBox =
      new wxListBox(this, ID_LISTBOX, wxDefaultPosition, wxSize(200, 150),
                    emptyList, wxLB_SINGLE);
   
   mViewButton = new wxButton( this, ID_BUTTON, wxT("View"), 
                                wxDefaultPosition, wxDefaultSize, 0 );
   
   wxStaticBoxSizer *selectedSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Parameter List");
   
   selectedSizer->Add(mSelectedListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(mViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // put in the order
   //-------------------------------------------------------
   wxFlexGridSizer *panelSizer = new wxFlexGridSizer(2, 1);
   panelSizer->Add(reportFileSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   panelSizer->Add(20, 10, 0, wxALIGN_CENTRE|wxALL, bsize);
   panelSizer->Add(selectedSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   //-------------------------------------------------------
   // add to parent sizer
   //-------------------------------------------------------
   theMiddleSizer->Add(panelSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ReportPanel::LoadData()
{
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::LoadData() entering...\n");
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = theCommand;
      
      // Get ReportFile name
      std::string reportFileName = theCommand->GetRefObjectName(Gmat::SUBSCRIBER);
      
      // Get parameters to report
      StringArray parameterList = theCommand->GetRefObjectNameArray(Gmat::PARAMETER);
      mNumParameters = parameterList.size();
      
      if (mNumParameters > 0)
      {      
         Parameter *param;
         wxString paramName;
         
         for (int i=0; i<mNumParameters; i++)
         {
            param = theGuiInterpreter->GetParameter(parameterList[i]);
            paramName = parameterList[i].c_str();
            mSelectedListBox->Append(paramName);
            mReportWxStrings.Add(paramName);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_REPORT_PANEL
   MessageInterface::ShowMessage("ReportPanel::LoadData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ReportPanel::SaveData()
{
   #if DEBUG_REPORT_PANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() entering...\n");
   #endif
   
   canClose = true;
   
   std::string rfName = mReportFileComboBox->GetValue().c_str();
   
   ReportFile *reportFile =
      (ReportFile*)theGuiInterpreter->GetConfiguredObject(rfName);
   
   try
   {
      // save ReportFile
      if (mHasReportFileChanged)
      {
         #if DEBUG_REPORT_PANEL_SAVE
         MessageInterface::ShowMessage
            ("    rfName=%s, reportFile=%p\n", rfName.c_str(), reportFile);
         #endif
         
         theCommand->SetRefObject(reportFile, Gmat::SUBSCRIBER, rfName, 0);
         mHasReportFileChanged = false;
      }
      
      // save parameters
      if (mHasParameterChanged)
      {
         mHasParameterChanged = false;
         theCommand->TakeAction("Clear");
         Parameter *param = NULL;
         mNumParameters = mSelectedListBox->GetCount();
         
         for (int i=0; i<mNumParameters; i++)
         {
            std::string selYName =
               std::string(mSelectedListBox->GetString(i).c_str());
            param = theGuiInterpreter->GetParameter(selYName);
            theCommand->SetRefObject(param, Gmat::PARAMETER, selYName, 1);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   #if DEBUG_REPORT_PANEL_SAVE
   MessageInterface::ShowMessage("ReportPanel::SaveData() exiting...\n");
   #endif
   
}


//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void ReportPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewButton)
   {
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_REPORTABLE, true);
      
      paramDlg.SetParamNameArray(mReportWxStrings);
      paramDlg.ShowModal();
      
      if (paramDlg.HasSelectionChanged())
      {
         mHasParameterChanged = true;
         EnableUpdate(true);
         mReportWxStrings = paramDlg.GetParamNameArray();
         
         if (mReportWxStrings.Count() > 0)
         {
            mSelectedListBox->Clear();
            for (unsigned int i=0; i<mReportWxStrings.Count(); i++)
               mSelectedListBox->Append(mReportWxStrings[i]);
         }
         else // no selections
         {
            mSelectedListBox->Clear();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ReportPanel::OnComboBoxChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   
   if (obj == mReportFileComboBox)
   {
      mHasReportFileChanged = true;
      EnableUpdate(true);
   }
}


