//$Id: ErrorModel.cpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 ErrorModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/01/07
//
/**
 * Implementation for the ErrorModel class
 */
//------------------------------------------------------------------------------


#include "ErrorModel.hpp"
#include "StringUtil.hpp"
#include "GmatBase.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string ErrorModel::PARAMETER_TEXT[] =
{
   "Type",
//   "Trip",
//   "Strand",
   "NoiseSigma",
//   "NoiseModel",
   "Bias",
   "SolveFors",
};

const Gmat::ParameterType ErrorModel::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			// TYPE                  // Its value will be "Range_KM", "Range_RU", "Doppler_RangeRate", "Doppler_HZ", TDRSDoppler_HZ"
//   Gmat::INTEGER_TYPE,			// TRIP                  // Its value is 1 for one-way, 2 for two-way, 3 for three-way, and so on
//   Gmat::STRINGARRAY_TYPE,    // STRAND                // containing a name list of participants along signal path
   Gmat::REAL_TYPE,			   // NOISE_SIGMA           // Measurement noise sigma value
//   Gmat::STRING_TYPE,         // NOISE_MODEL           // Specify model of error. It is "RandomConstant" for Gausian distribution 
   Gmat::REAL_TYPE,			   // BIAS                  // Measurement bias
   Gmat::STRINGARRAY_TYPE,    // SOLVEFORS             // Contains a list of all solve-for parameters in ErrorModel 
};



//------------------------------------------------------------------------------
// ErrorModel(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for ErrorModel objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const std::string name) :
   GmatBase          (Gmat::ERROR_MODEL, "ErrorModel", name),
   measurementType   ("Range_KM"),
//   measurementTrip   (2),
//   strand            (""),
   noiseSigma        (0.01),                   // 0.01 Km
//   noiseModel        ("NoiseConstant"), 
   bias              (0.0)                    // 0.0 Km
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::ERROR_MODEL);
   objectTypeNames.push_back("ErrorModel");

   parameterCount = ErrorModelParamCount;
}


//------------------------------------------------------------------------------
// ~ErrorModel()
//------------------------------------------------------------------------------
/**
 * ErrorModel destructor
 */
//------------------------------------------------------------------------------
ErrorModel::~ErrorModel()
{
}


//------------------------------------------------------------------------------
// ErrorModel(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a ErrorModel
 *
 * @param em        The ErrorModel object that provides data for the new one
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const ErrorModel& em) :
   GmatBase              (em),
   measurementType       (em.measurementType),
//   measurementTrip       (em.measurementTrip),
//   participantNameList   (em.participantNameList),
   noiseSigma            (em.noiseSigma),
//   noiseModel            (em.noiseModel),
   bias                  (em.bias),
   solveforNames         (em.solveforNames)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel copy constructor from <%s,%p>  to  <%s,%p>\n", em.GetName().c_str(), &em, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// ErrorModel& operator=(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel assignment operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return This object, configured to match em
 */
