//$Header$
//------------------------------------------------------------------------------
//                                 AchievePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/12
//
/**
 * This class contains the Solver Variables window.
 */
//------------------------------------------------------------------------------

#ifndef AchievePanel_hpp
#define AchievePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Achieve.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

class AchievePanel : public GmatPanel
{
public:
   // constructors
   AchievePanel(wxWindow *parent, GmatCommand *cmd);
   ~AchievePanel(); 
    
private:             

   struct SolverType
   {
      wxString  solverName;
      wxString  goalName;
      Real      goalValue;
      Real      tolerance;
      Parameter *goalParam;
   };
   
   wxTextCtrl *mGoalNameTextCtrl;
   wxTextCtrl *mGoalValueTextCtrl;
   wxTextCtrl *mToleranceTextCtrl;
   
   wxButton *mViewGoalButton;
   
   wxComboBox *mSolverComboBox;
   
   SolverType mSolverData;
   
   Achieve *mAchieveCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void ShowGoalSetup();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection();
   void OnButton(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
  
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 53000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBO,
      ID_GRID,
   };
};

#endif // SolverGoalsPanel_hpp

