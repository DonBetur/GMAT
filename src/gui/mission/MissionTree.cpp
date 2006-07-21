//$Header$
//------------------------------------------------------------------------------
//                              MissionTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/09/02
/**
 * This class provides the tree for missions.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/propagateevent.xpm"
#include "bitmaps/target.xpm"
#include "bitmaps/whileloop.xpm"
#include "bitmaps/varyevent.xpm"
#include "bitmaps/achieveevent.xpm"
#include "bitmaps/deltav.xpm"
#include "bitmaps/callfunction.xpm"
#include "bitmaps/nestreturn.xpm"
#include "bitmaps/saveobject.xpm"
#include "bitmaps/equalsign.xpm"
#include "bitmaps/toggle.xpm"
#include "bitmaps/beginfb.xpm"
#include "bitmaps/endfb.xpm"

#include "MissionTree.hpp"
#include "MissionTreeItemData.hpp"
#include "ViewTextFrame.hpp"

#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MISSION_TREE_SHOW_CMD 1
//#define DEBUG_MISSION_TREE 1
//#define DEBUG_MISSION_TREE_CHILD 1
//#define DEBUG_MISSION_TREE_FIND 2
//#define DEBUG_MISSION_TREE_MENU 1

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(MissionTree, wxTreeCtrl)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MissionTree, wxTreeCtrl)
   EVT_PAINT(DecoratedTree::OnPaint)
   //EVT_UPDATE_UI(-1, DecoratedTree::OnPaint)
   //ag: 10/20/2004 Commented out so that the position of the click is not
   // checked to open up a panel from the variables/goals boxes
   //loj: 11/4/04 Uncommented so that double click on Target/If/For/While folder
   // will not collapse
   EVT_LEFT_DCLICK(MissionTree::OnDoubleClick)
   
   EVT_TREE_ITEM_RIGHT_CLICK(-1, MissionTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, MissionTree::OnItemActivated)
   
   EVT_MENU(POPUP_OPEN, MissionTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, MissionTree::OnClose)
   
   EVT_MENU(POPUP_ADD_MISSION_SEQ, MissionTree::OnAddMissionSeq)
   
   EVT_MENU_RANGE(POPUP_ADD_PROPAGATE, POPUP_ADD_SWITCH_CASE,
                  MissionTree::OnAddCommand)
   EVT_MENU_RANGE(POPUP_INSERT_PROPAGATE, POPUP_INSERT_SWITCH_CASE,
                  MissionTree::OnInsertCommand)
   
   EVT_MENU(POPUP_RUN, MissionTree::OnRun)
   EVT_MENU(POPUP_DELETE, MissionTree::OnDelete)
   EVT_MENU(POPUP_SHOW_SCRIPT, MissionTree::OnShowScript)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTree(wxWindow *parent, const wxWindowID id,
//              const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTree object.
 *
 * @param <parent> input parent.
 * @param <id> input id.
 * @param <pos> input position.
 * @param <size> input size.
 * @param <style> input style.
 *
 * @note Creates the tree for missions and adds a default mission.
 */
//------------------------------------------------------------------------------
MissionTree::MissionTree(wxWindow *parent, const wxWindowID id,
                         const wxPoint &pos, const wxSize &size, long style)
   : DecoratedTree(parent, id, pos, size, style),
     inScriptEvent(false),
     inFiniteBurn(false)
{
   parent = parent;
   
   // mainNotebook = GmatAppData::GetMainNotebook();
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   mCommandList.Clear();
   mCommandList.Add("Propagate");
   mCommandList.Add("Maneuver");
   mCommandList.Add("BeginFiniteBurn");
   mCommandList.Add("EndFiniteBurn");
   mCommandList.Add("Target");
   mCommandList.Add("CallFunction");
   mCommandList.Add("GMAT");
   mCommandList.Add("Toggle");
   mCommandList.Add("Save");
   mCommandList.Add("ScriptEvent");
   
   SetParameter(BOXCOUNT, 0);
   SetParameter(DRAWOUTLINE, 0);

   InitializeCounter();
   AddIcons();
   AddDefaultMission();
}


//------------------------------------------------------------------------------
// void UpdateMission(bool resetCounter)
//------------------------------------------------------------------------------
/**
 * Updates Mission Sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateMission(bool resetCounter)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::UpdateMission() entered\n");
   #endif
   
   if (resetCounter)
   {
      InitializeCounter();      
      inScriptEvent = false;
      inFiniteBurn = false;
   }
   
   DeleteChildren(mMissionSeqSubItem);
   UpdateCommand();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void InitializeCounter()
//------------------------------------------------------------------------------
/**
 * Initializes command counter.
 */
//------------------------------------------------------------------------------
void MissionTree::InitializeCounter()
{
   mTempCounter = 0;
   mNumManeuver = 0;
   mNumMissionSeq = 0;
   mNumPropagate = 0;
   mNumManeuver = 0;
   mNumTarget = 0;
   mNumAchieve = 0;
   mNumVary = 0;
   mNumSave = 0;
   mNumToggle = 0;
   mNumIfStatement = 0;
   mNumWhileLoop = 0;
   mNumForLoop = 0;
   mNumDoWhile = 0;
   mNumSwitchCase = 0;
   mNumFunct = 0;
   mNumAssign = 0;
   mNumScriptEvent = 0;
   mNumFiniteBurn = 0;
}


//------------------------------------------------------------------------------
// void UpdateCommand()
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::UpdateCommand()
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   MessageInterface::ShowMessage("MissionTree::UpdateCommand() entered\n");
   ShowCommands("InUpdateCommand()");
   #endif
   
   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();
   GmatCommand *child;
   MissionTreeItemData *seqItemData =
      (MissionTreeItemData *)GetItemData(mMissionSeqSubItem);
   wxTreeItemId node;
   
   if (cmd->GetTypeName() == "NoOp")
      seqItemData->SetCommand(cmd);

   while (cmd != NULL)
   {
      node = UpdateCommandTree(mMissionSeqSubItem, cmd);
      
      child = cmd->GetChildCommand(0);
      
      if (child != NULL)
      {
         ExpandChildCommand(node, cmd, 0);
      }
      
      cmd = cmd->GetNext();
      
   }

   Expand(mMissionSeqSubItem);
}


//------------------------------------------------------------------------------
// wxTreeItemId& UpdateCommandTree(wxTreeItemId parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Updates commands in the mission sequence
 */
//------------------------------------------------------------------------------
wxTreeItemId& MissionTree::UpdateCommandTree(wxTreeItemId parent,
                                             GmatCommand *cmd)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::UpdateCommandTree() cmd=%s\n", cmd->GetTypeName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxTreeItemId currId;
   
   // Moved BeginScript to the top so it can catch all of the commands between
   // it and the EndScript
   if ((cmdTypeName == "BeginScript") || inScriptEvent)
   {
      if (!inScriptEvent) {
         mNewTreeId = AppendCommand(parent, GmatTree::MISSION_ICON_FILE,
            GmatTree::SCRIPT_COMMAND, cmd, &mNumScriptEvent,
            mNumScriptEvent);
         
         inScriptEvent = true;
         Expand(parent);
      }
      if (cmdTypeName == "EndScript")
         inScriptEvent = false;
   }
   