//------------------------------------------------------------------------------
ErrorModel& ErrorModel::operator=(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      GmatBase::operator=(em);

      measurementType     = em.measurementType;
//      measurementTrip     = em.measurementTrip;
//      participantNameList = em.participantNameList;
      noiseSigma          = em.noiseSigma;
//      noiseModel          = em.noiseModel;
      bias                = em.bias;
      solveforNames       = em.solveforNames;
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool operator==(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel equal comparing operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return      true if both error model objects having the same measurement 
 *              type, trip, and strand; false otherwise
 */
//------------------------------------------------------------------------------
bool ErrorModel::operator==(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator == <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      //retVal = GmatBase::operator==(em);

      if ((GetName() != em.GetName())||
          (measurementType != em.measurementType)   //||
//          (measurementTrip != em.measurementTrip)||
//          (participantNameList != em.participantNameList)
         )
         return false;
   }
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for ErrorModel
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* ErrorModel::Clone() const
{
   return new ErrorModel(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Code fired in the Sandbox when the Sandbox initializes objects prior to a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ErrorModel::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif
   if (isInitialized)
      return true;

   bool retval = true;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif

   isInitialized = retval;
   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Code that executes after a run completes
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ErrorModel::Finalize()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a ErrorModel property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The text string specifying the property's units
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetParameterUnit(const Integer id) const
{
   // @Todo: It needs to add code to specify unit used for each parameter
   return GmatBase::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a scripted property string
 *
 * @param str The scripted string used for the property
 *
 * @return The associated ID
 */
//------------------------------------------------------------------------------
Integer ErrorModel::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < ErrorModelParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ErrorModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a property
 *
 * @param id The ID of the property
 *
 * @return The text description of the property type
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetStringParameter(const Integer id) const
{
   if (id == TYPE)
      return measurementType;

   //if (id == NOISE_MODEL)
   //   return noiseModel;

   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param id The ID of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVEFORS)
   {
      std::string value1 = GmatStringUtil::Trim(GmatStringUtil::RemoveOuterString(value, "{", "}"));
      if (value1 == "")
         return true;

      std::string::size_type pos = value1.find_first_of(',');
      Integer i = 0;
      while (pos != value.npos)
      {
         std::string value2 = value1.substr(0, pos);
         value1 = value1.substr(pos+1);
         SetStringParameter(id, value2, i);
         ++i;
         pos = value1.find_first_of(',');
      }
      SetStringParameter(id, value1, i);
   }

   if (id == TYPE)
   {
      // Get a list of all available types
      StringArray typesList = GetAllAvailableTypes();
      bool found = false;
      for (Integer i = 0; i < typesList.size(); ++i)
      {
         if (typesList[i] == value)
         {
            found = true;
            break;
         }
      }
      if (!found)
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".Type parameter is an invalid measurement type.\n");

      measurementType = value;
      return true;
   }

   //if (id == NOISE_MODEL)
   //{
   //   if (value != "RandomConstant")
   //   {
   //      throw GmatBaseException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
   //      return false;
   //   }
   //   else
   //   {
   //      noiseModel = value;    
	  //    return true;
   //   }
   //}

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a ErrorModel
 *
 * @param label The text description of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param label The text description of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ErrorModel::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetStringParameter(const Integer id, const Integer index) const
{
   //if (id == STRAND)
   //{
   //   if ((index < 0)||(index >= (Integer)participantNameList.size()))
   //   {
   //      std::stringstream ss;
   //      ss << "Error: participant index (" << index << ") is out of bound.\n";
   //      throw GmatBaseException(ss.str());
   //   }

   //   return participantNameList[index];
   //}

   if (id == SOLVEFORS)                                                             // made changes by TUAN NGUYEN
   {                                                                                // made changes by TUAN NGUYEN
      if ((index < 0)||(index >= (Integer)solveforNames.size()))                    // made changes by TUAN NGUYEN
      {                                                                             // made changes by TUAN NGUYEN
         std::stringstream ss;                                                      // made changes by TUAN NGUYEN
         ss << "Error: solve-for index (" << index << ") is out of bound.\n";       // made changes by TUAN NGUYEN
         throw GmatBaseException(ss.str());                                         // made changes by TUAN NGUYEN
      }                                                                             // made changes by TUAN NGUYEN

      return solveforNames[index];                                                  // made changes by TUAN NGUYEN
   }                                                                                // made changes by TUAN NGUYEN

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
   //if (id == STRAND)
   //{
   //   if ((0 <= index)&&(index < (Integer)participantNameList.size()))
   //   {
   //      participantNameList[index] = value;
   //      return true;
   //   }
   //   else
   //   {
   //      if (index == -1)
   //      {
   //         participantNameList.clear();
   //         return true;
   //      }
   //      else if (index == participantNameList.size())
   //      {
   //         participantNameList.push_back(value);
   //         return true;
   //      }
   //      else
   //      {
   //         std::stringstream ss;
   //         ss << "Error: participant name's index (" << index << ") is out of bound.\n"; 
   //         throw GmatBaseException(ss.str());
   //      }
   //   }
   //}

   if (id == SOLVEFORS)                                                             // made changes by TUAN NGUYEN
   {                                                                                // made changes by TUAN NGUYEN
      if (!GmatStringUtil::IsValidIdentity(value))                                  // made changes by TUAN NGUYEN
         throw MeasurementException("Error: '" + value + "' is an invalid value. "  // made changes by TUAN NGUYEN
         + GetName() + ".SolveFors parameter only accepts Bias as a solve-for.\n"); // made changes by TUAN NGUYEN

      if ((0 <= index)&&(index < (Integer)solveforNames.size()))                    // made changes by TUAN NGUYEN
      {                                                                             // made changes by TUAN NGUYEN
         solveforNames[index] = value;                                              // made changes by TUAN NGUYEN
         return true;                                                               // made changes by TUAN NGUYEN
      }                                                                             // made changes by TUAN NGUYEN
      else                                                                          // made changes by TUAN NGUYEN
      {                                                                             // made changes by TUAN NGUYEN
         if (index == -1)
         {
            solveforNames.clear();
            return true;
         }
         else if (index == solveforNames.size())                                         // made changes by TUAN NGUYEN
         {                                                                          // made changes by TUAN NGUYEN
            solveforNames.push_back(value);                                         // made changes by TUAN NGUYEN
            return true;                                                            // made changes by TUAN NGUYEN
         }                                                                          // made changes by TUAN NGUYEN
         else                                                                       // made changes by TUAN NGUYEN
         {                                                                          // made changes by TUAN NGUYEN
            std::stringstream ss;                                                   // made changes by TUAN NGUYEN
            ss << "Error: solve-for's index (" << index << ") is out of bound.\n";  // made changes by TUAN NGUYEN
            throw GmatBaseException(ss.str());                                      // made changes by TUAN NGUYEN
         }                                                                          // made changes by TUAN NGUYEN
      }                                                                             // made changes by TUAN NGUYEN
   }                                                                                // made changes by TUAN NGUYEN

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param labe    The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param label   The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool ErrorModel::SetStringParameter(const std::string &label, const std::string &value,
                                           const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& ErrorModel::GetStringArrayParameter(const Integer id) const
{
   //if (id == STRAND)
   //   return participantNameList;

   if (id == SOLVEFORS)                           // made changes by TUAN NGUYEN
      return solveforNames;                       // made changes by TUAN NGUYEN

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param label   The name of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& ErrorModel::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}



Real ErrorModel::GetRealParameter(const Integer id) const
{
   if (id == NOISE_SIGMA)
      return noiseSigma;

   if (id == BIAS)
      return bias;

   return GmatBase::GetRealParameter(id);
}


Real ErrorModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == NOISE_SIGMA)
   {
      if (value <= 0.0)
         throw GmatBaseException("Error: value of "+ GetName() +".NoiseSigma has to be a positive number.\n");

	  noiseSigma = value;
	  return noiseSigma;
   }

   if (id == BIAS)
   {
	  bias = value;
	  return bias;
   }
   return GmatBase::SetRealParameter(id, value);
}


Real ErrorModel::GetRealParameter(const std::string& label) const
{
	return GetRealParameter(GetParameterID(label));
}


Real ErrorModel::SetRealParameter(const std::string& label, const Real value)
{
	return SetRealParameter(GetParameterID(label), value);
}



Integer ErrorModel::GetIntegerParameter(const Integer id) const
{
   //if (id == TRIP)
   //   return measurementTrip;

   return GmatBase::GetIntegerParameter(id);
}


Integer ErrorModel::SetIntegerParameter(const Integer id, const Integer value)
{
   //if (id == TRIP)
   //{
   //   if (value < 0)
   //      throw GmatBaseException("Error: value of "+ GetName() +".Trip has to be a non-positive integer.\n");

	  // measurementTrip = value;
	  // return measurementTrip;
   //}

   return GmatBase::SetIntegerParameter(id, value);
}


Integer ErrorModel::GetIntegerParameter(const std::string& label) const
{
	return GetIntegerParameter(GetParameterID(label));
}


Integer ErrorModel::SetIntegerParameter(const std::string& label, const Integer value)
{
	return SetIntegerParameter(GetParameterID(label), value);
}



//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
* This function is used to verify an estimation paramter is either valid or not
*
* @param item      Estimation parameter ID (Note that: it is defferent from object ParameterID)
*
* return           true if it is valid, false otherwise 
*/
//------------------------------------------------------------------------------
bool ErrorModel::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID

   switch (id)
   {
      case BIAS:
         retval = true;
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::IsEstimationParameterValid(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer ErrorModel::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;


   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("ErrorModel::GetEstimationParameterSize(%d)"
            " called; parameter ID is %d\n", item, id);
   #endif


   switch (id)
   {
      case BIAS:
         retval = 1;
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::GetEstimationParameterSize(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real* GetEstimationParameterValue(const Integer item)
//------------------------------------------------------------------------------
Real* ErrorModel::GetEstimationParameterValue(const Integer item)
{
   Real* retval = NULL;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   switch (id)
   {
      case BIAS:
         retval = &bias;
         break;

      // All other values call up the class heirarchy
      default:
         retval = GmatBase::GetEstimationParameterValue(item);
   }

   return retval;
}


StringArray ErrorModel::GetAllAvailableTypes()
{
   StringArray sa;

   sa.push_back("Range_KM");
   sa.push_back("Range_RU");
   sa.push_back("Doppler_HZ");
   sa.push_back("Doppler_RangeRate");

   return sa;
}

