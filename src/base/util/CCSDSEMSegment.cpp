//------------------------------------------------------------------------------
//                                  CCSDSEMSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2013.12.02
//
/**
 * Stores, validates, and manages meta data and real data read from, or
 * to be written to, a CCSDS Ephemeris Message file.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle attitude segments).
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <iomanip>
#include "CCSDSEMSegment.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "Rvector3.hpp"
#include "UTCDate.hpp"
#include "A1Mjd.hpp"

//#define DEBUG_EM_SET_META
//#define DEBUG_PARSE_EPOCH
//#define DEBUG_EM_COVERS_EPOCH
//#define DEBUG_SLERP
//#define DEBUG_EM_FIND_EXACT_MATCH
//#define DEBUG_USABLE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Real        CCSDSEMSegment::EPOCH_MATCH_TOLERANCE = 1e-3 / 86400;  // right?
const std::string CCSDSEMSegment::UNUSED_STRING  = "UNUSED";
const std::string CCSDSEMSegment::UNSET_STRING   = "UNSET";
const Real        CCSDSEMSegment::UNSET_REAL     = -999.999;
const Integer     CCSDSEMSegment::UNSET_INTEGER  = -999;

//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Parse a time string read from the EM file and convert it to
// a Real (A1Mjd) epoch
//------------------------------------------------------------------------------
Real CCSDSEMSegment::ParseEpoch(const std::string &epochString)
{
   // The epochs can be in either of two formats:
   // YYYY-MM-DDThh:mm:ss.mmm
   // YYYY-DOYThh:mm:ss
   std::string yyyymmdd, hhmmss, YYstr, MMstr, DDstr, hhstr, mmstr, ssmmmstr;
   Integer year, month = 0, day, hour, minute;
   Real    seconds;
   bool    doyUsed = false;

   std::size_t tPos = epochString.find_first_of("Tt");
   if (tPos == std::string::npos)
   {
      std::string errmsg = "Error reading ephemeris message file segment.  ";
      errmsg += "Missing or incorrectly formatted data Epoch.\n";
      throw UtilityException(errmsg);
   }
   yyyymmdd   = epochString.substr(0,tPos);
   hhmmss     = epochString.substr(tPos+1);
   if ((hhmmss.length() < 8) ||
       (GmatStringUtil::NumberOfOccurrences(hhmmss, ':') != 2))
   {
      std::string errmsg = "Error reading ephemeris message file segment.  ";
      errmsg += "hhmmss.sss part of Time \"" + epochString;
      errmsg += "\" is not formatted correctly.\n";
      throw UtilityException(errmsg);
   }

   hhstr     = hhmmss.substr(0,2);
   mmstr     = hhmmss.substr(3,2);
   ssmmmstr  = hhmmss.substr(6);
   if ((!GmatStringUtil::ToInteger(hhstr, hour))     ||
       (!GmatStringUtil::ToInteger(mmstr, minute))   ||
       (!GmatStringUtil::ToReal(ssmmmstr, seconds)))
   {
      std::string errmsg = "Error reading ephemeris message file segment.  ";
      errmsg += "hhmmss.sss part of Time \"" + epochString;
      errmsg += "\" is not formatted correctly.\n";
      throw UtilityException(errmsg);
   }

   Integer numDashes     = GmatStringUtil::NumberOfOccurrences(yyyymmdd, '-');
   if (numDashes == 1)  // YYYY-DOYThh:mm:ss
   {
      doyUsed   = true;
      YYstr     = yyyymmdd.substr(0,4);
      DDstr     = yyyymmdd.substr(5,3);
      if ((!GmatStringUtil::ToInteger(YYstr, year))     ||
          (!GmatStringUtil::ToInteger(DDstr, day)))
      {
         std::string errmsg = "Error reading ephemeris message file segment.  ";
         errmsg += "YYYY-DOY part of Time \"" + epochString;
         errmsg += "\" is not formatted correctly.\n";
         throw UtilityException(errmsg);
      }
   }
   else if (numDashes == 2)  // YYYY-MM-DDThh:mm:ss.mmm
   {
      doyUsed   = false;
      YYstr     = yyyymmdd.substr(0,4);
      MMstr     = yyyymmdd.substr(5,2);
      DDstr     = yyyymmdd.substr(8,2);
      if ((!GmatStringUtil::ToInteger(YYstr, year))     ||
          (!GmatStringUtil::ToInteger(MMstr, month))    ||
          (!GmatStringUtil::ToInteger(DDstr, day)))
      {
         std::string errmsg = "Error reading ephemeris message file segment.  ";
         errmsg += "YYYY-MM-DD part of Time \"" + epochString;
         errmsg += "\" is not formatted correctly.\n";
         throw UtilityException(errmsg);
      }
   }
   else
   {
      std::string errmsg = "Error reading ephemeris message file segment.  ";
      errmsg += "Time \"" + epochString;
      errmsg += "\" is not formatted correctly.\n";
      throw UtilityException(errmsg);
   }

   #ifdef DEBUG_PARSE_EPOCH
      MessageInterface::ShowMessage("epochString = %s, yyyymmdd = %s, hhmmss = %s\n",
            epochString.c_str(), yyyymmdd.c_str(), hhmmss.c_str());
      MessageInterface::ShowMessage("year = %d, month = %d, day = %d\n",
            year, month, day);
      MessageInterface::ShowMessage("hour = %d, minute = %d, seconds = %lf\n",
            hour, minute, seconds);
   #endif

   if (doyUsed)
   {
      UtcDate utc(year, day, hour, minute, seconds);
      return utc.ToA1Mjd();
   }
   else
   {
      UtcDate utc(year, month, day, hour, minute, seconds);
      return utc.ToA1Mjd();
   }

}

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor (default)
//------------------------------------------------------------------------------
CCSDSEMSegment::CCSDSEMSegment(Integer segNum) :
   segmentNumber       (segNum),
   dataSize            (0),
   dataType            (UNSET_STRING),
   timeSystem          (UNSET_STRING),
   startTime           (UNSET_REAL),
   stopTime            (UNSET_REAL),
   usableStartTime     (UNSET_REAL),
   usableStopTime      (UNSET_REAL),
   interpolationMethod (UNSET_STRING),
   interpolationDegree (UNSET_INTEGER),
   objectName          (UNSET_STRING),
   objectID            (UNSET_STRING),
   centerName          (UNSET_STRING),
   usesUsableTimes     (false),
   checkLagrangeOrder  (false)
{
   dataStore.clear();
   dataComments.clear();
   metaComments.clear();

   std::ostringstream errmsg;
   errmsg << "Error reading ephemeris message file segment ";
   errmsg << segmentNumber << ".  ";
   segError = errmsg.str();
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CCSDSEMSegment::CCSDSEMSegment(const CCSDSEMSegment &copy) :
   segmentNumber       (copy.segmentNumber),
   dataSize            (copy.dataSize),
   dataType            (copy.dataType),
   timeSystem          (copy.timeSystem),
   startTime           (copy.startTime),
   stopTime            (copy.stopTime),
   usableStartTime     (copy.usableStartTime),
   usableStopTime      (copy.usableStopTime),
   interpolationMethod (copy.interpolationMethod),
   interpolationDegree (copy.interpolationDegree),
   objectName          (copy.objectName),
   objectID            (copy.objectID),
   centerName          (copy.centerName),
   usesUsableTimes     (copy.usesUsableTimes),
   checkLagrangeOrder  (copy.checkLagrangeOrder)
{
   EpochAndData *ed;
   for (Integer jj = 0; jj < (Integer) copy.dataStore.size(); jj++)
   {
      ed = new EpochAndData();
      ed->epoch = (copy.dataStore.at(jj))->epoch;
      Rvector itsData = (copy.dataStore.at(jj))->data;
      ed->data  = Rvector(itsData);
      dataStore.push_back(ed);
   }

   dataComments = copy.dataComments;
   metaComments = copy.metaComments;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CCSDSEMSegment& CCSDSEMSegment::operator=(const CCSDSEMSegment &copy)
{
   if (&copy == this)
      return *this;

   segmentNumber       = copy.segmentNumber;
   dataSize            = copy.dataSize;
   dataType            = copy.dataType;
   timeSystem          = copy.timeSystem;
   startTime           = copy.startTime;
   stopTime            = copy.stopTime;
   usableStartTime     = copy.usableStartTime;
   usableStopTime      = copy.usableStopTime;
   interpolationMethod = copy.interpolationMethod;
   interpolationDegree = copy.interpolationDegree;
   objectName          = copy.objectName;
   objectID            = copy.objectID;
   centerName          = copy.centerName;
   usesUsableTimes     = copy.usesUsableTimes;
   checkLagrangeOrder  = copy.checkLagrangeOrder;

   for (Integer ii = 0; ii < (Integer) dataStore.size(); ii++)
   {
      delete dataStore.at(ii);
   }
   dataStore.clear();
   dataComments.clear();
   metaComments.clear();

   EpochAndData *ed;
   for (Integer jj = 0; jj < (Integer) copy.dataStore.size(); jj++)
   {
      ed = new EpochAndData();
      ed->epoch = (copy.dataStore.at(jj))->epoch;
      ed->data  = Rvector((copy.dataStore.at(jj))->data);
      dataStore.push_back(ed);
   }

   dataComments = copy.dataComments;
   metaComments = copy.metaComments;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CCSDSEMSegment::~CCSDSEMSegment()
{
   for (Integer ii = 0; ii < (Integer) dataStore.size(); ii++)
   {
      delete dataStore.at(ii);
   }
   dataStore.clear();
   dataComments.clear();
   metaComments.clear();
}

//------------------------------------------------------------------------------
// Validates the contents of the handled common meta data elements and checks
// for the data.
//------------------------------------------------------------------------------
bool CCSDSEMSegment::Validate(bool checkData)
{
   // Time system
   if (timeSystem == UNSET_STRING)
   {
      std::string errmsg = segError;
      errmsg += "Required field TIME_SYSTEM is missing.\n";
      throw UtilityException(errmsg);
   }
   if (timeSystem != "UTC")
   {
      std::string errmsg = segError;
      errmsg += "Specified TIME_SYSTEM \"" + timeSystem;
      errmsg += "\" is not supported at this time.\n";
      throw UtilityException(errmsg);
   }

   // Start and stop times
   if ((startTime == UNSET_REAL) || (stopTime == UNSET_REAL))
   {
      std::string errmsg = segError;
      errmsg += "Required field START_TIME or STOP_TIME is missing.\n";
      throw UtilityException(errmsg);
   }
   if (startTime > stopTime)
   {
      std::string errmsg = segError;
      errmsg += "START_TIME must be less than STOP_TIME.\n";
      throw UtilityException(errmsg);
   }

   // Usable start and stop times
   if (((usableStartTime == UNSET_REAL) && (usableStopTime != UNSET_REAL)) ||
       ((usableStartTime != UNSET_REAL) && (usableStopTime == UNSET_REAL)))
   {
      std::string errmsg = segError;
      errmsg += "If usable start and stop times are to be used, both ";
      errmsg += "USEABLE_START_TIME and USEABLE_STOP_TIME must be present ";
      errmsg += "in the segment.\n";
      throw UtilityException(errmsg);
   }

   if ((usableStartTime != UNSET_REAL) &&
       (usableStopTime  != UNSET_REAL))
   {
      if ((usableStartTime < startTime) || (usableStartTime > stopTime))
      {
         std::string errmsg = segError;
         errmsg += "USEABLE_START_TIME must be ";
         errmsg += "between START_TIME and STOP_TIME.\n";
         throw UtilityException(errmsg);
      }
      if ((usableStopTime < startTime) || (usableStopTime > stopTime))
      {
         std::string errmsg = segError;
         errmsg += "USEABLE_STOP_TIME must be ";
         errmsg += "between START_TIME and STOP_TIME.\n";
         throw UtilityException(errmsg);
      }
      if (usableStartTime > usableStopTime)
      {
         std::string errmsg = segError;
         errmsg += "USEABLE_START_TIME must be less than USEABLE_STOP_TIME.\n";
         throw UtilityException(errmsg);
      }
      usesUsableTimes = true;
   }
   else
   {
      usesUsableTimes = false;
   }

   // Interpolation
   if ((interpolationMethod == "LAGRANGE") || checkLagrangeOrder)
   {
      if ((interpolationDegree < 0) || (interpolationDegree > 9))
      {
         std::string errmsg = segError;
         errmsg += "Field INTERPOLATION_DEGREE is out-of-range.  For Lagrange ";
         errmsg += "interpolation, degree must be between 0 and 9.\n";
         throw UtilityException(errmsg);
      }
   }

   // Is there data?
   if (checkData && dataStore.empty())
   {
      std::string errmsg = segError;
      errmsg += "File does not contain data for segment of ";
      errmsg += "data type " + dataType + "\n";
      throw UtilityException(errmsg);
   }

   return true;
}

//------------------------------------------------------------------------------
// Sets the corresponding meta data for the input field name.
//------------------------------------------------------------------------------
bool CCSDSEMSegment::SetMetaData(const std::string &fieldName,
                                 const std::string &value)
{
   #ifdef DEBUG_EM_SET_META
      MessageInterface::ShowMessage(
            "Entering EMSegment::SetMetaData with field %s and value %s\n",
            fieldName.c_str(), value.c_str());
   #endif
   Real theTime;
   if (fieldName == "COMMENT")
   {
      metaComments.push_back(value);
      return true;
   }
   else if (fieldName == "OBJECT_NAME")
   {
      objectName = value;
      return true;
   }
   else if (fieldName == "OBJECT_ID")
   {
      objectID = value;
      return true;
   }
   else if (fieldName == "CENTER_NAME")
   {
      centerName = value;
      return true;
   }
   // @todo - check for this:
   // "The TIME_SYSTEM value must remain fixed within an AEM."
   else if (fieldName == "TIME_SYSTEM")
   {
      timeSystem = GmatStringUtil::ToUpper(value);
      return true;
   }
   else if (fieldName == "START_TIME")
   {
      theTime   = ParseEpoch(value);
      startTime = theTime;
      return true;
   }
   else if (fieldName == "USEABLE_START_TIME")
   {
      theTime         = ParseEpoch(value);
      usableStartTime = theTime;
      return true;
   }
   else if (fieldName == "USEABLE_STOP_TIME")
   {
      theTime        = ParseEpoch(value);
      usableStopTime = theTime;
      return true;
   }
   else if (fieldName == "STOP_TIME")
   {
      theTime  = ParseEpoch(value);
      stopTime = theTime;
      return true;
   }
   else if (fieldName == "INTERPOLATION_DEGREE")
   {
      Integer theDegree;
      std::string trimmedValue = GmatStringUtil::Trim(value, GmatStringUtil::BOTH, true, true);
      bool isInt = GmatStringUtil::ToInteger(trimmedValue, theDegree);
      if (!isInt)
      {
         std::string errmsg = segError;
         errmsg += "Value for INTERPOLATION_DEGREE is not ";
         errmsg += "a valid integer\n";
         throw UtilityException(errmsg);
      }
      interpolationDegree = theDegree;
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// Adds an epoch/data pair to the dataStore.
// At this level, it just checks for errors in data size or epoch order.
//------------------------------------------------------------------------------
bool CCSDSEMSegment::AddData(Real epoch, Rvector data)
{
   if (data.GetSize() != dataSize)
   {
      std::ostringstream errmsg("");
      errmsg << segError;
      errmsg << "Data of type " << dataType;
      errmsg << " must contain " << dataSize;
      errmsg << " elements.\n";
      throw UtilityException(errmsg.str());
   }
   if ((epoch < startTime) || (epoch > stopTime))
   {
      std::ostringstream errmsg("");
      errmsg.precision(16);
      errmsg << segError;
      errmsg << "Data epoch " << epoch << " must be between the start time, ";
      errmsg << startTime << ", and the stop time, " << stopTime;
      errmsg << ", for the segment.\n";
      throw UtilityException(errmsg.str());
   }
   if (!dataStore.empty())
   {
      Real lastEpoch = dataStore.at(dataStore.size()-1)->epoch;
      if (epoch <= lastEpoch)
      {
         std::string errmsg = segError;
         errmsg += "Epochs within DATA segment must be increasing.\n";
         throw UtilityException(errmsg);
      }
   }
   return true;
}

//------------------------------------------------------------------------------
// Adds a data comment.
//------------------------------------------------------------------------------
bool CCSDSEMSegment::AddDataComment(const std::string& comment)
{
   dataComments.push_back(comment);
   return true;
}

//------------------------------------------------------------------------------
// Returns the required size of the data vector.
//------------------------------------------------------------------------------
Integer CCSDSEMSegment::GetDataSize() const
{
   return dataSize;
}

//------------------------------------------------------------------------------
// Determines whether or not the input epoch is covered by the data of this
// segment.
//------------------------------------------------------------------------------
bool CCSDSEMSegment::CoversEpoch(Real theEpoch)
{
   #ifdef DEBUG_EM_COVERS_EPOCH
      MessageInterface::ShowMessage("In CoversEpoch (segmentNumber = %d), theEpoch = %12.10f, usesUsableTimes = %s\n",
            segmentNumber, theEpoch, (usesUsableTimes? "true" : "false"));
      if (usesUsableTimes)
         MessageInterface::ShowMessage("     usableStart = %12.10f,     usableStop = %12.10f\n",
               usableStartTime, usableStopTime);
      else
         MessageInterface::ShowMessage("     start = %12.10f,     stop = %12.10f\n",
               startTime, stopTime);
   #endif
   if (usesUsableTimes)
   {
      if ((theEpoch >= usableStartTime - EPOCH_MATCH_TOLERANCE) &&
          (theEpoch <= usableStopTime  + EPOCH_MATCH_TOLERANCE))
         return true;
   }
   else
   {
      if ((theEpoch >= startTime - EPOCH_MATCH_TOLERANCE) &&
          (theEpoch <= stopTime  + EPOCH_MATCH_TOLERANCE))
         return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// Returns the start time of the data.
//------------------------------------------------------------------------------
Real CCSDSEMSegment::GetStartTime() const
{
//   if (usesUsableTimes) return usableStartTime;
   return startTime;
}

//------------------------------------------------------------------------------
// Returns the stop time of the data.
//------------------------------------------------------------------------------
Real CCSDSEMSegment::GetStopTime() const
{
//   if (usesUsableTimes) return usableStopTime;
   return stopTime;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Determines whether or not the input time exactly matches an epoch from the
// dataStore.  If so, it returns the state at that epoch.  If not, and the
// interpolation degree = 0, it returns the last data before the input epoch.
// Otherwise, it calls the interpolation method to get the state.
//------------------------------------------------------------------------------
Rvector CCSDSEMSegment::DetermineState(Real atEpoch)
{
   // Make sure that if we are using usable times, we only check times
   // between usableStartTime and usableStopTime
   if (usesUsableTimes &&
      ((atEpoch < (usableStartTime - EPOCH_MATCH_TOLERANCE)) ||
       (atEpoch > (usableStopTime  + EPOCH_MATCH_TOLERANCE))))
   {
      std::ostringstream errmsg;
      errmsg.precision(16);
      errmsg << "Specified epoch ";
      errmsg << atEpoch << " in segment ";
      errmsg << segmentNumber << " must be within usable time range.\n";
      throw UtilityException(errmsg.str());
   }
   bool      exactMatchFound = false;
   Integer   matchPos        = -1;
   for (unsigned int ii= 0; ii < dataStore.size(); ii++)
   {
      Real theTime = (dataStore.at(ii))->epoch;

      #ifdef DEBUG_EM_FIND_EXACT_MATCH
         MessageInterface::ShowMessage("FindExactEpochMatch data entry %d "
               "epoch = %12.10f\n",
               ii, theTime);
      #endif
      if (GmatMathUtil::IsEqual(theTime, atEpoch, EPOCH_MATCH_TOLERANCE))
      {
         exactMatchFound = true;
         matchPos        = ii;
         #ifdef DEBUG_EM_FIND_EXACT_MATCH
            MessageInterface::ShowMessage("In FindExactEpochMatch exact match to epoch = %12.10f\n",
                  (dataStore.at(ii))->epoch);
          #endif
         break;
      }
      if (theTime < atEpoch)  matchPos = ii;
      // if the time is bigger, we've gone past and won't find an exact match
      else if (theTime > atEpoch) break;
   }
   // if we didn't find an exact match OR an epoch less than the input
   // epoch, that is an error
   if (matchPos < 0)
   {
      std::ostringstream errmsg;
      errmsg.precision(16);
      errmsg << "Error searching for epoch ";
      errmsg << atEpoch << " in segment ";
      errmsg << segmentNumber << ", within usable time range.\n";
      throw UtilityException(errmsg.str());
   }
   if (exactMatchFound || (interpolationDegree == 0))
      return (dataStore.at(matchPos))->data;
   else
      return Interpolate(atEpoch);
}

//------------------------------------------------------------------------------
// Determines the indices of the first and last usable line of data, based
// on the usableStartTime and usableStopTime
//------------------------------------------------------------------------------
bool CCSDSEMSegment::GetUsableIndexRange(Integer &first, Integer &last)
{
   #ifdef DEBUG_USABLE
      MessageInterface::ShowMessage("In GetUsableIndexRange, usableStart = %12.10f, usableStop = %12.10f\n",
            usableStartTime, usableStopTime);
   #endif
   // If we are not using usableStartTime and usableStopTime, the range
   // covers the entire startTime-stopTime span
   first = 0;
   last  = (Integer) dataStore.size() - 1;

   // If we are using usableStartTime and usableStopTime, we need to figure
   // out which lines of data are included in that span
   if (usesUsableTimes)
   {
      bool firstFound = false;
      for (Integer ii = 0; ii < (Integer) dataStore.size(); ii++)
      {
         Real iiEpoch = dataStore.at(ii)->epoch;
         #ifdef DEBUG_USABLE
            MessageInterface::ShowMessage("        ii = %d, epoch = %12.10f\n",
                  ii, iiEpoch);
         #endif
         if (!firstFound && (iiEpoch > (usableStartTime - EPOCH_MATCH_TOLERANCE)))
         {
            first      = ii;
            firstFound = true;
         }
         else if (iiEpoch > (usableStopTime + EPOCH_MATCH_TOLERANCE))
         {
            last = ii - 1;
            break;
         }
      }
      #ifdef DEBUG_USABLE
         MessageInterface::ShowMessage("In GetUsableIndexRange, first = %d, last = %d\n",
               first, last);
      #endif
   }
   if (first == last)
   {
      std::string errmsg = segError;
      errmsg += "Only one data point available ";
      errmsg += "in usable epoch range.\n";
      throw UtilityException(errmsg);
   }
   return true;
}

//------------------------------------------------------------------------------
// Rvector InterpolateLagrange(Real atEpoch)
// Interpolates the segment data using Lagrange interpolation.
//------------------------------------------------------------------------------
Rvector CCSDSEMSegment::InterpolateLagrange(Real atEpoch)
{
   // Adapted from MATLAB version:
   //   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>

   // Input parsing
   Integer n = interpolationDegree;

   Integer firstUsable;
   Integer lastUsable;

   GetUsableIndexRange(firstUsable, lastUsable);

   // Sanity Checks
   Real minEpoch = dataStore.at(firstUsable)->epoch;
   Real maxEpoch = dataStore.at(lastUsable)->epoch;
   if ((atEpoch < (minEpoch - EPOCH_MATCH_TOLERANCE)) ||
       (atEpoch > (maxEpoch + EPOCH_MATCH_TOLERANCE)))
   {
      std::string errmsg = "Requested time for LAGRANGE interpolation ";
      errmsg += "is out of usable epoch range.\n";
      throw UtilityException(errmsg);
   }

//   Integer numStates = dataStore.size();
   // The number of usable states we have
   Integer numStates = lastUsable - firstUsable + 1;
   if (n >= numStates)
   {
      std::string errmsg = "Insufficient usable data for LAGRANGE interpolation.\n";
      throw UtilityException(errmsg);
   }

   // Initialization
   Rvector state(dataSize);   // Creates a vector of zeroes

   // find intended position of epoch in ephemeris data
   // find correct (first largest) epoch in ephemeris data
   Real    anEpoch  = 0.0;
   Integer epochPos = 0;
   for (Integer ii = firstUsable; ii <= lastUsable; ii++)
   {
      anEpoch = (dataStore.at(ii))->epoch;
      if ( anEpoch > atEpoch)
      {
         epochPos = ii;
         break;
      }
   }
   Integer initIndex = -1;
   // pick starting point for interpolation data
   // (region ending just before epoch's position in the ephemeris)
   if (n+1 >= (epochPos - firstUsable))  // was just epochPos
       initIndex = firstUsable + 1;      // was 1
   else
       initIndex = epochPos - (n+1);

   // slide interpolation data region forward until epoch is nearest
   // numerical center
   Integer pDiff = GmatRealConstants::INTEGER_MAX;
   Integer q     = -1;;
   Real    diff  = -999.99;
   for (Integer ii = initIndex; ii <= (numStates-n); ii++)
   {
      Real dataEpoch     = dataStore.at(ii)->epoch;
      Real dataPlusEpoch = dataStore.at(ii+n)->epoch;
      diff = GmatMathUtil::Abs((dataEpoch + dataPlusEpoch) / 2 - atEpoch);
      if (diff > pDiff) break;
      else              q = ii;
      pDiff = diff;
   }

   Rvector d1(dataSize);

   Real    t1, t2;

   for (Integer ii = q; ii <= q+n; ii++)
   {
      t1      = dataStore.at(ii)->epoch;
      d1      = dataStore.at(ii)->data;
      for (Integer jj = q; jj <= q+n; jj++)
      {
         t2  = dataStore.at(jj)->epoch;
         if (ii != jj)
            d1    = d1 * ( (atEpoch - t2) / (t1 - t2) );
      }
      state = state + d1;
   }
   return state;
}

//------------------------------------------------------------------------------
// Rvector InterpolateSLERP(Real atEpoch)
// Interpolates the segment data using SLERP interpolation.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Rvector CCSDSEMSegment::InterpolateSLERP(Real atEpoch)
{

   // Adapted from MATLAB version:
   // YRL, Interpolate Quaternion using SLERP

   #ifdef DEBUG_SLERP
      MessageInterface::ShowMessage(">>>> Entering SLERP (%d), atEpoch = %12.10f\n",
            segmentNumber, atEpoch);
   #endif
   // Sanity Checks
   Integer firstUsable, lastUsable;
   GetUsableIndexRange(firstUsable, lastUsable);

   #ifdef DEBUG_SLERP
      MessageInterface::ShowMessage(">>>> dataStore size = %d, first = %d, last = %d\n",
            (Integer) dataStore.size(), firstUsable, lastUsable);
      MessageInterface::ShowMessage(">>>> first time = %12.10f,  last time = %12.10f\n",
            dataStore.at(firstUsable)->epoch, dataStore.at(lastUsable)->epoch);
      MessageInterface::ShowMessage(">>>>       and requested epoch = %12.10f\n",
            atEpoch);
   #endif
   Real minEpoch = dataStore.at(firstUsable)->epoch;
   Real maxEpoch = dataStore.at(lastUsable)->epoch;
   if ((atEpoch < minEpoch) || (atEpoch > maxEpoch))
   {
      std::string errmsg = "Requested time for SLERP interpolation ";
      errmsg += "is out of usable epoch range.\n";
      throw UtilityException(errmsg);
   }

   // Interpolation Algorithm (SLERP)
   // find correct (first largest) epoch in ephemeris data
   Real    anEpoch  = 0.0;
   Integer epochPos = 0;
   for (Integer ii = firstUsable; ii <= lastUsable; ii++)
   {
      anEpoch = (dataStore.at(ii))->epoch;
      if ( anEpoch > atEpoch)
      {
         epochPos = ii;
         break;
      }
   }
   #ifdef DEBUG_SLERP
      MessageInterface::ShowMessage("In SLERP, minEpoch = %12.10f\n", minEpoch);
      MessageInterface::ShowMessage("In SLERP, maxEpoch = %12.10f\n", maxEpoch);
      MessageInterface::ShowMessage("In SLERP, epochPos = %d\n", epochPos);
   #endif

   // Get times and data for points before and after requested time
   Real t1    = (dataStore.at(epochPos-1))->epoch;
   Real t2    = anEpoch;
   Rvector d1 = (dataStore.at(epochPos-1))->data;
   Rvector d2 = (dataStore.at(epochPos))->data;

   #ifdef DEBUG_SLERP
      MessageInterface::ShowMessage("In SLERP, t1 = %12.10f\n", t1);
      MessageInterface::ShowMessage("In SLERP, t2 = %12.10f\n", t2);
      MessageInterface::ShowMessage("In SLERP, d1 = \n");
      for (Integer ii = 0; ii < (Integer) d1.GetSize(); ii++)
         MessageInterface::ShowMessage("    %12.10f\n", d1[ii]);
      MessageInterface::ShowMessage("In SLERP, d2 = \n");
      for (Integer ii = 0; ii < (Integer) d2.GetSize(); ii++)
         MessageInterface::ShowMessage("    %12.10f\n", d2[ii]);
   #endif

   Real cosOmega = ( d1 * d2 ) / d1.GetMagnitude() / d2.GetMagnitude() ;
   Real sinOmega = GmatMathUtil::Sqrt(1.0 - GmatMathUtil::Pow(cosOmega,2));
   Real omega    = GmatMathUtil::ASin(sinOmega) ;

   Real t        = ( atEpoch - t1 ) / ( t2 - t1 ) ;
   Rvector dSlerp(dataSize);
   #ifdef DEBUG_SLERP
      MessageInterface::ShowMessage("In SLERP, t        = %12.10f\n", t);
      MessageInterface::ShowMessage("In SLERP, cosOmega = %12.10f\n", cosOmega);
      MessageInterface::ShowMessage("In SLERP, sinOmega = %12.10f\n", sinOmega);
      MessageInterface::ShowMessage("In SLERP, omega    = %12.10f\n", omega);
   #endif

   for (Integer jj = 0; jj < dataSize; jj++)
   {
      if (sinOmega == 0)
         dSlerp[jj] = (1-t)*d1[jj] + t*d2[jj] ;
      else
         dSlerp[jj] = (GmatMathUtil::Sin((1-t)*omega) * d1[jj] + GmatMathUtil::Sin(t*omega) * d2[jj]) / sinOmega;
   }
   return dSlerp;
}
