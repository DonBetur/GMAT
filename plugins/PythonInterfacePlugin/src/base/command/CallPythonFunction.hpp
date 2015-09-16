//$Id$
//------------------------------------------------------------------------------
//                                 CallPythonFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II .
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
/**
* Definition for the CallPythonFunction command class
*/
//------------------------------------------------------------------------------
#ifndef CALLPYTHONFUNCTION_HPP
#define CALLPYTHONFUNCTION_HPP

#include "pythoninterface_defs.hpp"
#include "CallFunction.hpp"
#include "PythonInterface.hpp"


class  PYTHON_API CallPythonFunction : public CallFunction
{
public:

   CallPythonFunction();
   CallPythonFunction(const CallPythonFunction &cpf);
   CallPythonFunction& operator=(const CallPythonFunction &cpf);
   virtual ~CallPythonFunction();
	
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
//   virtual std::string  GetStringParameter(const Integer id,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const char *value,
//                                           const Integer index);
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value,
//                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
//   virtual std::string  GetStringParameter(const std::string &label,
//                                           const Integer index) const;
//   virtual bool         SetStringParameter(const std::string &label,
//                                           const std::string &value,
//                                           const Integer index);

   // override GmatCommand methods
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();


	DEFAULT_TO_NO_CLONES

protected:
   /// Module name for the Python code
	std::string moduleName;
	/// Function name for the Python code
	std::string functionName;
   /// input array row
   UnsignedInt inRow;
   /// input array column
   UnsignedInt inCol;
   /// output array row
   UnsignedInt outRow;
   /// output array column
   UnsignedInt outCol;
	/// Python Interface singleton
	PythonInterface *pythonIf;

	/// Holder for return data from Python, for translation to GMAT objects
	struct PyReturnValue
	{
	   /// The type GMAT expects, based on what Python returned
	   Gmat::ParameterType toType;
	   /// The container for numeric returns
	   RealArray floatData;
	   /// The container for string returns
	   std::string stringData;
	};

	/// The returned data collection
	std::vector<PyReturnValue> dataReturn;

   /// Fill in Input parameter list
   Integer FillInputList();
   /// Fill in Output parameter list
   Integer FillOutputList();
   /// Create C++ variable type declared in GMAT script
   void SendInParam(std::vector<void *> &argIn, std::vector<Gmat::ParameterType> &pType);
   /// Create C++ variable type declared in GMAT script
   void GetOutParams();
   bool BuildReturnFromPyObject(PyObject* member);
//   void GetOutParams(const std::vector<void *> &argOut);

   /// Published parameters for Python functions
   enum
   {
      MODULENAME = CallFunctionParamCount,
      FUNCTIONNAME,
      PythonFunctionParamCount
   };

   /// Python function parameter labels
   static const std::string
      PARAMETER_TEXT[PythonFunctionParamCount - CallFunctionParamCount];
   /// Python function parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[PythonFunctionParamCount - CallFunctionParamCount];
};




#endif
