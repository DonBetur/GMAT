//$Header$
//------------------------------------------------------------------------------
//                                Assignment
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Implementation for the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 * 
 * Eventually (currently scheduled for build 4) this class will be extended to 
 * include numerical operators on the right side of the assignment line.
 */
//------------------------------------------------------------------------------


#include "Assignment.hpp"
#include "MessageInterface.hpp"
#include "Parameter.hpp"

//#define DEBUG_RENAME 1
//#define DEBUG_PARM_ASSIGNMENT
//#define DEBUG_ARRAY_INTERPRETING
//------------------------------------------------------------------------------
//  Assignment()
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (default constructor).
 */
//------------------------------------------------------------------------------
Assignment::Assignment  () :
   GmatCommand          ("GMAT"),
   ownerName            (""),
   parmName             (""),
   parmOwner            (NULL),
   rhsObject            (NULL),
   rhsType              (NUMBER),
   rhsParmName          (""),
   objToObj             (false),
   parmID               (-1),
   parmType             (Gmat::UNKNOWN_PARAMETER_TYPE),
   value                ("Not_Set"),
   row                  (0),
   col                  (0),
   rowObj               (NULL),
   colObj               (NULL),
   lrowObj              (NULL),
   lcolObj              (NULL),
   rowIndex             (0),
   colIndex             (0),
   isLhsArray           (false)
{
}


//------------------------------------------------------------------------------
//  ~Assignment()
//------------------------------------------------------------------------------
/**
 * Destroys the Assignment command (destructor).
 */
//------------------------------------------------------------------------------
Assignment::~Assignment()
{
}


//------------------------------------------------------------------------------
//  Assignment(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (copy constructor).
 *
 * @param <a> The instance that is copied.
 */
//------------------------------------------------------------------------------
Assignment::Assignment  (const Assignment& a) :
   GmatCommand          (a),
   ownerName            (a.ownerName),
   parmName             (a.parmName),
   parmOwner            (a.parmOwner),
   rhsObject            (a.rhsObject),
   rhsType              (a.rhsType),
   rhsParmName          (a.rhsParmName),
   parmID               (a.parmID),
   parmType             (a.parmType),
   value                (a.value),
   row                  (a.row),
   col                  (a.col),
   rowObj               (NULL),
   colObj               (NULL),
   lrow                 (a.lrow),
   lcol                 (a.lcol),
   lrowObj              (NULL),
   lcolObj              (NULL),
   rowIndex             (a.rowIndex),
   colIndex             (a.colIndex),
   isLhsArray           (a.isLhsArray)
{
}


//------------------------------------------------------------------------------
//  Assignment& operator=(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Assignment assignment operator.
 *
 * @param <a> The instance that is copied.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
