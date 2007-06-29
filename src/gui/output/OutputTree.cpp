//$Header$
//------------------------------------------------------------------------------
//                              OutputTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2005/04/11
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/report.xpm"
#include "bitmaps/openglplot.xpm"
#include "bitmaps/xyplot.xpm"
#include "bitmaps/default.xpm"
#include <wx/string.h>        // for wxArrayString

#include "GmatAppData.hpp"
#include "OutputTree.hpp"
#include "GmatTreeItemData.hpp"
#include "ViewTextFrame.hpp"

#include "GuiInterpreter.hpp"
#include "Subscriber.hpp"
#include "ReportFile.hpp"
#include "FileUtil.hpp"       // for GmatFileUtil::Compare()
#include "MessageInterface.hpp"

//#define DEBUG_OUTPUT_TREE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OutputTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, OutputTree::OnItemRightClick)

   EVT_TREE_ITEM_ACTIVATED(-1, OutputTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, OutputTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, OutputTree::OnEndLabelEdit)
   EVT_MENU(POPUP_COMPARE_NUMERIC_VALUES, OutputTree::OnCompareNumericValues)
   EVT_MENU(POPUP_COMPARE_TEXT_LINES, OutputTree::OnCompareTextLines)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// OutputTree(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * A constructor.
 *
 * @param <parent> parent window
 * @param <id> window id for referencing events
 * @param <pos> position
 * @param <size> size 
 * @param <style> style flags
 */
//------------------------------------------------------------------------------
OutputTree::OutputTree(wxWindow *parent, const wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxTreeCtrl(parent, id, pos, size, style)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   AddIcons();
   AddDefaultResources();
   
   theGuiManager->UpdateAll();
}


//------------------------------------------------------------------------------
// void UpdateOutput(bool resetTree)
//------------------------------------------------------------------------------
/**
 * Updates output tree.
 *
 * @param  resetTree  true if just clearing tree
 */
//------------------------------------------------------------------------------
void OutputTree::UpdateOutput(bool resetTree)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::UpdateOutput() resetTree=%d\n", resetTree);
   #endif
   
   Collapse(mReportItem);
   Collapse(mOpenGlItem);
   Collapse(mXyPlotItem);
   
   // delete all old children
   DeleteChildren(mReportItem);
   DeleteChildren(mOpenGlItem);
   DeleteChildren(mXyPlotItem);
   
   if (resetTree)    // do not load subscribers
      return;
   
   // get list of report files, opengl plots, and xy plots
   StringArray listOfSubs = theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);

   // put each subscriber in the proper folder
   for (unsigned int i=0; i<listOfSubs.size(); i++)
   {
      Subscriber *sub = (Subscriber*)theGuiInterpreter->GetConfiguredObject(listOfSubs[i]);
      wxString objName = wxString(listOfSubs[i].c_str());
      wxString objTypeName = wxString(sub->GetTypeName().c_str());
      
      if (objTypeName.Trim() == "ReportFile")
         AppendItem(mReportItem, objName, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(objName, GmatTree::OUTPUT_REPORT));
      else if (objTypeName.Trim() == "OpenGLPlot")
         AppendItem(mOpenGlItem, objName, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(objName, GmatTree::OUTPUT_OPENGL_PLOT));
      else if (objTypeName.Trim() == "XYPlot")
         AppendItem(mXyPlotItem, objName, GmatTree::ICON_FILE, -1,
                 new GmatTreeItemData(objName, GmatTree::OUTPUT_XY_PLOT));
   }
   
   Expand(mReportItem);
   Expand(mOpenGlItem);
   Expand(mXyPlotItem);
}

//------------------------------------------------------------------------------
//  void AddDefaultResources()
//------------------------------------------------------------------------------
/**
 * Add default folders
 */