//    // BeginFiniteBurn is not a brach command but treat it as branch command
//    // in the MissionTree, i.e show folder icon
   
//    else if (cmdTypeName == "BeginFiniteBurn")
//    {
//       mFiniteBurnTreeId = 
//          AppendCommand(parent, GetIconId(cmdTypeName),
//                        GetCommandId(cmdTypeName), cmd,
//                        GetCommandCounter(cmdTypeName),
//                        *GetCommandCounter(cmdTypeName));
      
//       inFiniteBurn = true;
//       SetItemImage(mFiniteBurnTreeId, GmatTree::MISSION_ICON_OPENFOLDER,
//                    wxTreeItemIcon_Expanded);
//    }
//    else if (cmdTypeName == "EndFiniteBurn")
//    {
//       mNewTreeId =
//          AppendCommand(mFiniteBurnTreeId, GmatTree::MISSION_ICON_NEST_RETURN,
//                        GmatTree::END_FINITE_BURN_COMMAND,
//                        cmd, &mNumFiniteBurn, mNumFiniteBurn);
//       inFiniteBurn = false;
//    }
   else if (cmdTypeName == "NoOp")
   {
      
   }
   else
   {
      currId = parent;
      
//       if (inFiniteBurn)
//          currId = mFiniteBurnTreeId;
      
      mNewTreeId = 
            AppendCommand(currId, GetIconId(cmdTypeName),
                          GetCommandId(cmdTypeName), cmd,
                          GetCommandCounter(cmdTypeName),
                          *GetCommandCounter(cmdTypeName));
      
      SetItemImage(mNewTreeId, GmatTree::MISSION_ICON_OPENFOLDER,
                   wxTreeItemIcon_Expanded);
      
      //Expand(parent);
      Expand(currId);
   }
   
   return mNewTreeId;
}


//------------------------------------------------------------------------------
// void ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Expands child commands in the mission sequence
 */
//------------------------------------------------------------------------------
void MissionTree::ExpandChildCommand(wxTreeItemId parent, GmatCommand *cmd,
                                     Integer level)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::ExpandChildCommand() cmd=%s, level=%d\n",
       cmd->GetTypeName().c_str(), level);
   #endif
   
   wxTreeItemId node;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = cmd->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      while ((nextInBranch != NULL) && (nextInBranch != cmd))
      {
         #if DEBUG_MISSION_TREE_CHILD
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         MessageInterface::ShowMessage("----- %s\n", nextInBranch->GetTypeName().c_str());
         #endif
         
         node = UpdateCommandTree(parent, nextInBranch);
         
         if (nextInBranch->GetChildCommand() != NULL)
            ExpandChildCommand(node, nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}


//------------------------------------------------------------------------------
// void AppendCommand(const wxString &cmdName)
//------------------------------------------------------------------------------
void MissionTree::AppendCommand(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Append: " + cmdName);
   #endif
   
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId lastId = GetLastChild(itemId);
   wxTreeItemId parentId = GetItemParent(itemId);
   wxTreeItemId currId = itemId;
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::AppendCommand(%s) still here,  "
                     "\n    itemId = \"%s\"\n   lastId = \"%s\"\n",
                     cmdName.c_str(),
                     GetItemText(itemId).c_str(),
                     GetItemText(lastId).c_str());
   #endif
   
   // Append command before Else, if there is Else
   if (cmdName != "Else")
   {
      wxTreeItemId elseId = FindChild(itemId, "Else");

      if (elseId.IsOk())
         lastId = elseId;
   }
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand(%s) after FindChild(Else) itemId=<%s>, "
       " lastId=<%s>\n", cmdName.c_str(), GetItemText(itemId).c_str(),
       GetItemText(lastId).c_str());
   #endif
   
   wxTreeItemId prevId = GetPrevVisible(lastId);
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(itemId);
   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);
   GmatCommand *prevCmd = NULL;
   GmatCommand *cmd = NULL;
   
   // handle case prevItem is NULL
   if (prevItem != NULL)
      prevCmd = prevItem->GetCommand();
   else
      prevCmd = currItem->GetCommand();

   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand(cmd) prevCmd=%s\n",
       prevCmd->GetTypeName().c_str());
   #endif
   
   if (cmdName == "IfElse")
      cmd = theGuiInterpreter->CreateDefaultCommand("If");
   else
      cmd = theGuiInterpreter->CreateDefaultCommand(std::string(cmdName.c_str()));
   
   if (cmd != NULL)
   {
      // if Maneuver, BeginFiniteBurn, Vary, update burn to show default burn
      if (cmdName == "Maneuver" || cmdName == "BeginFiniteBurn" ||
          cmdName == "Vary")
         theGuiManager->UpdateBurn();
      
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage
         ("MissionTree::AppendCommand(%s) calling InsertCommand()\n", cmdName.c_str());
      #endif

      wxTreeItemId node =
         InsertCommand(itemId, itemId, prevId, GetIconId(cmdName),
                       GetCommandId(cmdName), cmdName, prevCmd, cmd,
                       GetCommandCounter(cmdName));
      
      SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
      
      Expand(itemId);
      Expand(node);
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After Append: " + cmdName);
   #endif
}