Assignment& Assignment::operator=(const Assignment& a)
{
   if (this == &a)
      return *this;
        
   ownerName   = a.ownerName;
   parmName    = a.parmName;
   parmOwner   = a.parmOwner;
   rhsObject   = a.rhsObject;
   rhsType     = a.rhsType;
   rhsParmName = a.rhsParmName;
   objToObj    = a.objToObj;
   parmID      = a.parmID;
   parmType    = a.parmType;
   value       = a.value;
   row         = a.row;
   col         = a.col;
   rowObj      = NULL;
   colObj      = NULL;
   lrow        = a.lrow;
   lcol        = a.lcol;
   lrowObj     = NULL;
   lcolObj     = NULL;
   rowIndex    = a.rowIndex;
   colIndex     =a.colIndex;
   isLhsArray  = a.isLhsArray;

   return *this;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * Assignment lines have the form
 *
 *    GMAT Sat.Element1 = 7654.321;
 *
 * or
 *
 *    GMAT object1 = object2;
 *
 * This method breaks this line into its constituent pieces.
 *
 * @return true on successful parsing of the command.
 *
 * @todo: Determine if Assignment::InterpretAction can be moved into the
 *        Interpreter
 */
//------------------------------------------------------------------------------
bool Assignment::InterpretAction()
{
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage(
         "Assignment::InterpretAction entered\n");
   #endif

   Integer end = generatingString.find("%");
   std::string genString = generatingString.substr(0, end);
   
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "Comment-free generating string is %s\n", genString.c_str());
   #endif

   Integer loc = genString.find("GMAT", 0) + 4;
   const char *str = genString.c_str();
   while (str[loc] == ' ')
      ++loc;

   std::string lhs, rhs;
   UnsignedInt eqloc = genString.find("=", loc), lend, rstart;

   // Check to be sure equal sign is in place
   if (eqloc == std::string::npos)
      throw CommandException("Assignment string does not set value");
      
   lend = eqloc;
   rstart = eqloc;
   while ((str[lend] == '=') || (str[lend] == ' '))
      --lend;
   while ((str[rstart] == '=') || (str[rstart] == ' '))
      ++rstart;
   lhs = genString.substr(loc, lend - loc + 1);
   rhs = genString.substr(rstart);
   isLhsArray = false;

   if (lhs.find("(", 0) != std::string::npos)
      isLhsArray = true;

   // Strip off trailing spaces and semicolon from RHS
   end = rhs.find(";");
   rhs = rhs.substr(0, end);
   lend = rhs.length() - 1;
   while (rhs[lend] == ' ')
      --lend;
   rhs = rhs.substr(0, lend + 1);

   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "Left side is \"%s\"\nRight side is \"%s\"\nLeft side %s an array\n",
         lhs.c_str(), rhs.c_str(), (isLhsArray ? "is" : "is not"));
   #endif

   end = genString.find(".", loc);
   if ((end == (Integer)std::string::npos) && !isLhsArray)
   {
      // Script line is set to make one object the same as another, or to set a
      // value on a variable
      ownerName = lhs;
      value = rhs;
      objToObj = true;

      return true;
   }

   // Parse array handling elements
   if (isLhsArray)
   {
      // Break apart the array text into owner, row, and column
      UnsignedInt paren, comma, closeparen, temp;
      paren = lhs.find("(");
      comma = lhs.find(",");
      closeparen = lhs.find(")");

      if ((comma == std::string::npos) || (closeparen == std::string::npos))
         throw CommandException("Syntax error in the assignment \"" +
            generatingString +
            "\"\nArray assignments must specify row and column, separated by" +
            " a comma, in parentheses.");

      ownerName = lhs.substr(0, paren);
      lrow      = lhs.substr(paren + 1, comma - (paren + 1));
      lcol      = lhs.substr(comma + 1, closeparen - (comma + 1));
      
      // Trim the white space
      temp = 0;
      while (ownerName[temp] == ' ')
         ++temp;
      ownerName = ownerName.substr(temp);
      temp = ownerName.length();
      while (ownerName[temp] == ' ')
         --temp;
      ownerName = ownerName.substr(0, temp);

      temp = 0;
      while (lrow[temp] == ' ')
         ++temp;
      lrow = lrow.substr(temp);
      temp = lrow.length();
      while (lrow[temp] == ' ')
         --temp;
      lrow = lrow.substr(0, temp+1);

      temp = 0;
      while (lcol[temp] == ' ')
         ++temp;
      lcol = lcol.substr(temp);
      temp = lcol.length();
      while (lcol[temp] == ' ')
         --temp;
      lcol = lcol.substr(0, temp+1);

      value = rhs;

      #ifdef DEBUG_ARRAY_INTERPRETING
         MessageInterface::ShowMessage(
            "\n   Array is \"%s\", row \"%s\", column \"%s\"\n",
            ownerName.c_str(), lrow.c_str(), lcol.c_str());
      #endif

      return true;
   }

   end = lhs.find(".", 0);
   std::string component = lhs.substr(0, end);
   if (component == "")
      throw CommandException("Assignment string does not identify object");
   ownerName = component;
   loc = end + 1;

   component = lhs.substr(loc);
   if (component == "")
      throw CommandException("Assignment string does not identify parameter");
   parmName = component;

   // Strip off trailing white space, if any
   unsigned n = parmName.length() - 1;
   while ((parmName[n] == ' ') || (parmName[n] == '\t'))
      --n;
   parmName = parmName.substr(0, n+1);

   value = rhs;
   
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage(
         "\nOwner is \"%s\"\nParameter is \"%s\"\nValue is \"%s\"\n",
         ownerName.c_str(), parmName.c_str(), value.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Assignment command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Assignment::Initialize()
{
   // Find the object
   if (objectMap->find(ownerName) == objectMap->end())
      throw CommandException("Assignment command cannot find object \"" +
                             ownerName + "\" for line \n" + generatingString);

   parmOwner = (*objectMap)[ownerName];
   if (objToObj)
   {
      if (objectMap->find(value) != objectMap->end())
      {
         rhsObject = (*objectMap)[value];
         return true;
      }
      else
      {
         if (parmOwner->GetTypeName() == "Variable")
         {
            parmName = "Expression";
            objToObj = false;
            #ifdef DEBUG_PARM_ASSIGNMENT
               MessageInterface::ShowMessage(
                  "Assignment::Initialize has owner %s, name %s, and val %s\n",
                  ownerName.c_str(), parmName.c_str(), value.c_str());
            #endif
         }
         else
            throw CommandException("Assignment command cannot find object \"" +
                     value + "\" for line \n" + generatingString);
      }
   }
   
   if (isLhsArray)
   {
      if (parmOwner->GetTypeName() != "Array")
         throw CommandException(
            "Attemping to treat " + parmOwner->GetTypeName() + " named " +
            parmOwner->GetName() + " like an Array object.");\

      // Build row index
      if (lrow == ":")
         rowIndex = -1;
      else
      {
         if (objectMap->find(lrow) == objectMap->end())
         {
            rowIndex = atoi(lrow.c_str());
            if (rowIndex < 0)
               throw CommandException(
                  "Attempting to use an invalid (negative) row index for " +
                  ownerName);
            if (rowIndex == 0)
               throw CommandException(
                  "Attempting to use an invalid row index (0 -- arrays are "
                  "indexed from 1) for " + ownerName);
         }
         else
            lrowObj = (*objectMap)[lrow];
      }
      
      // Build column index
      if (lcol == ":")
         colIndex = -1;
      else
      {
         if (objectMap->find(lcol) == objectMap->end())
         {
            colIndex = atoi(lcol.c_str());
            if (colIndex < 0)
               throw CommandException(
                  "Attempting to use an invalid (negative) column index for " +
                  ownerName);
            if (colIndex == 0)
               throw CommandException(
                  "Attempting to use an invalid column index (0 -- arrays "
                  "are indexed from 1) for " + ownerName);
         }
         else
            lcolObj = (*objectMap)[lcol];
      }
   }

   return InitializeRHS(value);
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * The method that is fired to perform the command.
 *
 * Derived classes implement this method to perform their actions on 
 * GMAT objects.
 *
 * @return true if the GmatCommand runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool Assignment::Execute()
{
   #ifdef DEBUG_ARRAY_INTERPRETING
      MessageInterface::ShowMessage("Assignment::Execute entered for " +
         generatingString + "\n");
   #endif
   bool retval = false;

   // Get the parameter ID and ID type
   try
   {
      #ifdef DEBUG_ARRAY_INTERPRETING
         MessageInterface::ShowMessage("   In the try clause\n");
      #endif
      if (parmOwner == NULL)
         throw CommandException("Parameter Owner Not Initialized");
       
      if (isLhsArray)
      {
         #ifdef DEBUG_ARRAY_INTERPRETING
            MessageInterface::ShowMessage("   Executing array branch\n");
         #endif

         if (lrowObj)
            throw CommandException(
               "Objects cannot be used to set row indexes yet.");
         if (lcolObj)
            throw CommandException(
               "Objects cannot be used to set column indexes yet.");

         if (rowIndex == -1)
            throw CommandException(
               "Multiple array row elements cannot be set yet.");
         if (colIndex == -1)
            throw CommandException(
               "Multiple array column elements cannot be set yet.");

         parmOwner->SetRealParameter("SingleValue", EvaluateRHS(), rowIndex-1,
            colIndex-1);

         return true;
      }

      if (objToObj)
      {
         #ifdef DEBUG_ARRAY_INTERPRETING
            MessageInterface::ShowMessage(
               "   Executing object to object branch\n");
         #endif

         if (!rhsObject)
            throw CommandException("Assignment command cannot find object \"" +
               value + "\"");
         if (parmOwner->GetTypeName() != rhsObject->GetTypeName())
            throw CommandException("Mismatched object types between \"" +
               parmOwner->GetName() + "\" and \"" + rhsObject->GetName() +
               "\"");
         parmOwner->Copy(rhsObject);
         return true;
      }
      
      #ifdef DEBUG_ARRAY_INTERPRETING
         MessageInterface::ShowMessage("   Executing parameter setting\n");
      #endif

      parmID    = parmOwner->GetParameterID(parmName);
      parmType  = parmOwner->GetParameterType(parmID);

      #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage("Assignment::Execute Parameter %s has "
            "type %s\n", parmName.c_str(),
            parmOwner->GetParameterTypeString(parmID).c_str());
      #endif

      switch (parmType)
      {
         case Gmat::INTEGER_TYPE:
            parmOwner->SetIntegerParameter(parmID, (Integer)EvaluateRHS());
            retval = true;
            break;
               
         case Gmat::REAL_TYPE:
            parmOwner->SetRealParameter(parmID, EvaluateRHS());
            retval = true;
            break;
               
         case Gmat::STRING_TYPE:
         case Gmat::STRINGARRAY_TYPE:
            #ifdef DEBUG_PARM_ASSIGNMENT
               MessageInterface::ShowMessage("Assignment::Execute setting "
                  "string to %s\n", value.c_str());
            #endif
            parmOwner->SetStringParameter(parmID, value);
            retval = true;
            break;
               
         case Gmat::BOOLEAN_TYPE:
            bool tf;
            if (value == "true")
               tf = true;
            else
               tf = false;
            parmOwner->SetBooleanParameter(parmID, tf);
            retval = true;
            break;
   
         default:
            break;
      }

       // "Add" parameters could also mean to set reference objects
      if (parmName == "Add")
      {
         if (objectMap->find(value) != objectMap->end())
         {
            GmatBase *obj = (*objectMap)[value];
            if (obj)
               parmOwner->SetRefObject(obj, obj->GetType(), value);
         }
      }
   }
   catch (BaseException& ex)
   {
      if (parmOwner == NULL)
         throw;
      // Could be an action rather than a parameter
      if (!parmOwner->TakeAction(parmName, value))
         throw;
       
      retval = true;
   }

   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("Assignment::Execute finished\n");
   #endif
   return retval;
}

//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param <type> Type of the object that is renamed.
 * @param <oldName> The current name for the object.
 * @param <newName> The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Assignment::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   // Assignment needs to know about spacecraft, formation
   if (type != Gmat::SPACECRAFT && type != Gmat::FORMATION )
      return true;

   if (ownerName == oldName)
      ownerName = newName;

   if (parmName == oldName)
      parmName = newName;

   if (value == oldName)
      value = newName;
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Assignment.
 *
 * @return clone of the Assignment.
 */
//------------------------------------------------------------------------------
GmatBase* Assignment::Clone() const
{
   return (new Assignment(*this));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//                                            const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const std::string& Assignment::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   std::string gen = "GMAT " + ownerName;
   if (parmName != "")
      gen += "." + parmName;
   gen += " = " + value + ";";
   
   #ifdef DEBUG_ASSIGNMENT_SCRIPTING
      MessageInterface::ShowMessage("Assignment command generator is \"%s\"\n",
         gen.c_str());
   #endif
   generatingString = gen;

   return generatingString;
}


//------------------------------------------------------------------------------
// bool InitializeRHS(const std::string &rhs)
//------------------------------------------------------------------------------
/**
 * Initializes the objects used on the right side of the assignemnt.
 *
 * @param <rhs> The string on the right side of the equals sign.
 *
 * @return true on success, false on failure.
 *
 * @todo Handle RHS Variables and Parameters
 */
//------------------------------------------------------------------------------
bool Assignment::InitializeRHS(const std::string &rhs)
{
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage("Assignment::InitializeRHS(%s) entered\n",
         rhs.c_str());
   #endif
   
   std::string chunk, kind, subchunk;
   Integer start = 0, end;
   
   // Skip leading spaces
   while (rhs[start] == ' ')
      ++start;
   end = start;
   // Find the next delimiter
   while ((rhs[end] != ';')  && (rhs[end] != ' ') &&
          (rhs[end] != '.')  && (rhs[end] != '=') &&
          (rhs[end] != '(')  && (rhs[end] != ',') &&
          (rhs[end] != '\t') && (end < (Integer)rhs.length()))
      ++end;
      
   chunk = rhs.substr(start, end - start);
   subchunk = rhs.substr(end);
   
   #ifdef DEBUG_PARM_ASSIGNMENT
      MessageInterface::ShowMessage(
         "'%s' was broken into '%s' and '%s'\n", rhs.c_str(), chunk.c_str(),
         subchunk.c_str());
   #endif
   
   if (objectMap->find(chunk) != objectMap->end())
   {
      rhsObject = (*objectMap)[chunk];
      kind = rhsObject->GetTypeName();

      #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage(
            "Assignment RHS object is a %s named %s\n", kind.c_str(),
            rhsObject->GetName().c_str());
      #endif

      // Determine what type of object handling is needed
      if (kind == "Array")
      {
         rhsType = ARRAY_ELEMENT;
         std::string rowStr, colStr = "1";
         
         unsigned openParen, comma, closeParen, index;
         openParen  = subchunk.find("(");
         comma      = subchunk.find(",");
         closeParen = subchunk.find(")");
         
         if ((openParen == std::string::npos) ||
             (closeParen == std::string::npos))
            throw CommandException(
                      "Assignment::InitializeRHS encountered mismatched "
                      "parenthesis\n   Command text is \"" +
                      (GetGeneratingString()) + "\"");
         if (comma != std::string::npos)
         {
            colStr = subchunk.substr(comma+1, closeParen - (comma+1));
            closeParen = comma;
         }
         rowStr = subchunk.substr(openParen+1, closeParen - (openParen+1));
            
         // Trim whitespace
         index = 0;
         while ((rowStr[index] == ' ') && (index < rowStr.length()))
            ++index;
         rowStr = rowStr.substr(index);
         index = rowStr.length() - 1;
         while ((rowStr[index] == ' ') && (index >= 0))
            --index;
         rowStr = rowStr.substr(0, index+1);

         index = 0;
         while ((colStr[index] == ' ') && (index < colStr.length()))
            ++index;
         colStr = colStr.substr(index);
         index = colStr.length() - 1;
         while ((colStr[index] == ' ') && (index >= 0))
            --index;
         colStr = colStr.substr(0, index+1);

         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage(
               "   Array element given by ('%s', '%s')\n", rowStr.c_str(),
               colStr.c_str());
         #endif

         if (objectMap->find(rowStr) == objectMap->end())
         {
            row = atoi(rowStr.c_str()) - 1;
         }
         else
            throw CommandException(
               "Assignment commands cannot handle dynamic row indices yet.");

         if (objectMap->find(colStr) == objectMap->end())
         {
            col = atoi(colStr.c_str()) - 1;
         }
         else
            throw CommandException(
               "Assignment commands cannot handle dynamic column indices yet.");
      }
      else
      {
         if (kind == "Variable")
         {
            throw CommandException(
               "Assignment commands cannot handle Variables yet.");
         }
         else if (rhsObject->GetType() == Gmat::PARAMETER)
         {
            throw CommandException(
               "Assignment commands cannot handle Parameters yet.");
         }
      }
   }
   else
   {
      #ifdef DEBUG_PARM_ASSIGNMENT
         MessageInterface::ShowMessage(
            "Assignment RHS object is the number %le\n", atof(rhs.c_str()));
      #endif
      rhsType = NUMBER;
   }

   return true;
}


