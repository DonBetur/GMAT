//$Header$
//------------------------------------------------------------------------------
//                               BranchCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Base class implementation for the Command classes that branch (Target, If, 
 * While, etc).
 */
//------------------------------------------------------------------------------


// #define DEBUG_BRANCHCOMMAND_DEALLOCATION


#include "BranchCommand.hpp"

#define DEBUG_BRANCHCOMMAND_DEALLOCATION

#ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
   #include "MessageInterface.hpp"
#endif

BranchCommand::BranchCommand(const std::string &typeStr) :
   GmatCommand          (typeStr),
   branch               (1),
   commandComplete      (false),
   commandExecuting     (false),
   branchToFill         (0),
   nestLevel            (0)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
}


BranchCommand::~BranchCommand()
{
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage("In BranchCommand::~BranchCommand()\n");
   #endif
   std::vector<GmatCommand*>::iterator node;
   GmatCommand* current;
      
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      // Find the end for each branch and disconnect it fron the start
      current = *node;
      while (current->GetNext() != this) {
         current = current->GetNext();
         if (current == NULL)
{
MessageInterface::ShowMessage("current == NULL\n");
            break;
}
      }
         
      // Calling Remove this way just sets the next pointer to NULL
      if (current) {
         #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
            MessageInterface::ShowMessage("Removing %s\n", 
                                       current->GetTypeName().c_str());
         #endif
         current->Remove(current);  
      }
      
      if (*node)
         delete *node;
   }
   #ifdef DEBUG_BRANCHCOMMAND_DEALLOCATION
      MessageInterface::ShowMessage("Finished BranchCommand::~BranchCommand()\n");
   #endif
}


BranchCommand::BranchCommand(const BranchCommand& bc) :
   GmatCommand       (bc),
   branch            (1),
   commandComplete   (false),
   commandExecuting  (false),
   branchToFill      (0),
   nestLevel        (bc.nestLevel)
{
   depthChange = 1;
   parameterCount = BranchCommandParamCount;
}


BranchCommand& BranchCommand::operator=(const BranchCommand& bc)
{
   if (this == &bc)
      return *this;

   GmatCommand::operator=(bc);
   return *this;
}


GmatCommand* BranchCommand::GetNext(void)
{
   // Return the next pointer in the command sequence if this command -- 
   // includng its branches -- has finished executing.
   if ((commandExecuting) && (!commandComplete))
      return this;
   
   return next;
}


GmatCommand* BranchCommand::GetChildCommand(Integer whichOne)
{
   if (whichOne > (Integer)(branch.size())-1)
      return NULL;
   return branch[whichOne];
}


bool BranchCommand::Initialize(void)
{
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *current;
   bool retval = true;
   
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;
//      while ((current != NULL) && (current != this))
      while (current != this)
      {
         if (!current->Initialize())
               retval = false;
         current = current->GetNext();
         if (current == NULL)
            throw CommandException("Branch command \"" + generatingString +
                                   "\" was not terminated!");
      }
   }
   
   commandExecuting = false;
   return retval;
}


void BranchCommand::AddBranch(GmatCommand *cmd, Integer which)
{
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
      branch.reserve(which+1);
   
   if (which == (Integer)(branch.size()))
      branch.push_back(cmd);   
   else if (branch[which] == NULL)
      branch.at(which) = cmd;
      //branch[which] = cmd;
   else
      (branch.at(which))->Append(cmd);
      //branch[which]->Append(cmd);
}

void BranchCommand::AddToFrontOfBranch(GmatCommand *cmd, Integer which)
{
   // Increase the size of the vector if it's not big enough
   if (which >= (Integer)branch.capacity())
      branch.reserve(which+1);
   
   if (which == (Integer)(branch.size()))
      branch.push_back(cmd);   
   else if (branch.at(which) == NULL)
      branch.at(which) = cmd;
   else
   {
      GmatCommand* tmp = branch.at(which);
      branch.at(which) = cmd;
      cmd->Append(tmp);
   }
}