//------------------------------------------------------------------------------
// MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType type, GmatCommand *cmd,
//                            int *cmdCount, int endCount)
//------------------------------------------------------------------------------
/**
 * Appends command to command list and/or command tree.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::AppendCommand(wxTreeItemId parent, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, GmatCommand *cmd,
                           int *cmdCount, int endCount)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand(%s) type = \"%s\" and name = \"%s\"\n",
       GetItemText(parent).c_str(),  cmd->GetTypeName().c_str(), 
       cmd->GetName().c_str());
   #endif
   
   wxString cmdTypeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   
   wxTreeItemId node;
   
   // compose node name
   if (cmdTypeName.Contains("End"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), endCount);
   }
   else if (cmdTypeName.Contains("Else"))
   {
      if (nodeName.Trim() == "")
         nodeName.Printf("%s%d", cmdTypeName.c_str(), endCount);
   }
   else
   {
      if (nodeName.Trim() == "" || nodeName == cmdTypeName)
         nodeName.Printf("%s%d", cmdTypeName.c_str(), ++(*cmdCount));      
   }
   
   if (nodeName.Contains("BeginScript"))
   {
      nodeName.Replace("BeginScript", "ScriptEvent");
   }
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::AppendCommand() cmdTypeName=%s, nodeName=%s\n",
       cmdTypeName.c_str(), nodeName.c_str());
   #endif

   node = AppendItem(parent, nodeName, icon, -1,
                     new MissionTreeItemData(nodeName, type, nodeName, cmd));

   return node;
}


//------------------------------------------------------------------------------
// void InsertCommand(const wxString &cmdName)
//------------------------------------------------------------------------------
void MissionTree::InsertCommand(const wxString &cmdName)
{
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("Before Insert: " + cmdName);
   #endif
   
   wxTreeItemId itemId = GetSelection();
   wxTreeItemId parentId = GetItemParent(itemId);

#ifdef __WXMAC__
   wxTreeItemId prevId = GetPrevSibling(itemId);

   // Check if no previous item then get parent item
   if (!prevId.IsOk())
   {
      if (parentId.IsOk())
         prevId = parentId;
   }
#else
   wxTreeItemId prevId = GetPrevVisible(itemId);
#endif

   MissionTreeItemData *prevItem = (MissionTreeItemData *)GetItemData(prevId);   
   // Check if empty previous item then doesn't insert anything
   if (prevItem == NULL)
   {
      MessageInterface::ShowMessage("\n***************  Warning ***************"
             "\nMissionTree::InsertCommand() has empty prevItem "
             "so it can't insert before this."
             "\n****************************************");
      return;
   } 

   GmatCommand *prevCmd = prevItem->GetCommand();
   GmatCommand *cmd = NULL;
   
   if (prevCmd != NULL)
   {
      if (cmdName == "IfElse")
      {
         cmd = theGuiInterpreter->CreateDefaultCommand("If");
      }
      else
      {
         cmd = theGuiInterpreter->
            CreateDefaultCommand(std::string(cmdName.c_str()));
      }
      
      if (cmd != NULL)
      {
         wxTreeItemId node =
            InsertCommand(parentId, itemId, prevId, GetIconId(cmdName),
                          GetCommandId(cmdName), cmdName, prevCmd, cmd,
                          GetCommandCounter(cmdName));
         
         SetItemImage(node, GmatTree::MISSION_ICON_OPENFOLDER, 
                      wxTreeItemIcon_Expanded);
         
         Expand(node);
      }
   }
   
   #if DEBUG_MISSION_TREE_SHOW_CMD
   ShowCommands("After Insert: " + cmdName);
   #endif
}


//------------------------------------------------------------------------------
// wxTreeItemId InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
//                            wxTreeItemId prevId, GmatTree::MissionIconType icon,
//                            GmatTree::ItemType type, const wxString &cmdName,
//                            GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount)
//------------------------------------------------------------------------------
/**
 * Inserts tree item.
 */
//------------------------------------------------------------------------------
wxTreeItemId
MissionTree::InsertCommand(wxTreeItemId parentId, wxTreeItemId currId,
                           wxTreeItemId prevId, GmatTree::MissionIconType icon,
                           GmatTree::ItemType type, const wxString &cmdName,
                           GmatCommand *prevCmd, GmatCommand *cmd, int *cmdCount)
{
   MissionTreeItemData *currItem = (MissionTreeItemData *)GetItemData(currId); 
   GmatCommand *currCmd = currItem->GetCommand();
   wxString currTypeName = currCmd->GetTypeName().c_str();
   wxString typeName = cmd->GetTypeName().c_str();
   wxString nodeName = cmd->GetName().c_str();
   wxTreeItemId node;
   GmatCommand *endCmd = NULL;
   GmatCommand *elseCmd = NULL;
   GmatTree::ItemType endType = GmatTree::END_TARGET_COMMAND;
   bool cmdAdded = false;
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() parentId=%s, currId=%s, prevId=%s\n",
       GetItemText(parentId).c_str(), GetItemText(currId).c_str(),
       GetItemText(prevId).c_str());
   
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parentId); 
   GmatCommand *parentCmd = parentItem->GetCommand();
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() cmdName=%s, typeName=%s, nodeName=%s, "
       "cmdCount=%d\n   parentCmd=%s, prevCmd=%s, currCmd=%s\n", cmdName.c_str(),
       typeName.c_str(), nodeName.c_str(), *cmdCount,
       parentCmd->GetTypeName().c_str(), prevCmd->GetTypeName().c_str(),
       currTypeName.c_str());
   #endif
   
   //@Note "BeginFiniteBurn" is not a branch command but need "EndFiniteBurn"
   
   //------------------------------------------------------------
   // Create End* command if branch command
   //------------------------------------------------------------
   if (typeName == "Target" || typeName == "For"  ||  typeName == "While" ||
       typeName == "If" || typeName == "BeginScript")
       ////typeName == "BeginFiniteBurn")
   {
      if (typeName == "Target")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndTarget");
         endType = GmatTree::END_TARGET_COMMAND;
      }
      else if (typeName == "For")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndFor");
         endType = GmatTree::END_FOR_CONTROL;
      }
      else if (typeName == "While")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndWhile");
         endType = GmatTree::END_WHILE_CONTROL;
      }
      else if (typeName == "If")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndIf");
         endType = GmatTree::END_IF_CONTROL;
      }
      else if (typeName == "BeginScript")
      {
         endCmd = theGuiInterpreter->CreateCommand("EndScript");
         endType = GmatTree::END_TARGET_COMMAND;
      }
//       else if (typeName == "BeginFiniteBurn")
//       {
//          endCmd = theGuiInterpreter->CreateDefaultCommand("EndFiniteBurn", "", cmd);
//          endType = GmatTree::END_FINITE_BURN_COMMAND;
//       }
      
      // create Else (temp code)
      if (cmdName == "IfElse")
      {
         elseCmd = theGuiInterpreter->CreateDefaultCommand("Else");
         cmd->Append(elseCmd);
      }
      
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Appending End*...\n");
      #endif
      
      cmdAdded = cmd->Append(endCmd);
      
   }
   
   //------------------------------------------------------------
   // Compose node name
   //------------------------------------------------------------
   if (typeName == "Else")
      nodeName.Printf("%s%d", typeName.c_str(), (*cmdCount));
   
   // if command has name or command has the type name append counter
   if (nodeName.Trim() == "" || nodeName == typeName)
      nodeName.Printf("%s%d", typeName.c_str(), ++(*cmdCount));
   
