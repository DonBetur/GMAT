//$Header$
//------------------------------------------------------------------------------
//                              PropagatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/09/08
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropgateCommandPanel
/**
 * This class contains the Propagate command setup window.
 */
//------------------------------------------------------------------------------

#ifndef PropagatePanel_hpp
#define PropagatePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "Command.hpp"
#include "Propagate.hpp"
#include "StopCondition.hpp"
#include "PropSetup.hpp"

class PropagatePanel : public GmatPanel
{
public:
   // constructors
   PropagatePanel(wxWindow *parent, GmatCommand *cmd); //loj: 10/6/04 removed propName
   ~PropagatePanel();  
   
private:
   
   static const int MAX_PROP_ROW = 5;
   static const int MAX_STOPCOND_ROW = 10;
   static const int PROP_NAME_COL = 0;
   static const int PROP_SOS_COL = 1;
   static const int STOPCOND_NAME_COL = 0;
   static const int STOPCOND_DESC_COL = 1;
   
   struct PropType
   {
      bool isChanged;
      int  soCount;
      wxString propName;
      wxString soNames;
      wxArrayString soNameList;
   };
   
   struct StopCondType
   {
      bool           isChanged;
      wxString       name;
      wxString       desc;
      wxString       varName;
      wxString       typeName;
      wxString       relOpStr;
      Real           goal;
      Real           tol;
      Integer        repeat;
      StopCondition *stopCondPtr;
   };
   
   wxStaticText *synchStaticText;
   wxStaticText *varStaticText;
   wxStaticText *repeatStaticText;
   wxStaticText *tolStaticText;
   wxStaticText *nameStaticText;
   
   wxTextCtrl *stopNameTextCtrl;
   wxTextCtrl *varNameTextCtrl;
   wxTextCtrl *goalTextCtrl;
   wxTextCtrl *toleranceTextCtrl;
   wxTextCtrl *repeatTextCtrl;
   
   wxButton *updateButton;
   wxButton *deleteButton;
   wxButton *viewButton;
   
   wxComboBox *mPropModeComboBox;
   wxComboBox *equalityComboBox;
   
   wxGrid *propGrid;
   wxGrid *stopCondGrid;
   
   wxBoxSizer *mMiddleBoxSizer;
   wxStaticBoxSizer *mStopSizer;

   bool    mPropModeChanged;
   bool    mPropChanged;
   bool    mStopCondChanged;
   
   Integer mPropModeCount;
   Integer numOfEqualities;
   Integer mPropCount;
   Integer mSpaceObjectCount;
   Integer mStopCondCount;
   Integer mTempPropCount;
   Integer mCurrStopRow;
   PropType mTempProp[MAX_PROP_ROW];
   StopCondType mTempStopCond[MAX_STOPCOND_ROW];
   
   Propagate *thePropCmd;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void DisplayPropagator();
   void DisplayStopCondition(int selRow);
   void UpdateStopCondition();
   void RemoveStopCondition();
   void ClearStopCondition(int selRow);
   void ShowDetailedStopCond(int stopRow);
   void ActivateUpdateButton();
   
   // event handling method
   void OnTextChange(wxCommandEvent &event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent &event);
   void OnCellLeftClick(wxGridEvent &event);
   void OnCellRightClick(wxGridEvent &event);

   wxString FormatStopCondDesc(const wxString &varName,
                               const wxString &relOpStr,
                               Real &goal);
   void MoveUpPropData();
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 44000,
      ID_COMBOBOX,
      ID_BUTTON,
      ID_CHECKBOX,
      ID_GRID,
   };
};

#endif // PropagatePanel_hpp