bool BranchCommand::Append(GmatCommand *cmd)
{
   // If we are still filling in a branch, append on that branch
   if (branchToFill >= 0)
   {
      AddBranch(cmd, branchToFill);
      return true;
   }
   
   // Otherwise, just call the base class method
   return GmatCommand::Append(cmd);
}


bool BranchCommand::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   GmatCommand *current = NULL;
   
   // See if we're supposed to put it at the top of the first branch
   if (prev == this)
   {
      
      current = branch[0];
      branch[0] = cmd;
      cmd->Append(current);
      return true;
   }
   // see if we're supposed to add it to the front of a branch
   // i.e. the prev = the last command in the previous branch
   // (e.g. an Else command)
   // check all but the last branch - End*** should take care of it
   // at that point
   for (Integer br = 0; br < (Integer) (branch.size() - 1); ++br)
   {
      current = branch.at(br);
      if (current != NULL)
      {
         while (current->GetNext() != this)
            current = current->GetNext();
         if (current == prev) 
         {
            AddToFrontOfBranch(cmd,br+1);
            return true;
         }
      }
   }
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      if (current != NULL)
         if (current->Insert(cmd, prev))
               return true;
   }
   
   // Otherwise, just call the base class method
   return GmatCommand::Insert(cmd, prev);
}


GmatCommand* BranchCommand::Remove(GmatCommand *cmd)
{
   if (cmd == this)
      return GmatCommand::Remove(cmd);    // Use base method to remove cmd
   
   GmatCommand *fromBranch = NULL;
   GmatCommand *current = NULL;
   
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      if (current != NULL)
      {
         fromBranch = current->Remove(cmd);
         if (fromBranch != NULL)
               return fromBranch;
      }
   }
   
   // Not in the branches, so continue with the sequence
   return GmatCommand::Remove(cmd);
}

bool BranchCommand::InsertRightAfter(GmatCommand *cmd)
{
   if (!next) 
   {
      next = cmd;
      return true;
   }
   
   return GmatCommand::Insert(cmd,this);
}


void BranchCommand::SetSolarSystem(SolarSystem *ss)
{
   GmatCommand::SetSolarSystem(ss);
   GmatCommand *current = NULL;
   
   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetSolarSystem(ss);;
         current = current->GetNext();
      }
   }
}


//------------------------------------------------------------------------------
//  void SetAssetMap(std::map<std::string, Asset *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the Command.  This
 * implementation chains through the branches and sets the asset map for each of
 * the branch nodes.
 * 
 * @param <map> Pointer to the local asset map
 */
//------------------------------------------------------------------------------
void BranchCommand::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   GmatCommand::SetObjectMap(map);
   GmatCommand *current = NULL;
   
   // Set it for all of the branch nodes
   // If we have branches, try to insert there first
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         current->SetObjectMap(map);
         current = current->GetNext();
      }
   }
}


const std::string& BranchCommand::GetGeneratingString(void)
{
   fullString = generatingString;
   GmatCommand *current;
   
   // Loop through the branches, appending the strings from commands in each
   for (Integer which = 0; which < (Integer)branch.size(); ++which)
   {
      current = branch[which];
      while ((current != NULL) && (current != this))
      {
         fullString += "\n";
         fullString += current->GetGeneratingString();
         current = current->GetNext();
      }
   }
   
   return fullString;
}


bool BranchCommand::Execute(void)
{
   commandExecuting = true;
   return true;
}


bool BranchCommand::ExecuteBranch(Integer which)
{
   bool retval = true;
   
   GmatCommand *current = branch[which];
   while ((current != NULL) && (current != this))
   {
      if (current->Execute() == false)
      {
         retval = false;
         break;
      }
      // May need to add a test for user interruption here
      current = current->GetNext();
   }
   
   return retval;
}