//    if (nodeName.Contains("BeginScript"))
//       nodeName.Replace("BeginScript", "ScriptEvent");
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() cmd=%s, nodeName=%s, cmdCount=%d\n",
       typeName.c_str(), nodeName.c_str(), *cmdCount);
   #endif
   
   //------------------------------------------------------------
   // Insert command to mission sequence
   //------------------------------------------------------------
   wxString prevTypeName = prevCmd->GetTypeName().c_str();
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() cmd=%s, nodeName=%s, prevTypeName=%s\n",
       typeName.c_str(), nodeName.c_str(), prevTypeName.c_str());
   MessageInterface::ShowMessage
      ("MissionTree::InsertCommand() prevTypeName=%s\n", prevTypeName.c_str());
   #endif
   
   if (currCmd->GetTypeName() == "NoOp")
   {
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Appending command...\n");
      #endif
      
      // Append to base command list
      cmdAdded = theGuiInterpreter->AppendCommand(cmd);
   }
   else
   {
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("----- Inserting command...\n");
      #endif
      
      cmdAdded = theGuiInterpreter->InsertCommand(cmd, prevCmd);
   }
   
   //loj: 9/21/05 Set to true for now.
   // Why returning false eventhough it inserted?
   cmdAdded = true; 
   
   //------------------------------------------------------------
   // Insert command to mission tree
   //------------------------------------------------------------
   if (cmdAdded)
   {
      if (currCmd->GetTypeName() == "NoOp")
      {
         node = AppendItem(currId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (typeName == "EndFiniteBurn")
      {
         node = AppendItem(parentId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName == "NoOp" ||
               prevTypeName == "Target" || prevTypeName == "For"  ||
               prevTypeName == "While" || prevTypeName == "If")
               ////prevTypeName == "BeginFiniteBurn")
      {
         node = InsertItem(parentId, 0, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else if (prevTypeName.Contains("End"))
      {
         wxString currTypeName = currCmd->GetTypeName().c_str();
         
         node = InsertItem(parentId, GetPrevSibling(currId), nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      else
      {
         node = InsertItem(parentId, prevId, nodeName, icon, -1,
                           new MissionTreeItemData(nodeName, type, nodeName, cmd));
      }
      
      //---------------------------------------------------------
      // Append End* command
      //---------------------------------------------------------
      if (typeName == "Target" || typeName == "For"  ||  typeName == "While" ||
          typeName == "If")//// || typeName == "BeginFiniteBurn")
      {
         // append Else (temp code until Else is implemented)
         if (cmdName == "IfElse")
         {
            wxString elseName;
            elseName.Printf("Else%d", (*cmdCount));
            
            wxTreeItemId elseNode =
               InsertItem(node, 0, elseName, icon, -1,
                          new MissionTreeItemData(elseName, GmatTree::ELSE_CONTROL,
                                                  elseName, elseCmd));
            
            SetItemImage(elseNode, GmatTree::MISSION_ICON_OPENFOLDER,
                         wxTreeItemIcon_Expanded);
            
            wxString endName = "End" + typeName;
            endName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, elseNode, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(endName, endType, endName, endCmd));
         }
         else
         {
            wxString endName;
//             if (typeName == "BeginFiniteBurn")
//                endName = "EndFiniteBurn";
//             else
               endName = "End" + typeName;
            
            endName.Printf("%s%d", endName.c_str(), *cmdCount);
            InsertItem(node, 0, endName, GmatTree::MISSION_ICON_NEST_RETURN, -1,
                       new MissionTreeItemData(endName, endType, endName, endCmd));
         }
      }
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Command:%s not appended or created\n",
          cmd->GetTypeName().c_str());
   }
   
   return node;
}

//------------------------------------------------------------------------------
// void AddDefaultMission()
//------------------------------------------------------------------------------
/**
 * Adds a default mission to tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMission()
{
   wxTreeItemId mission = AddRoot(wxT("Mission"), -1, -1,
                                  new MissionTreeItemData(wxT("Mission"),
                                                          GmatTree::MISSIONS_FOLDER));

   //----- Mission Sequence
   mMissionSeqTopItem =
      AppendItem(mission, wxT("MissionSequence"), GmatTree::MISSION_ICON_FOLDER,
                 -1, new MissionTreeItemData(wxT("MissionSequence"),
                                             GmatTree::MISSION_SEQ_TOP_FOLDER));
    
   SetItemImage(mMissionSeqTopItem, GmatTree::MISSION_ICON_OPENFOLDER,
               wxTreeItemIcon_Expanded);
   
   AddDefaultMissionSeq(mMissionSeqTopItem);
   
   UpdateCommand();
   
   theGuiInterpreter->ResetConfigurationChanged(false, true);

}

//------------------------------------------------------------------------------
// void AddDefaultMissionSeq(wxTreeItemId item)
//------------------------------------------------------------------------------
void MissionTree::AddDefaultMissionSeq(wxTreeItemId item)
{
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::AddDefaultMission() entered\n");
   #endif
   
   //loj: for multiple mission sequence for b2
   //StringArray itemNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::MISSION_SEQ);
    
   //int size = itemNames.size();
   //for (int i = 0; i<size; i++)
   //{
   //    wxString objName = wxString(itemNames[i].c_str());
   //    AppendItem(item, wxT(objName), GmatTree::MISSION_ICON_FOLDER, -1,
   //                     new MissionTreeItemData(wxT(objName), GmatTree::MISSION_SEQ_COMMAND));
   //};

   wxString name;
    
   name.Printf("Sequence%d", ++mNumMissionSeq);
  
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, 
                                         GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   Expand(item);
}

//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Adds icons to a list, so that they can be used in the tree.
 */
//------------------------------------------------------------------------------
void MissionTree::AddIcons()
{
   int size = 16;

   wxImageList *images = new wxImageList ( size, size, true );

   wxBusyCursor wait;
   wxIcon icons[17];

   icons[0] = wxIcon ( propagateevent_xpm );
   icons[1] = wxIcon ( target_xpm );
   icons[2] = wxIcon ( folder_xpm );
   icons[3] = wxIcon ( file_xpm );
   icons[4] = wxIcon ( openfolder_xpm );
   icons[5] = wxIcon ( openfolder_xpm );
   icons[6] = wxIcon ( whileloop_xpm );
   icons[7] = wxIcon ( varyevent_xpm );
   icons[8] = wxIcon ( achieveevent_xpm );
   icons[9] = wxIcon ( deltav_xpm );
   icons[10]= wxIcon ( callfunction_xpm );
   icons[11]= wxIcon ( nestreturn_xpm );
   icons[12]= wxIcon ( saveobject_xpm );
   icons[13]= wxIcon ( equalsign_xpm );
   icons[14]= wxIcon ( toggle_xpm );
   icons[15]= wxIcon ( beginfb_xpm );
   icons[16]= wxIcon ( endfb_xpm );

//   int sizeOrig = icons[0].GetWidth();
   for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
   {
          // 30/01/06 - arg - always resize because linux is not showing unscaled
          // icons correctly
//      if ( size == sizeOrig )
//      {
//         images->Add(icons[i]);
//      }
//      else
//      {
         images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
//      }
   }

   AssignImageList(images);

}

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * Brings up popup menu on a right click.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemRightClick(wxTreeEvent& event)
{
   ShowMenu(event.GetItem(), event.GetPoint());
}

//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * On a double click sends the TreeItemData to GmatMainFrame to open a new
 * window.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);
   
   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage("MissionTree::OnItemActivated() item=%s\n",
                                 item->GetDesc().c_str());
   #endif
   
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnDoubleClick(wxMouseEvent &event)
//------------------------------------------------------------------------------
/**
 * On a double click ...
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void MissionTree::OnDoubleClick(wxMouseEvent &event)
{
   //MessageInterface::ShowMessage("MissionTree::OnDoubleClick() entered\n");
   //wxPoint position = event.GetPosition();
   //MessageInterface::ShowMessage("Event position is %d %d\n", position.x, position.y );
   
   wxTreeItemId itemId = GetSelection();
   MissionTreeItemData *item = (MissionTreeItemData *)GetItemData(itemId);

   #if DEBUG_MISSION_TREE
   MessageInterface::ShowMessage
      ("MissionTree::OnDoubleClick() item=%s\n", item->GetDesc().c_str());
   #endif
   
   // Show panel here. because OnItemActivated() always collapse the node.
   GmatAppData::GetMainFrame()->CreateChild(item);

   //CheckClickIn(position);
}


//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId id, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Creates and shows a popup menu.
 *
 * @param <id> input TreeItemId.
 * @param <pt> input location for popup menu.
 */
//------------------------------------------------------------------------------
void MissionTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
   MissionTreeItemData *treeItem = (MissionTreeItemData *)GetItemData(id);
   wxString title = treeItem->GetDesc();
   int dataType = treeItem->GetDataType();
   wxTreeItemId parent = GetItemParent(id);
   MissionTreeItemData *parentItem = (MissionTreeItemData *)GetItemData(parent);
   int parentDataType = parentItem->GetDataType();
   
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::ShowMenu() dataType=%d\n", dataType);
   #endif
   
#if wxUSE_MENUS
   wxMenu menu;

   if (dataType == GmatTree::MISSION_SEQ_TOP_FOLDER)
   {
      menu.Append(POPUP_ADD_MISSION_SEQ, wxT("Add Mission Sequence"));
      menu.Enable(POPUP_ADD_MISSION_SEQ, FALSE);
   }
   else if (dataType == GmatTree::MISSION_SEQ_SUB_FOLDER)
   {
      menu.Append(POPUP_ADD_COMMAND, wxT("Append"),
                  CreatePopupMenu(dataType, false));
      // ag: can't delete because sys. doesn't handle multiple sequences yet
      // menu.Append(POPUP_DELETE, wxT("Delete"));
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Enable(POPUP_RENAME, FALSE);
      menu.AppendSeparator();
      menu.Append(POPUP_RUN, wxT("Run"));
      menu.AppendSeparator();
      menu.Append(POPUP_SHOW_SCRIPT, wxT("Show Script"));
   }
   else
   {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      
      if (dataType == GmatTree::TARGET_COMMAND)
      {
         menu.Append(POPUP_ADD_COMMAND, wxT("Append"),
                     CreateTargetPopupMenu(dataType, false));
         if (parentDataType == GmatTree::TARGET_COMMAND)
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert Before"),
                        CreateTargetPopupMenu(dataType, true));
         else
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert Before"),
                        CreatePopupMenu(dataType, true));
         
         menu.AppendSeparator();
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      } 
      else 
      {
         if (parentDataType == GmatTree::TARGET_COMMAND)
         {
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert Before"),
                        CreateTargetPopupMenu(dataType, true));
         }
         else
         {
            menu.Append(POPUP_INSERT_COMMAND, wxT("Insert Before"),
                        CreatePopupMenu(dataType, true));
            
            if ((dataType == GmatTree::IF_CONTROL) ||
                (dataType == GmatTree::ELSE_CONTROL) ||
                (dataType == GmatTree::FOR_CONTROL) ||
                (dataType == GmatTree::WHILE_CONTROL))
            {
               menu.Append(POPUP_ADD_COMMAND, wxT("Append"),
                           CreatePopupMenu(dataType, false));
            }
         }
         
         menu.AppendSeparator();
         menu.Append(POPUP_DELETE, wxT("Delete"));
         menu.Append(POPUP_RENAME, wxT("Rename"));
         menu.Enable(POPUP_RENAME, FALSE);
      }
   }
   
   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


