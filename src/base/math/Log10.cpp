//------------------------------------------------------------------------------
//                                  Log10
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/14
//
/**
 * Implements Log10 class.
 */
//------------------------------------------------------------------------------

#include "Log10.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Log10()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
Log10::Log10(const std::string &nomme)
   : MathFunction("Log10", nomme)
{
}


//------------------------------------------------------------------------------
// ~Log10()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Log10::~Log10()
{
}


//------------------------------------------------------------------------------
//  Log10(const Log10 &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the Log10 object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
Log10::Log10(const Log10 &copy) :
   MathFunction      (copy)
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the Log10 operation.
 *
 * @return clone of the Log10 operation.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Log10::Clone() const
{
   return (new Log10(*this));
}


//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void Log10::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   Integer type1, row1, col1; // Left node
   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
   // Get the type(Real or Matrix), # rows and # columns of the right node
   rightNode->GetOutputInfo(type2, row2, col2);

   if ((type1 != type2) || (row1 != row2) || (col1 != col2))
      throw MathException("Matrixes are not the same can not add.\n");    
   else
   {
      type = type1;
      rowCount = row1;
      colCount = col1;
   }
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool Log10::ValidateInputs()
{
   Integer type1, row1, col1; // Left node
//   Integer type2, row2, col2; // Right node
   
   // Get the type(Real or Matrix), # rows and # columns of the left node
   leftNode->GetOutputInfo(type1, row1, col1);
   
//   // Get the type(Real or Matrix), # rows and # columns of the right node
//   rightNode->GetOutputInfo(type2, row2, col2);

//   if ((type1 == Gmat::REAL_TYPE) && (type2 == Gmat::REAL_TYPE))
   if (type1 == Gmat::REAL_TYPE)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
/**
 * @return the base 10 logarithm.
 *
 */
//------------------------------------------------------------------------------
Real Log10::Evaluate()
{
   return GmatMathUtil::Log10(leftNode->Evaluate());
}

