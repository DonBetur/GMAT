//$Header$
//------------------------------------------------------------------------------
//                             EndFor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus
// Created: 2004/01/29
// Modified: W. Shoan 2004.09.13 - Updated for use in Build 3
//
/**
 * Definition for the closing line of a for loop
 */
//------------------------------------------------------------------------------


#include "EndFor.hpp"
#include "BranchCommand.hpp"


EndFor::EndFor(void) :
    GmatCommand         ("EndFor")
{
   depthChange = -1;
}


EndFor::~EndFor(void)
{
}
    

EndFor::EndFor(const EndFor& ef) :
    GmatCommand         (ef)
{
}


EndFor& EndFor::operator=(const EndFor& ef)
{
   if (this == &ef)
     return *this;

   GmatCommand::operator=(ef);
   return *this;
}
    

bool EndFor::Initialize(void)
{
   // Validate that next points to the owning for command
   if (!next)
     throw CommandException("EndFor Command not properly reconnected");

   if (next->GetTypeName() != "For")
     throw CommandException("EndFor Command not connected to For Command");
                          

   return true;    
}


bool EndFor::Execute(void)
{
   return true;
}

bool EndFor::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   if (this == prev) return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EndFor.
 *
 * @return clone of the EndFor.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EndFor::Clone(void) const
{   
   return (new EndFor(*this));
}