//------------------------------------------------------------------------------
// void OnAddMissionSeq(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddMissionSeq(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   
   name.Printf("Sequence%d", ++mNumMissionSeq);
   
   mMissionSeqSubItem =
      AppendItem(item, name, GmatTree::MISSION_ICON_FOLDER, -1,
                 new MissionTreeItemData(name, GmatTree::MISSION_SEQ_SUB_FOLDER));
    
   SetItemImage(mMissionSeqSubItem, GmatTree::MISSION_ICON_OPENFOLDER,
                wxTreeItemIcon_Expanded);

   Expand(item);
}


//------------------------------------------------------------------------------
// void OnAddCommand(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnAddCommand(wxCommandEvent &event)
{
   switch (event.GetId())
   {
   case POPUP_ADD_PROPAGATE:
      AppendCommand("Propagate");
      break;
   case POPUP_ADD_MANEUVER:
      AppendCommand("Maneuver");
      break;
   case POPUP_ADD_BEGIN_FINITE_BURN:
      AppendCommand("BeginFiniteBurn");
      break;
   case POPUP_ADD_END_FINITE_BURN:
      AppendCommand("EndFiniteBurn");
      break;
   case POPUP_ADD_TARGET:
      AppendCommand("Target");
      break;
   case POPUP_ADD_VARY:
      AppendCommand("Vary");
      break;
   case POPUP_ADD_ACHIEVE:
      AppendCommand("Achieve");
      break;
   case POPUP_ADD_FUNCTION:
      AppendCommand("CallFunction");
      break;
   case POPUP_ADD_ASSIGNMENT:
      AppendCommand("GMAT");
      break;
   case POPUP_ADD_TOGGLE:
      AppendCommand("Toggle");
      break;
   case POPUP_ADD_SAVE:
      AppendCommand("Save");
      break;
   case POPUP_ADD_SCRIPT_EVENT:
      AppendCommand("BeginScript");
      break;
   case POPUP_ADD_IF_STATEMENT:
      AppendCommand("If");
      break;
   case POPUP_ADD_IF_ELSE_STATEMENT:
      AppendCommand("IfElse");
      break;
   case POPUP_ADD_ELSE_STATEMENT:
      AppendCommand("Else");
      break;
   case POPUP_ADD_ELSE_IF_STATEMENT:
      //AppendCommand("ElseIf");
      break;
   case POPUP_ADD_FOR_LOOP:
      AppendCommand("For");
      break;
   case POPUP_ADD_WHILE_LOOP:
      AppendCommand("While");
      break;
   case POPUP_ADD_D0_WHILE:
      //AppendCommand("Do");
      break;
   case POPUP_ADD_SWITCH_CASE:
      //AppendCommand("Switch");
      break;
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// void OnInsertCommand(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MissionTree::OnInsertCommand(wxCommandEvent &event)
{
   switch (event.GetId())
   {
   case POPUP_INSERT_PROPAGATE:
      InsertCommand("Propagate");
      break;
   case POPUP_INSERT_MANEUVER:
      InsertCommand("Maneuver");
      break;
   case POPUP_INSERT_BEGIN_FINITE_BURN:
      InsertCommand("BeginFiniteBurn");
      break;
   case POPUP_INSERT_END_FINITE_BURN:
      InsertCommand("EndFiniteBurn");
      break;
   case POPUP_INSERT_TARGET:
      InsertCommand("Target");
      break;
   case POPUP_INSERT_VARY:
      InsertCommand("Vary");
      break;
   case POPUP_INSERT_ACHIEVE:
      InsertCommand("Achieve");
      break;
   case POPUP_INSERT_FUNCTION:
      InsertCommand("CallFunction");
      break;
   case POPUP_INSERT_ASSIGNMENT:
      InsertCommand("GMAT");
      break;
   case POPUP_INSERT_TOGGLE:
      InsertCommand("Toggle");
      break;
   case POPUP_INSERT_SAVE:
      InsertCommand("Save");
      break;
   case POPUP_INSERT_SCRIPT_EVENT:
      InsertCommand("BeginScript");
      break;
   case POPUP_INSERT_IF_STATEMENT:
      InsertCommand("If");
      break;
   case POPUP_INSERT_IF_ELSE_STATEMENT:
      InsertCommand("IfElse");
      break;
   case POPUP_INSERT_ELSE_STATEMENT:
      InsertCommand("Else");
      break;
   case POPUP_INSERT_ELSE_IF_STATEMENT:
      //InsertCommand("ElseIf");
      break;
   case POPUP_INSERT_FOR_LOOP:
      InsertCommand("For");
      break;
   case POPUP_INSERT_WHILE_LOOP:
      InsertCommand("While");
      break;
   case POPUP_INSERT_D0_WHILE:
      //InsertCommand("Do");
      break;
   case POPUP_INSERT_SWITCH_CASE:
      //InsertCommand("Switch");
      break;
   default:
      break;
   }
}


//---------------------------------
// Crete popup menu
//---------------------------------

//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(int type, bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreatePopupMenu(int type, bool insert)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreatePopupMenu() type=%d, insert=%d\n", type, insert);
   #endif
   
   unsigned int i;
   wxMenu *menu = new wxMenu;

   for (i=0; i<mCommandList.GetCount(); i++)
      menu->Append(GetMenuId(mCommandList[i], insert), mCommandList[i]);
   
   menu->Append(POPUP_CONTROL_LOGIC, "Control Logic",
                CreateControlLogicPopupMenu(type, insert));
   
   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateTargetPopupMenu(int type, bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateTargetPopupMenu(int type, bool insert)
{
   wxMenu *menu;
   
   menu = CreatePopupMenu(type, insert);
   menu = AppendTargetPopupMenu(menu, insert);

   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* AppendTargetPopupMenu(wxMenu *menu, bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::AppendTargetPopupMenu(wxMenu *menu, bool insert)
{
   if (insert)
   {
      menu->Append(POPUP_INSERT_VARY, wxT("Vary"));
      menu->Append(POPUP_INSERT_ACHIEVE, wxT("Achieve"));
   }
   else
   {
      menu->Append(POPUP_ADD_VARY, wxT("Vary"));
      menu->Append(POPUP_ADD_ACHIEVE, wxT("Achieve"));
   }

   return menu;
}


//------------------------------------------------------------------------------
// wxMenu* CreateControlLogicPopupMenu(int type, bool insert)
//------------------------------------------------------------------------------
wxMenu* MissionTree::CreateControlLogicPopupMenu(int type, bool insert)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage
      ("MissionTree::CreateControlLogicMenu() type=%d, insert=%d\n",
       type, insert);
   #endif
   
   wxMenu *menu = new wxMenu;
   bool addElse = false;
   
   if (type == GmatTree::IF_CONTROL || type == GmatTree::END_IF_CONTROL)
   {
      addElse = true;
      wxTreeItemId itemId = GetSelection();
      wxTreeItemId parentId = itemId;
      
      if (type == GmatTree::END_IF_CONTROL)
         parentId = GetItemParent(itemId);

      // show only one Else
      if (FindChild(parentId, "Else").IsOk())
         addElse = false;
   }

   
   if (insert)
   {
      menu->Append(POPUP_INSERT_IF_STATEMENT, wxT("If"));
      menu->Append(POPUP_INSERT_IF_ELSE_STATEMENT, wxT("If-Else"));
      
      if (addElse)
         menu->Append(POPUP_INSERT_ELSE_STATEMENT, wxT("Else"));
      
      menu->Append(POPUP_INSERT_FOR_LOOP, wxT("For"));
      menu->Append(POPUP_INSERT_WHILE_LOOP, wxT("While")); 
   }
   else
   {
      menu->Append(POPUP_ADD_IF_STATEMENT, wxT("If"));
      menu->Append(POPUP_ADD_IF_ELSE_STATEMENT, wxT("If-Else"));
      
      if (addElse)
         menu->Append(POPUP_ADD_ELSE_STATEMENT, wxT("Else"));
      
      menu->Append(POPUP_ADD_FOR_LOOP, wxT("For"));
      menu->Append(POPUP_ADD_WHILE_LOOP, wxT("While")); 
   }
   
   return menu;
}


//------------------------------------------------------------------------------
// void OnDelete()
//------------------------------------------------------------------------------
void MissionTree::OnDelete(wxCommandEvent &event)
{
   // get selected item
   wxTreeItemId itemId = GetSelection();
   
   // if panel is open close it
   OnClose(event);
   
   // delete from gui interpreter
   MissionTreeItemData *missionItem = (MissionTreeItemData *)GetItemData(itemId);
   if (missionItem == NULL)
      return;   // error
     
   GmatCommand *theCmd = missionItem->GetCommand();  
   if (theCmd == NULL)
      return;  //error 
   
   theGuiInterpreter->DeleteCommand(theCmd);
   
   // delete from tree - if parent only has 1 child collapse
   wxTreeItemId parentId = GetItemParent(itemId);
   if (GetChildrenCount(parentId) <= 1)
      this->Collapse(parentId);
      
   this->Delete(itemId);  
}


//---------------------------------------------------------------------------
// void MissionTree::OnRun()
//--------------------------------------------------------------------------
void MissionTree::OnRun(wxCommandEvent &event)
{
   theGuiInterpreter->RunMission();
}


//---------------------------------------------------------------------------
// void MissionTree::OnShowScript()
//--------------------------------------------------------------------------
void MissionTree::OnShowScript(wxCommandEvent &event)
{
   std::string str = theGuiInterpreter->GetScript();

   ViewTextFrame *vtf =
      new ViewTextFrame(GmatAppData::GetMainFrame(), _T("Show Script"),
       50, 50, 800, 500, "Temporary", "Script");
   
   vtf->AppendText(str.c_str());
   vtf->Show(true);
   
}


//---------------------------------------------------------------------------
// bool MissionTree::CheckClickIn(wxPoint position)
//--------------------------------------------------------------------------
bool MissionTree::CheckClickIn(wxPoint position)
{
   //MessageInterface::ShowMessage("Click position is %d %d\n", position.x, position.y );
   //MissionTreeItemData *missionTreeItem = (MissionTreeItemData*) GetFirstVisibleItem();
   wxTreeItemId visibleItemId = GetFirstVisibleItem();
   MissionTreeItemData *missionTreeItemData = 
      (MissionTreeItemData*) GetItemData(visibleItemId);
   //MessageInterface::ShowMessage("Got first visible");

   // loop through all the visible items on the mission tree
   // to compare the event click with the position of the box
   while (missionTreeItemData != NULL)
   {
      int dataType = missionTreeItemData->GetDataType();
      // don't have to open any panels for top folders
      if ((dataType != GmatTree::MISSIONS_FOLDER)       &&
          (dataType != GmatTree::MISSION_SEQ_TOP_FOLDER)&&
          (dataType != GmatTree::MISSION_SEQ_SUB_FOLDER)&&
          (dataType != GmatTree::MISSION_SEQ_COMMAND))
      {
         // get the surrounding box to compare click and commands
         wxRect bound;
         int w, h;
       
         GetBoundingRect(visibleItemId, bound, TRUE);
         GetSize(&w, &h);
       
         // compare event click to see if it is in the box or the
         // icon which is size 16
         if ((position.x >= (bound.x - 16)) &&
             (position.x <= w-offset) &&
             (position.y <= bound.y+rowHeight+1) &&
             (position.y >= bound.y-1))
         {
            //MessageInterface::ShowMessage("\nInside a rect\n");
              
            // set this item selected
            SelectItem(visibleItemId);
              
            // now that we know it is in a box, check to see
            // which box it is in
            // we only need to compare the left and the right, because
            // we already know it is within the top and the bottom
          
            // get box width
            int boxWidth = GetParameter(BOXWIDTH);
 
            // box count is 2, rightmost is for variables
            // next is goals, and the rest is the cmd panel
            int boxNum = 0;
          
            // check if in variables
            if ((position.x <= w-offset-boxWidth*boxNum) &&
                (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside variables");
               MissionTreeItemData *item =
                  new MissionTreeItemData(
                                          wxT("Variables"),
                                          GmatTree::VIEW_SOLVER_VARIABLES);
               GmatAppData::GetMainFrame()->CreateChild(item);
            }
            else if ((position.x <= w-offset-boxWidth*boxNum) &&
                     (position.x >= w-offset-boxWidth*(++boxNum)))
            {
               //MessageInterface::ShowMessage("\nInside goals");
               MissionTreeItemData *item =
                  new MissionTreeItemData(wxT("Goals"),
                                          GmatTree::VIEW_SOLVER_GOALS);
               GmatAppData::GetMainFrame()->CreateChild(item);
            }
            else
            {
               //MessageInterface::ShowMessage("\nOpen regular panel");
               //mainNotebook->CreatePage(missionTreeItemData);
               GmatAppData::GetMainFrame()->CreateChild(missionTreeItemData);
            }
            
            // get out of while loop
            break;
         }
      }
      //MessageInterface::ShowMessage("Not equal to null");
      visibleItemId = GetNextVisible(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible id");
      missionTreeItemData = (MissionTreeItemData*) GetItemData(visibleItemId);
      //MessageInterface::ShowMessage("Got next visible data");
   }

   return false;
}

//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnOpen(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   //    mainNotebook->CreatePage(item);
   GmatAppData::GetMainFrame()->CreateChild(item);
}

//------------------------------------------------------------------------------
// void OnClose()
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 */
//------------------------------------------------------------------------------
void MissionTree::OnClose(wxCommandEvent &event)
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
// int GetMenuId(const wxString &cmd, bool insert)
//------------------------------------------------------------------------------
int MissionTree::GetMenuId(const wxString &cmd, bool insert)
{
   #if DEBUG_MISSION_TREE_MENU
   MessageInterface::ShowMessage("MissionTree::GetMenuId() cmd=%s, insert=%d\n",
                                 cmd.c_str(), insert);
   #endif
   
   int id = 0;
   
   for (unsigned int i=0; i<mCommandList.Count(); i++)
   {
      if (insert)
      {
         if (cmd == "Propagate")
            return POPUP_INSERT_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_INSERT_MANEUVER;
         else if (cmd == "BeginFiniteBurn")
            return POPUP_INSERT_BEGIN_FINITE_BURN;
         else if (cmd == "EndFiniteBurn")
            return POPUP_INSERT_END_FINITE_BURN;
         else if (cmd == "Target")
            return POPUP_INSERT_TARGET;
         else if (cmd == "CallFunction")
            return POPUP_INSERT_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_INSERT_ASSIGNMENT;
         else if (cmd == "Toggle")
            return POPUP_INSERT_TOGGLE;
         else if (cmd == "Save")
            return POPUP_INSERT_SAVE;
         else if (cmd == "ScriptEvent")
            return POPUP_INSERT_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
      else
      {
         if (cmd == "Propagate")
            return POPUP_ADD_PROPAGATE;
         else if (cmd == "Maneuver")
            return POPUP_ADD_MANEUVER;
         else if (cmd == "BeginFiniteBurn")
            return POPUP_ADD_BEGIN_FINITE_BURN;
         else if (cmd == "EndFiniteBurn")
            return POPUP_ADD_END_FINITE_BURN;
         else if (cmd == "Target")
            return POPUP_ADD_TARGET;
         else if (cmd == "CallFunction")
            return POPUP_ADD_FUNCTION;
         else if (cmd == "GMAT")
            return POPUP_ADD_ASSIGNMENT;
         else if (cmd == "Toggle")
            return POPUP_ADD_TOGGLE;
         else if (cmd == "Save")
            return POPUP_ADD_SAVE;
         else if (cmd == "ScriptEvent")
            return POPUP_ADD_SCRIPT_EVENT;
         else
            MessageInterface::ShowMessage
               ("MissionTree::GetMenuId() Unknown command:%s\n", cmd.c_str());
      }
   }

   return id;
}


//------------------------------------------------------------------------------
// GmatTree::MissionIconType GetIconId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::MissionIconType MissionTree::GetIconId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::MISSION_ICON_PROPAGATE_EVENT;
   if (cmd == "Maneuver")
      return GmatTree::MISSION_ICON_DELTA_V;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::MISSION_ICON_BEGIN_FB;
   if (cmd == "EndFiniteBurn")
      return GmatTree::MISSION_ICON_END_FB;
   if (cmd == "Target")
      return GmatTree::MISSION_ICON_TARGET;
   if (cmd == "EndTarget")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "Achieve")
      return GmatTree::MISSION_ICON_ACHIEVE;
   if (cmd == "Vary")
      return GmatTree::MISSION_ICON_VARY;
   if (cmd == "Save")
      return GmatTree::MISSION_ICON_SAVE;
   if (cmd == "GMAT")
      return GmatTree::MISSION_ICON_ASSIGNMENT;
   if (cmd == "Toggle")
      return GmatTree::MISSION_ICON_TOGGLE;
   if (cmd == "For")
      return GmatTree::MISSION_ICON_FOLDER;
   if (cmd == "EndFor")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "If")
      return GmatTree::MISSION_ICON_FOLDER;
   if (cmd == "IfElse")
      return GmatTree::MISSION_ICON_FOLDER;
   if (cmd == "Else")
      return GmatTree::MISSION_ICON_FOLDER;
   if (cmd == "EndIf")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "While")
      return GmatTree::MISSION_ICON_WHILE;
   if (cmd == "EndWhile")
      return GmatTree::MISSION_ICON_NEST_RETURN;
   if (cmd == "CallFunction")
      return GmatTree::MISSION_ICON_FUNCTION;
   
   return GmatTree::MISSION_ICON_FILE;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetCommandId(const wxString &cmd)
//------------------------------------------------------------------------------
GmatTree::ItemType MissionTree::GetCommandId(const wxString &cmd)
{
   if (cmd == "Propagate")
      return GmatTree::PROPAGATE_COMMAND;
   if (cmd == "Maneuver")
      return GmatTree::MANEUVER_COMMAND;
   if (cmd == "BeginFiniteBurn")
      return GmatTree::BEGIN_FINITE_BURN_COMMAND;
   if (cmd == "EndFiniteBurn")
      return GmatTree::END_FINITE_BURN_COMMAND;
   if (cmd == "Target")
      return GmatTree::TARGET_COMMAND;
   if (cmd == "EndTarget")
      return GmatTree::END_TARGET_COMMAND;
   if (cmd == "Achieve")
      return GmatTree::ACHIEVE_COMMAND;
   if (cmd == "Vary")
      return GmatTree::VARY_COMMAND;
   if (cmd == "Save")
      return GmatTree::SAVE_COMMAND;
   if (cmd == "Toggle")
      return GmatTree::TOGGLE_COMMAND;
   if (cmd == "For")
      return GmatTree::FOR_CONTROL;
   if (cmd == "EndFor")
      return GmatTree::END_FOR_CONTROL;
   if (cmd == "If")
      return GmatTree::IF_CONTROL;
   if (cmd == "IfElse")
      return GmatTree::IF_CONTROL;
   if (cmd == "Else")
      return GmatTree::ELSE_CONTROL;
   if (cmd == "EndIf")
      return GmatTree::END_IF_CONTROL;
   if (cmd == "While")
      return GmatTree::WHILE_CONTROL;
   if (cmd == "EndWhile")
      return GmatTree::END_WHILE_CONTROL;
   if (cmd == "CallFunction")
      return GmatTree::CALL_FUNCTION_COMMAND;
   
   return GmatTree::SCRIPT_COMMAND;
}


//------------------------------------------------------------------------------
// int* GetCommandCounter(const wxString &cmd)
//------------------------------------------------------------------------------
int* MissionTree::GetCommandCounter(const wxString &cmd)
{
   if (cmd == "Propagate")
      return &mNumPropagate;
   if (cmd == "Maneuver")
      return &mNumManeuver;
   if (cmd == "BeginFiniteBurn")
      return &mNumFiniteBurn;
   if (cmd == "Target")
      return &mNumTarget;
   if (cmd == "EndTarget")
      return &mNumTarget;
   if (cmd == "Achieve")
      return &mNumAchieve;
   if (cmd == "Vary")
      return &mNumVary;
   if (cmd == "Save")
      return &mNumSave;
   if (cmd == "Toggle")
      return &mNumToggle;
   if (cmd == "For")
      return &mNumForLoop;
   if (cmd == "EndFor")
      return &mNumForLoop;
   if (cmd == "If")
      return &mNumIfStatement;
   if (cmd == "IfElse")
      return &mNumIfStatement;
   if (cmd == "Else")
      return &mNumIfStatement;
   if (cmd == "EndIf")
      return &mNumIfStatement;
   if (cmd == "While")
      return &mNumWhileLoop;
   if (cmd == "EndWhile")
      return &mNumWhileLoop;
   if (cmd == "CallFunction")
      return &mNumFunct;
   if (cmd == "GMAT")
      return &mNumAssign;

   return &mNumScriptEvent;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindChild(wxTreeItemId parentId, const wxString &cmd)
//------------------------------------------------------------------------------
wxTreeItemId MissionTree::FindChild(wxTreeItemId parentId, const wxString &cmd)
{
   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage
      ("\nMissionTree::FindChild() parentId=<%s>, cmd=<%s>\n",
       GetItemText(parentId).c_str(), cmd.c_str());
   #endif
   
   wxTreeItemIdValue cookie;
   MissionTreeItemData *item;
   wxTreeItemId childId = GetFirstChild(parentId, cookie);

   #if DEBUG_MISSION_TREE_FIND
   MessageInterface::ShowMessage 
      ("MissionTree::FindChild() childId=<%s>\n", GetItemText(childId).c_str());
   #endif
   
   while (childId.IsOk())
   {
      item = (MissionTreeItemData *)GetItemData(childId);
       
      #if DEBUG_MISSION_TREE
      MessageInterface::ShowMessage("MissionTree::FindChild() while-loop, "
                    "item->GetDesc(): \"%s\"\n",item->GetDesc().c_str());
      #endif

      if (item->GetDesc().Contains(cmd))
         return childId;
      
      childId = GetNextChild(parentId, cookie);
      
      #if DEBUG_MISSION_TREE_FIND > 1
      MessageInterface::ShowMessage("MissionTree::FindChild() childId=<%s>\n", 
                                    GetItemText(childId).c_str());
      #endif
   }
   
   return childId;

}


// for Debug
//------------------------------------------------------------------------------
// void ShowCommands(const wxString &msg = "")
//------------------------------------------------------------------------------
void MissionTree::ShowCommands(const wxString &msg)
{
   MessageInterface::ShowMessage("-------------------->%s\n", msg.c_str());
   
   GmatCommand *cmd = theGuiInterpreter->GetNextCommand();;
   
   while (cmd != NULL)
   {
      MessageInterface::ShowMessage("----- %s\n", cmd->GetTypeName().c_str());

      if ((cmd->GetChildCommand(0)) != NULL)
         ShowSubCommands(cmd, 0);
      
      cmd = cmd->GetNext();
   }
   
   MessageInterface::ShowMessage("<--------------------\n");
}


//------------------------------------------------------------------------------
// void ShowSubCommands(GmatCommand* brCmd, Integer level)
//------------------------------------------------------------------------------
void MissionTree::ShowSubCommands(GmatCommand* brCmd, Integer level)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
            MessageInterface::ShowMessage("-----");
         
         MessageInterface::ShowMessage("----- %s\n", nextInBranch->GetTypeName().c_str());
         
         if (nextInBranch->GetChildCommand() != NULL)
            ShowSubCommands(nextInBranch, level+1);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
   
}