//------------------------------------------------------------------------------
// Real EvaluateRHS()
//------------------------------------------------------------------------------
/**
 * Initializes the objects used on the right side of the assignemnt.
 *
 * @return The floating point number corresponding to the right hand side.
 *
 * @todo Handle RHS Variables and Parameters
 *
 * @tod0 Determine how to handle strings in Assignment commands.
 */
//------------------------------------------------------------------------------
Real Assignment::EvaluateRHS()
{
   // RHS could be a parameter, an array element, a variable, or just a number
   switch (rhsType)
   {
      case PARAMETER:
      case VARIABLE:
         return ((Parameter*)rhsObject)->EvaluateReal();

      case ARRAY_ELEMENT:
         if (rowObj != NULL)
            if (rowObj->GetType() == Gmat::PARAMETER)
               row = (Integer)((Parameter*)rowObj)->EvaluateReal();
         if (colObj != NULL)
            if (colObj->GetType() == Gmat::PARAMETER)
               col = (Integer)((Parameter*)colObj)->EvaluateReal();
         #ifdef DEBUG_PARM_ASSIGNMENT
            MessageInterface::ShowMessage(
               "   Getting 'SingleValue' for (%d, %d)\n", row, col);
         #endif
         return rhsObject->GetRealParameter("SingleValue", row, col);

      default:
         break;
   }
   
   // It's just a number
   return atof(value.c_str());
}