//------------------------------------------------------------------------------
void OutputTree::AddDefaultResources()
{
   wxTreeItemId output =
      AddRoot(wxT("Output"), -1, -1,
              new GmatTreeItemData(wxT("Output"), GmatTree::OUTPUT_FOLDER));

   //----- Reports
   mReportItem =
      AppendItem(output, wxT("Reports"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Reports"),
                                      GmatTree::REPORTS_FOLDER));
    
   SetItemImage(mReportItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- OpenGL Plots
    mOpenGlItem =
      AppendItem(output, wxT("OpenGL Plots"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("OpenGL PLOTS"),
                                      GmatTree::OPENGL_PLOTS_FOLDER));
    
   SetItemImage(mOpenGlItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   //----- XY Plots
   mXyPlotItem =
      AppendItem(output, wxT("XY Plots"), GmatTree::ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("XY Plots"),
                                      GmatTree::XY_PLOTS_FOLDER));
    
   SetItemImage(mXyPlotItem, GmatTree::ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);
}


//==============================================================================
//                         On Action Events
//==============================================================================

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * On right click show the pop up menu
 *
 */
//------------------------------------------------------------------------------
void OutputTree::OnItemRightClick(wxTreeEvent& event)
{
   ShowMenu(event.GetItem(), event.GetPoint());
}

//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <itemId> tree item that was right clicked
 * @param <pt> location of right click
 */
//------------------------------------------------------------------------------
void OutputTree::ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
{
   GmatTreeItemData *treeItem = (GmatTreeItemData *)GetItemData(itemId);
   theSubscriberName = treeItem->GetDesc();
   int dataType = treeItem->GetDataType();
   
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage
      ("OutputTree::ShowMenu() theSubscriberName=%s\n", theSubscriberName.c_str());
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;
   
   if (dataType == GmatTree::OUTPUT_REPORT)
   {
      menu.Append(POPUP_COMPARE_NUMERIC_VALUES, wxT("Compare Numeric Values"));
      menu.Append(POPUP_COMPARE_TEXT_LINES, wxT("Compare Text Lines"));
   }
   
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Double click on tree item
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnOpen(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnClose(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   
   // if its open, its activated
   if (GmatAppData::GetMainFrame()->IsChildOpen(item))
      GmatAppData::GetMainFrame()->CloseActiveChild();
   else
      return;
}

//------------------------------------------------------------------------------
// void OnRename(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnRename(wxCommandEvent &event)
{
   #if DEBUG_OUTPUT_TREE
   MessageInterface::ShowMessage("OutputTree::OnRename() entered\n");
   #endif
   
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
   wxString oldName = selItem->GetDesc();
   wxString newName = oldName;
   newName = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
                               newName, this);
   
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      
   }

}


//------------------------------------------------------------------------------
// void OnDelete(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Delete chosen from popup menu
 *
 * @param <event> command event
 * @todo Finish this when items can be deleted from interpreter
 */
//------------------------------------------------------------------------------
void OutputTree::OnDelete(wxCommandEvent &event)
{
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnBeginLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnBeginLabelEdit(wxTreeEvent &event)
{
  
   GmatTreeItemData *selItem = (GmatTreeItemData *)
      GetItemData(event.GetItem());
                               
   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if (GmatAppData::GetMainFrame()->IsChildOpen(selItem))
   {
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void OnEndLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Finished changing label on the tree
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void OutputTree::OnEndLabelEdit(wxTreeEvent &event)
{
   wxString newLabel = event.GetLabel();
   
   // check to see if label is a single word
   if (newLabel.IsWord())
   {
      GmatTreeItemData *selItem = (GmatTreeItemData *)
         GetItemData(GetSelection());
      
      wxString oldLabel = selItem->GetDesc();
      selItem->SetDesc(newLabel);
   }
   else
   {
      event.Veto();
   }
}


//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Add icons for items in the tree
 */
//------------------------------------------------------------------------------
void OutputTree::AddIcons()
{
   int size = 16;
  
   wxImageList *images = new wxImageList ( size, size, true );
  
   wxBusyCursor wait;
   wxIcon icons[7];

   icons[0] = wxIcon ( folder_xpm );
   icons[1] = wxIcon ( file_xpm );
   icons[2] = wxIcon ( openfolder_xpm );
   icons[3] = wxIcon ( report_xpm );
   icons[4] = wxIcon ( openglplot_xpm );
   icons[5] = wxIcon ( xyplot_xpm );
   icons[6] = wxIcon ( default_xpm );

   int sizeOrig = icons[0].GetWidth();
   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
   {
      if ( size == sizeOrig )
         images->Add(icons[i]);
      else
         images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
   }
   
   AssignImageList(images);

}

//---------------------------------
// Add items to tree
//---------------------------------
//------------------------------------------------------------------------------
// void OnAddReportFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a report file to reports folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddReportFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("ReportFile");

   if (theGuiInterpreter->CreateSubscriber
       ("ReportFile", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_REPORT, -1,
                 new GmatTreeItemData(name, GmatTree::REPORT_FILE));

      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnAddXyPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an xy plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddXyPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("XYPlot");

   if (theGuiInterpreter->CreateSubscriber
       ("XYPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_XY_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::XY_PLOT));
      
      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnAddOpenGlPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an opengl plot to plots folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnAddOpenGlPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
  
   wxString name;
   name.Printf("OpenGLPlot");

   if (theGuiInterpreter->CreateSubscriber
       ("OpenGLPlot", std::string(name.c_str())) != NULL)
   {
      AppendItem(item, name, GmatTree::ICON_OPEN_GL_PLOT, -1,
                 new GmatTreeItemData(name, GmatTree::OPENGL_PLOT));

      Expand(item);
   }
}


//------------------------------------------------------------------------------
// void OnCompareNumericValues(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Compare reports.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnCompareNumericValues(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("OutputTree::OnCompareNumericValues() entered\n");

   ReportFile *theReport =
      (ReportFile*) theGuiInterpreter->GetConfiguredObject(theSubscriberName.c_str());

   if (!theReport)
   {
      MessageInterface::ShowMessage
         ("OutputTree::OnCompareNumericValues() The ReportFile: %s is NULL.\n",
          theSubscriberName.c_str());
      return;
   }
   
   std::string filename1 = theReport->GetFileName();
   StringArray colTitles = theReport->GetRefObjectNameArray(Gmat::PARAMETER);
   wxString filename2 =
      wxFileSelector("Choose a file to open", "", "", "report",
                     "Report files (*.report)|*.report|"
                     "Text files (*.txt)|*.txt|All files (*.*)|*.*");
   
   if (filename2.empty())
      return;
   
   Real tol = GmatFileUtil::CompareAbsTol;
   wxString tolStr;
   tolStr.Printf("%e", tol);
   tolStr = wxGetTextFromUser("Enter absolute tolerance to be used in flagging: ",
                              "Tolerance", tolStr, this);
   
   if (!tolStr.ToDouble(&tol))
   {
      wxMessageBox("Entered Invalid Tolerance", "Error", wxOK, this);
      return;
   }
   
   StringArray output =
      GmatFileUtil::Compare(filename1.c_str(), filename2.c_str(), colTitles, tol);

   if (GmatAppData::theCompareWindow == NULL)
   {
      GmatAppData::theCompareWindow =
         new ViewTextFrame(GmatAppData::GetMainFrame(), _T("Compare Utility"),
                           50, 50, 800, 500, "Permanent");
      wxString msg;
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);      
      GmatAppData:: theCompareWindow->AppendText(msg);
   }
   
   GmatAppData::theCompareWindow->Show(true);
   
   for (unsigned int i=0; i<output.size(); i++)
      GmatAppData::theCompareWindow->AppendText(wxString(output[i].c_str()));
}


//------------------------------------------------------------------------------
// void OnCompareTextLines(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Compare reports.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void OutputTree::OnCompareTextLines(wxCommandEvent &event)
{
   //MessageInterface::ShowMessage("OutputTree::OnCompareTextLines() entered\n");

   ReportFile *theReport =
      (ReportFile*) theGuiInterpreter->GetConfiguredObject(theSubscriberName.c_str());
   
   if (!theReport)
   {
      MessageInterface::ShowMessage
         ("OutputTree::OnCompareTextLines() The ReportFile: %s is NULL.\n",
          theSubscriberName.c_str());
      return;
   }
   
   std::string filename1 = theReport->GetFileName();
   StringArray colTitles = theReport->GetRefObjectNameArray(Gmat::PARAMETER);
   wxString filename2 =
      wxFileSelector("Choose a file to open", "", "", "report",
                     "Report files (*.report)|*.report|"
                     "Text files (*.txt)|*.txt|All files (*.*)|*.*");
   
   if (filename2.empty())
      return;
   
   int file1DiffCount = 0;
   int file2DiffCount = 0;
   int file3DiffCount = 0;
   
   StringArray output =
      GmatFileUtil::CompareLines(1, filename1.c_str(), filename2.c_str(), "", "",
                                 file1DiffCount, file2DiffCount, file3DiffCount);
   
   if (GmatAppData::theCompareWindow == NULL)
   {
      GmatAppData::theCompareWindow =
         new ViewTextFrame(GmatAppData::GetMainFrame(), _T("Compare Utility"),
                           50, 50, 800, 500, "Permanent");
      wxString msg;
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);      
      GmatAppData:: theCompareWindow->AppendText(msg);
   }
   
   GmatAppData::theCompareWindow->Show(true);
   
   for (unsigned int i=0; i<output.size(); i++)
      GmatAppData::theCompareWindow->AppendText(wxString(output[i].c_str()));
}
