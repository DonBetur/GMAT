//$Id$
//------------------------------------------------------------------------------
//                               EclipseEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.01.08
//
/**
 * Implementation for the EclipseEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "EclipseEvent.hpp"
#include "TimeSystemConverter.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

EclipseEvent::EclipseEvent() :
   LocatedEvent  ()
{
}

EclipseEvent::EclipseEvent(Real startEpoch, Real endEpoch,
                           const std::string itsType, const std::string theBody) :
   LocatedEvent  (startEpoch, endEpoch),
   eclipseType   (itsType),
   occultingBody (theBody)
{
}

EclipseEvent::~EclipseEvent()
{
   // nothing to do here
}

EclipseEvent::EclipseEvent(const EclipseEvent& copy) :
   LocatedEvent  (copy),
   eclipseType   (copy.eclipseType),
   occultingBody (copy.occultingBody)
{
}

EclipseEvent& EclipseEvent::operator=(const EclipseEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      eclipseType   = copy.eclipseType;
      occultingBody = copy.occultingBody;
   }

   return *this;
}

std::string EclipseEvent::GetReportString()
{
   std::stringstream eventString("");
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string startGregorian, endGregorian;
   Real        resultMjd;
   std::string blanks       = "    ";

   TimeConverterUtil::Convert("A1ModJulian", start, "",
                              outputFormat, resultMjd, startGregorian);
   TimeConverterUtil::Convert("A1ModJulian", end, "",
                              outputFormat, resultMjd, endGregorian);

   Real currentDuration = GetDuration();

   eventString << startGregorian << blanks << endGregorian         << blanks <<
                  GmatStringUtil::BuildNumber(currentDuration, false, 14)    <<
                  blanks <<
                  GmatStringUtil::PadWithBlanks(occultingBody, 12) << blanks <<
                  GmatStringUtil::PadWithBlanks(eclipseType, 8);
   return eventString.str();
}

