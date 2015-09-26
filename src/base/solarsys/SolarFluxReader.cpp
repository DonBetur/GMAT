//$Id$
//------------------------------------------------------------------------------
//                            SolarFluxReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Farideh Farahnak
// Created: 2014/11/14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
/**
 * SolarFluxReader class will be used by the AtomsphereModel class.
 */
//------------------------------------------------------------------------------

#include "SolarFluxReader.hpp"
#include "SolarSystemException.hpp"

#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include <sstream>


//#define DEBUG_FILE_INDEXING
//#define DEBUG_GETFLUXINPUTS

//#define DEBUG_FIRSTFEW_READS

#ifdef DEBUG_FIRSTFEW_READS
   Integer howMany = 5;
   Integer numberReadIndex = 0;
   Integer refcount = 0;
#endif


//------------------------------------------------------------------------------
// SolarFluxReader::FluxData struct used for holding the records
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FluxData()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData::FluxData()
{
   // Default all to -1
   epoch = -1.0;
   apAvg = -1.0;
   adjF107 = -1.0;
   adjCtrF107a = -1.0;
   obsF107 = -1.0;
   obsCtrF107a = -1.0;

   for (Integer j = 0; j < 9; j++)
      F107a[j] = -1.0;
   for (Integer j = 0; j < 3; j++)
      apSchatten[j] = -1.0;
   for (Integer j = 0; j < 8; j++)
   {
      kp[j] = -1.0;
      ap[j] = -1.0;
   }

   index = -1;
   id = -1;
   isObsData = true;
}


//------------------------------------------------------------------------------
// FluxData(const FluxData& fD)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData::FluxData(const FluxData& fD)
{
   epoch = fD.epoch;
   for (Integer j = 0; j < 8; j++)
   {
      kp[j] = fD.kp[j];
      ap[j] = fD.ap[j];
   }
   apAvg = fD.apAvg;
   adjF107 = fD.adjF107;
   adjCtrF107a = fD.adjCtrF107a;
   obsF107 = fD.obsF107;
   obsCtrF107a = fD.obsCtrF107a;
   for (Integer j = 0; j < 9; j++)
      F107a[j] = fD.F107a[j];
   for (Integer j = 0; j < 3; j++)
      apSchatten[j] = fD.apSchatten[j];
   index = fD.index;
   id = fD.id;
   isObsData = fD.isObsData;
}


//------------------------------------------------------------------------------
// SolarFluxReader::FluxData& SolarFluxReader::FluxData::operator=()
//------------------------------------------------------------------------------
/**
* Operator=() for FluxData data structure
*/
//------------------------------------------------------------------------------
SolarFluxReader::FluxData& SolarFluxReader::FluxData::operator=(const SolarFluxReader::FluxData &fD)
{
   if (this != &fD)
   {
      epoch = fD.epoch;
      for (Integer j = 0; j < 8; j++)
      {
         kp[j] = fD.kp[j];
         ap[j] = fD.ap[j];
      }
      apAvg = fD.apAvg;
      adjF107 = fD.adjF107;
      adjCtrF107a = fD.adjCtrF107a;
      obsF107 = fD.obsF107;
      obsCtrF107a = fD.obsCtrF107a;
      for (Integer j = 0; j < 9; j++)
         F107a[j] = fD.F107a[j];
      for (Integer j = 0; j < 3; j++)
         apSchatten[j] = fD.apSchatten[j];
      index = fD.index;
      id = fD.id;
      isObsData = fD.isObsData;
   }

   return *this;
}





//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader():
   obsFileName       (""),
   predictFileName   (""),
   historicStart     (-1.0),
   historicEnd       (-1.0),
   predictStart      (-1.0),
   predictEnd        (-1.0),
   schattenFluxIndex (0),
   schattenApIndex   (0),
   warnEpochBefore   (true),
   warnEpochAfter    (true),
   f107RefEpoch      (18408.0),  // 5/31/91, epoch when the station moved (Vallado)
   interpolateFlux   (false)
{
   if (!obsFluxData.empty())
      obsFluxData.clear();

   if (!predictFluxData.empty())
      predictFluxData.clear();

   beg_ObsTag = "BEGIN OBSERVED";
   end_ObsTag = "END OBSERVED";
   begObs = endObs = begData = -1;
}


//------------------------------------------------------------------------------
// ~SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::~SolarFluxReader()
{
   #ifdef DEBUG_FIRSTFEW_READS
      --refcount;
      MessageInterface::ShowMessage("Deleting Solar Flux Reader %p, leaving %d\n", this, refcount);
   #endif
}


//------------------------------------------------------------------------------
// SolarFluxReader(const SolarFluxReader &)
//------------------------------------------------------------------------------
/**
 * copy constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader(const SolarFluxReader &sfr) :
   historicStart     (-1.0),
   historicEnd       (-1.0),
   predictStart      (-1.0),
   predictEnd        (-1.0),
   schattenFluxIndex (1),
   schattenApIndex   (0),
   warnEpochBefore   (true),
   warnEpochAfter    (true),
   f107RefEpoch      (sfr.f107RefEpoch),
   interpolateFlux   (sfr.interpolateFlux)
{
   obsFileName = sfr.obsFileName;
   predictFileName = sfr.predictFileName;
   obsFluxData = sfr.obsFluxData;
   predictFluxData = sfr.predictFluxData;
   beg_ObsTag = sfr.beg_ObsTag;
   end_ObsTag = sfr.end_ObsTag;
   begObs = sfr.begObs;
   endObs = sfr.endObs;
   begData = sfr.begData;
}


//------------------------------------------------------------------------------
// SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader& sfr)
//------------------------------------------------------------------------------
/**
 * assignment operator
 */
//------------------------------------------------------------------------------
SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader &sfr)
{
   if (this == &sfr)
      return *this;

   obsFileName = sfr.obsFileName;
   predictFileName = sfr.predictFileName;
   obsFluxData = sfr.obsFluxData;
   predictFluxData = sfr.predictFluxData;
   beg_ObsTag = sfr.beg_ObsTag;
   end_ObsTag = sfr.end_ObsTag;
   begObs = sfr.begObs;
   endObs = sfr.endObs;
   begData = sfr.begData;
   line = sfr.line;
   historicStart = -1.0;
   historicEnd = -1.0;
   predictStart = -1.0;
   predictEnd = -1.0;
   schattenFluxIndex = 1;
   schattenApIndex = 0;
   
   warnEpochBefore = true;
   warnEpochAfter = true;
   f107RefEpoch = sfr.f107RefEpoch;
   interpolateFlux = sfr.interpolateFlux;

   return *this;
}


//------------------------------------------------------------------------------
// bool Open()
//------------------------------------------------------------------------------
/**
 * Opens both Observed and Schatten input file streams.
 *
 * This method will open input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Open()
{
   #ifdef DEBUG_FIRSTFEW_READS
      MessageInterface::ShowMessage("Opening flux files:\n");
      MessageInterface::ShowMessage("   H: %s\n   P: %s\n", obsFileName.c_str(), predictFileName.c_str());
   #endif

   if (!obsFileName.empty())
   {
      inObs.open(obsFileName.c_str(), std::ios_base::in);
   }
   if (!predictFileName.empty())
   {
      inPredict.open(predictFileName.c_str(), std::ios_base::in);
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadFluxData()
//------------------------------------------------------------------------------
/**
 * Loads both Observed and Schatten input file data.
 *
 * This method will load input file data.
 * 
 * @param Observed file name and Predicted(Schatten) file name.
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadFluxData(const std::string &obsFile, const std::string &predictFile)
{
   std::string theLine;

   if (!obsFile.empty())
      obsFileName = obsFile;
   if (!predictFile.empty())
      predictFileName = predictFile;

   obsFluxData.clear();
   predictFluxData.clear();

   FileManager *fm = FileManager::Instance();

   if (obsFileName != "")
   {
      std::string weatherfile = obsFileName;
      if (fm->DoesFileExist(weatherfile) == false)
         weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
      if (fm->DoesFileExist(weatherfile) == false)
         throw SolarSystemException("Cannot open the predicted space weather file " +
               obsFileName + ", nor the file at the location " + weatherfile);

      obsFileName = weatherfile;
   }

   if (predictFileName != "")
   {
      std::string weatherfile = predictFileName;
      if (fm->DoesFileExist(weatherfile) == false)
         weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
      if (fm->DoesFileExist(weatherfile) == false)
         throw SolarSystemException("Cannot open the predicted space weather file " +
               predictFileName + ", nor the file at the location " + weatherfile);

      predictFileName = weatherfile;
   }

   // Open the files to load
   Open();
   
   if (obsFileName != "")
   {
      if (inObs.is_open())
      {
         inObs.seekg(0, std::ios_base::beg);

         while (!inObs.eof())
         {
            GmatFileUtil::GetLine(&inObs, theLine);

            if (theLine.find(beg_ObsTag) != std::string::npos)
            {
               begObs = inObs.tellg();
            }
            else if (theLine.find(end_ObsTag) != std::string::npos)
            {
               endObs = inObs.tellg();
               endObs = endObs - theLine.length() - 2;
               break;
            }
         }
         LoadObsData();
      }
      else
      {
         //throw an exception
         throw SolarSystemException("SolarFluxReader: Historic/Observed File " +
               obsFileName + " could not be opened.\n");
      }
   }

   if (predictFileName != "")
   {
      if (inPredict.is_open())
      {
         inPredict.seekg(0, std::ios_base::beg);
         while (!inPredict.eof())
         {
            GmatFileUtil::GetLine(&inPredict, theLine);

            if ((theLine.find("NOMINAL TIMING") != std::string::npos) &&
                (theLine.find("EARLY TIMING") != std::string::npos))
            {
               GmatFileUtil::GetLine(&inPredict, theLine);
               GmatFileUtil::GetLine(&inPredict, theLine);
               begData = inPredict.tellg();

               break;
            }
         }
         LoadPredictData();
      }
      else
      {
         //throw an exception
         throw SolarSystemException("SolarFluxReader: The Schatten predict "
               "file " + predictFileName + " could not be opened.\n");
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Spans: [%lf %lf], [%lf %lf]\n",
            historicStart, historicEnd, predictStart, predictEnd);
   #endif

   return(Close());
}


//------------------------------------------------------------------------------
// bool Close()
//------------------------------------------------------------------------------
/**
 * Closes both Observed and Schatten input file streams.
 *
 * This method will close input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Close()
{
   bool ret = false;
   if (inObs.is_open())
   {
      inObs.close();
      ret = true;
   }
   if (inPredict.is_open())
   {
      inPredict.close();
      ret = true;
   }

   return ret;
}


//------------------------------------------------------------------------------
// bool LoadObsData()
//------------------------------------------------------------------------------
/**
 * Loads Observed input file data.
 *
 * This method will load Observed input file data.
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadObsData()
{
   Integer hour = 0, minute = 0;
   Real sec = 0.0;

   // FORMAT(I4, I3, I3, I5, I3, 8I3, I4, 8I4, I4, F4.1, I2, I4, F6.1, I2, 5F6.1)
   //-------------------------------------------------------------------------------------------Adj     Adj   Adj   Obs   Obs   Obs---
   //yy mm dd BSRN ND Kp Kp Kp Kp Kp Kp Kp Kp Sum Ap  Ap  Ap  Ap Ap  Ap  Ap  Ap  Avg Cp C9 ISN F10.7 Q Ctr81 Lst81 F10.7 Ctr81 Lst81
   //--------------------------------------------------------------------------------------------------------------------------------- 
   // Divide each line to two sections : Geomagnetics (s1) and F107 (s2) data 
   char s1[92], s2[38];
   const char * format = "%92c %38c";
   // first section
   char s11[4], s12[2], s13[2], *s6[8], *s8[8], s9[3];
   const char * format1 = "%4c %2c %2c %*7c %2c %2c %2c %2c %2c %2c %2c %2c %*3c %3c %3c %3c %3c %3c %3c %3c %3c %3c";
   for (Integer i = 0; i < 8; i++)
   {
	   s6[i] = new char[2];
	   memset(s6[i], 0, 2);
	   s8[i] = new char[3];
	   memset(s8[i], 0, 3);
   }
	  
   // second section
   char s21[5], *s22[5];
   const char * format2 = "%5c %*1c %5c %5c %5c %5c %5c";
   for (Integer i = 0; i < 5; i++)
   {
	   s22[i] = new char[5];
	   memset(s22[i], 0, 5);
   }

   inObs.seekg(begObs, std::ios_base::beg);
   std::string theLine;
   GmatFileUtil::GetLine(&inObs, theLine);

   while (!inObs.eof() && (theLine.find(end_ObsTag) == std::string::npos))
   {
      line = theLine.c_str();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%s\n", line);
      #endif

      if (strlen(line) > 8)
      {
         // Read geomagnetic and F107 data
         sscanf(line, format, s1, s2);
         // s1 corresponds to geomagnetics data with formatted string (format1) defined above.
         sscanf(s1, format1, s11, s12, s13, s6[0], s6[1], s6[2], s6[3], s6[4],
                s6[5], s6[6], s6[7], s8[0], s8[1], s8[2], s8[3], s8[4], s8[5], s8[6], s8[7], s9);
         // s2 corresponds to F107 data with formatted string (format2) defined above.
         sscanf(s2, format2, s21, s22[0], s22[1], s22[2], s22[3], s22[4]);

         FluxData fD;

         // Set ref epoch to midnight for the date on the current line
         Real mjd = ModifiedJulianDate((YearNumber)atof(s11),
               (MonthOfYear)atof(s12), (DayOfMonth)atof(s13), hour, minute, sec);
         fD.epoch = mjd;

         // The CSSI file conains Kp * 10, then rounded to an int. Undo that here.
         for (Integer l = 0; l < 8; l++)
         {
            fD.kp[l] = atof(s6[l]) / 10.0;
            memset(s6[l], 0, 2);
         }
         for (Integer l = 0; l < 8; l++)
         {
            fD.ap[l] = atof(s8[l]);
            memset(s8[l], 0, 3);
         }

         fD.apAvg = atof(s9);
         fD.adjF107 = atof(s21);
         fD.adjCtrF107a = atof(s22[0]);
         fD.obsF107 = atof(s22[2]);
         fD.obsCtrF107a = atof(s22[3]);
         memset(s22[0], 0, 5);
         memset(s22[2], 0, 5);
         memset(s22[3], 0, 5);
         memset(s9, 0, 3);
         memset(s21, 0, 5);

         fD.index = -1;
         for (Integer l = 0; l<9; l++)
            fD.F107a[l] = -1;
         for (Integer l =0; l<3; l++)
            fD.apSchatten[l] = -1;

         fD.isObsData = true;
         obsFluxData.push_back(fD);
      }
      GmatFileUtil::GetLine(&inObs, theLine);
   }

   //delete new'd array
   for (Integer i = 0; i < 8; i++)
   {
	   if (s6[i] != NULL)
	   {
		   delete[] s6[i];
		   s6[i] = NULL;
	   }
	   if (s8[i] != NULL)
	   {
		   delete[] s8[i];
		   s8[i] = NULL;
	   }
   }

   //delete new'd array
   for (Integer i = 0; i < 5; i++)
   {
	   if (s22[i] != NULL)
	   {
		   delete[] s22[i];
		   s22[i] = NULL;
	   }
   }

   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("Loaded %d flux observation data records\n",
            obsFluxData.size());
   #endif

   if (obsFluxData.size() > 0)
   {
      historicStart = obsFluxData[0].epoch;
      // Note that epoch of last record is at day start; add 1 to reach end!
      historicEnd = obsFluxData[obsFluxData.size() - 1].epoch + 1.0;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadPredictData()
//------------------------------------------------------------------------------
/**
 * Loads Predict input file data.
 *
 * This method will load Predict input file data.
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadPredictData()
{ 
   Integer hour = 0, minute = 0;
   Real sec = 0.0;
   std::string theLine;

   inPredict.seekg(begData, std::ios_base::beg);
   
   Integer lineCounter = 0;
   std::stringstream lineList;
   while (!inPredict.eof())
   {
      GmatFileUtil::GetLine(&inPredict, theLine);
      line = theLine.c_str();
      ++lineCounter;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%s\n", line);
      #endif

      // Last line in file may or may not have an "END DATA" tag,
      // If it is found, break
      if (theLine.find("END_DATA") != std::string::npos)
         break;

      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> tokens(beg, end);

      if (tokens.size() < 14)
      {
         if (lineList.str() != "")
            lineList << ", ";
         lineList << lineCounter;
      }
      else
      {
         FluxData fD;
         fD.isObsData = false;

         // Set ref epoch to midnight for the date on the current line
         Real mjd = ModifiedJulianDate(atoi(tokens[1].c_str()),
            atoi(tokens[0].c_str()), 1, hour, minute, sec);
         fD.epoch = mjd;

         // Nominal Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l] = atof(tokens[l + 2].c_str());
         fD.apSchatten[0] = atof(tokens[5].c_str());

         // Early Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l + 3] = atof(tokens[l + 6].c_str());
         fD.apSchatten[1] = atof(tokens[9].c_str());

         // Late Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l + 6] = atof(tokens[l + 10].c_str());
         fD.apSchatten[2] = atof(tokens[13].c_str());
         fD.index = -1;
         fD.id = -1;

         for (Integer l = 0; l < 8; l++)
            fD.kp[l] = -1;
         for (Integer l = 0; l < 8; l++)
            fD.ap[l] = -1;
         fD.apAvg = -1;
         fD.adjF107 = -1;
         fD.adjCtrF107a = -1;
         fD.obsF107 = -1;
         fD.obsCtrF107a = -1;

         predictFluxData.push_back(fD);
      }
   }

   if (lineList.str().length() > 0)
      MessageInterface::ShowMessage("WARNING: Line(s) [%s] of the predict data "
            "file were not read correctly\n", lineList.str().c_str());

   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("Loaded %d flux predict data records\n",
            predictFluxData.size());
   #endif

   if (predictFluxData.size() > 0)
   {
      predictStart = predictFluxData[0].epoch;
      predictEnd = predictFluxData[predictFluxData.size() - 1].epoch;

      Integer runningDays = 0;
      for (Integer i = 0; i < predictFluxData.size(); ++i)
      {
         predictFluxData[i].index = (Integer)(predictFluxData[i].epoch - predictStart);
         predictFluxData[i].id = i;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// FluxData SolarFluxReader::GetInputs(GmatEpoch epoch)
//------------------------------------------------------------------------------
/**
 * Gets raw Observed or Predicted input file data for teh input epoch.
 * 
 * HistoricAndNearTerm source is always given precedence if the requested epoch
 * is in the historic data section.  Otherwise, predicted data is used from the
 * predicted source.  If the requested data is not on a file (regardless of the
 * file type), the closest but later data trelative to the requested epoch is
 * returned,  and issue a (single) warning message is posted indicating that
 * data was not available, so boundary data is being used from a specified
 * source.
 *
 * @param epoch The epoch for the requested data
 *
 * @return A populated FluxData struct
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData SolarFluxReader::GetInputs(GmatEpoch epoch)
{
   // Local instance for the return.  A more efficient approach would be to pass
   // in the struct to be filled.
   FluxData fD;
   Integer index;


   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("GetInputs(%.12lf): index = %d, using ",
            epoch, index);
   #endif

   // if the requested epoch falls beyond the
   // last item in the obsFluxData, then search in predictFluxData
   if (epoch >= historicEnd)
   {
      #ifdef DEBUG_GETFLUXINPUTS
         MessageInterface::ShowMessage("predict data\n");
      #endif

      if ((predictFluxData.size() > 0))
      {
         // Init to the first predict record
         fD = predictFluxData[0];

         if (epoch < predictStart)
         {
            // Warn the user that the epoch is too early
            if (warnEpochBefore)
            {
               MessageInterface::ShowMessage("Warning: Requested epoch for "
                     "solar flux data is earlier than the starting epoch on "
                     "the predict flux file.  GMAT is using the first file "
                     "entry.\n");
               warnEpochBefore = false;
            }
         }
         else if (epoch > predictEnd)
         {
            // Warn the user that the epoch is too late
            if (warnEpochAfter)
            {
               MessageInterface::ShowMessage("Warning: Requested epoch for solar flux "
                  "data is later than the ending epoch on the predict flux file; "
                  "using the last file entry.\n");
               warnEpochAfter = false;
            }
            fD = predictFluxData[predictFluxData.size() - 1];
         }
         else
         {
            // Look up the data for epoch
            for (UnsignedInt i = 1; i < predictFluxData.size(); ++i)
            {
               if (predictFluxData[i].epoch > epoch)
               {
                  fD = predictFluxData[i-1];
                  fD.index = -1;
                  break;
               }
            }
         }
      }
      else // Off the CSSI file and there is no predict data read
      {
         // Warn the user that the epoch is too late
         if (warnEpochAfter)
         {
            MessageInterface::ShowMessage("Warning: Requested epoch for solar flux "
               "data is later than the ending epoch on the flux file.  GMAT "
               "is using the last file entry.\n");
            warnEpochAfter = false;
            index = obsFluxData.size() - 1;
            fD = obsFluxData[index];
            fD.index = index;
         }
      }
   }
   else
   {
      index = (Integer) (epoch - historicStart);

      if (index < 0)
      {
         // Warn the user that the epoch is too early
         if (warnEpochBefore)
         {
            MessageInterface::ShowMessage("Warning: Requested epoch for solar "
                  "flux data is earlier than the starting epoch on the "
                  "historical flux file.  GMAT is using the first file entry.\n");
            warnEpochBefore = false;
         }
         index = 0;
      }

      #ifdef DEBUG_GETFLUXINPUTS
         MessageInterface::ShowMessage("observed data\n");
      #endif

      fD = obsFluxData[index];
      fD.index = index;
   }

#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Raw Data from read %d for epoch %.12lf:\n",
            numberReadIndex, epoch);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);
      MessageInterface::ShowMessage("   Record %s obs data\n\n", (fD.isObsData ? "is" : "is not"));
   }
#endif

   return fD;
}

//------------------------------------------------------------------------------
// PrepareApData(GmatEpoch epoch, Integer index, FluxData &fD)
//------------------------------------------------------------------------------
/**
 * Prepares the data used by the MSISE models
 *
 * Function that replaces the ap data with data MSISE models need, and the F10.7
 * obs value with the value from the previous da
 *
 * On return, the input FluxData object contains the following Ap data:
 *
 *     Ap[0]  DAILY AP
 *     Ap[1]  3 HR AP INDEX FOR CURRENT TIME
 *     Ap[2]  3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
 *     Ap[3]  3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
 *     Ap[4]  3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
 *     Ap[5]  AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
 *              TO CURRENT TIME
 *     Ap[6]  AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR
 *             TO CURRENT TIME
 *
 * per the MSIS specifications.
 *
 * @param fD The data record that contains raw data for the requested epoch
 *
 * @return void
*/
//------------------------------------------------------------------------------
void SolarFluxReader::PrepareApData(SolarFluxReader::FluxData &fD, GmatEpoch epoch )
{
   #ifdef DEBUG_FILE_INDEXING
      MessageInterface::ShowMessage("Reading flux data; epoch = %12lf, "
         "fD.Epoch = %.12lf, index = %d\n", epoch, fD.epoch, fD.index);
   #endif
   
   Integer f107index = fD.index;

   if (fD.isObsData)
   {
      Real fracEpoch = epoch - fD.epoch;
      Integer subIndex = (Integer)floor(fracEpoch * 8);

      // F10.7 is measured at 8 pm (5pm before 5/31/91), so we use the current
      // row for data from 8 am on the current day to 8 am the next day.
      // f107index is used to track that piece.
      Real f107Offset = (epoch < f107RefEpoch ? 5.0/24.0 : 8.0/24.0);
      if (fracEpoch < f107Offset)
         f107index = (f107index > 0 ? f107index - 1 : 0);

      if (subIndex >= 8) // Off the end of the array
         subIndex = 7;

      Real apValues[32];
      Integer i = 0,j = 0;

      #ifdef DEBUG_FILE_INDEXING
         MessageInterface::ShowMessage("   subindex = %d\n", subIndex);
      #endif
      for (i = subIndex, j = 0; i >= 0; --i, ++j)
         apValues[j] = fD.ap[i];
      if (fD.index > 0)
      {
         FluxData fD_OneBefore = obsFluxData[fD.index - 1];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_OneBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = obsFluxData[0].ap[0];
      }
      if (fD.index > 1)
      {
         FluxData fD_TwoBefore = obsFluxData[fD.index - 2];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_TwoBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = obsFluxData[0].ap[0];
      }

      if ( fD.index > 2)
      {
         FluxData fD_ThreeBefore = obsFluxData[fD.index - 3];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_ThreeBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = obsFluxData[0].ap[0];
      }


      for (i = 0; i < 7; i++)
      {
         if (i == 0)
            fD.ap[i] = fD.apAvg;
         if (i >= 1 && i <= 4)
            fD.ap[i] = apValues[i-1];
         if (i == 5)
         {
            fD.ap[i] = 0;
            for (Integer l = 0; l < 8; l++)
               fD.ap[i] += apValues[i+l-1];
            fD.ap[i] /= 8.0;
         }
         if (i == 6)
         {
            fD.ap[i] = 0;
            for (Integer l = 8; l < 16; l++)
               if ( apValues[i+l-2] >= 0)
                  fD.ap[i] += apValues[i+l-2];
            fD.ap[i] /= 8.0;
         }
      }

      // Update the F10.7 data and (if selected) interpolate
      if (interpolateFlux)
      {
         Real vals[2];
         Real portion = (f107Offset) + fracEpoch - 1.0/3.0;
         vals[1] = obsFluxData[f107index].obsF107;
         vals[0] = (f107index > 0 ? obsFluxData[f107index-1].obsF107 : vals[1]);
         fD.obsF107 = vals[0] + portion * (vals[1] - vals[0]);
      }
      else
      {
         // Daily value from previous day
         fD.obsF107 = (f107index > 0 ? obsFluxData[f107index-1].obsF107 :
                                       obsFluxData[f107index].obsF107);
      }

      // Average value from detected day
      fD.obsCtrF107a = obsFluxData[f107index].obsCtrF107a;
   }
   else // predict data
   {
      // For now, use nominal mean Schatten data
      fD.obsF107     = fD.F107a[schattenFluxIndex];
      fD.obsCtrF107a = fD.F107a[schattenFluxIndex];
      for (Integer i = 0; i < 8; i++)
         fD.ap[i] = fD.apSchatten[schattenApIndex];
   }


#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Data after Ap Prep from read %d:\n", numberReadIndex);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);

      ++numberReadIndex;
   }
#endif
}


//------------------------------------------------------------------------------
// PrepareKpData(GmatEpoch epoch, Integer index, FluxData &fD)
//------------------------------------------------------------------------------
/**
 * Function that replaces the Kp data with data the Jacchia Roberts model needs
 *
 * @param fD The data record that contains raw data for the requested epoch
 *
 * @return void
*/
//------------------------------------------------------------------------------
void SolarFluxReader::PrepareKpData(SolarFluxReader::FluxData &fD, GmatEpoch epoch )
{
   Integer f107index = fD.index;
   if ((fD.index < (Integer)obsFluxData.size()) && ((fD.index - 2) >= 0))
   {
      // Index >= 0 means obs data; -1 means predict data
      if (fD.index >= 0)
      {
         FluxData fD_OneBefore = obsFluxData[fD.index - 1];

         // Fill in fD.kp[0] so it contains the reading at epoch - 6.7 Hrs, per
         // Vallado and Finkleman
         Real fracEpoch = (epoch - 0.5) - fD.epoch - 6.7/24.0;
         Integer subIndex = (Integer)floor(fracEpoch * 8);

         // F10.7 is measured at 8 pm (5pm before ???), so we use the current row for
         // data from 8 am on the current day to 8 am the next day.  f107index is
         // used to track that piece.
         if (fracEpoch < 8.0/24.0)
            f107index = (f107index > 0 ? f107index - 1 : 0);

         if (subIndex >= 8)
            subIndex = 7;

         if (subIndex > 0)
            fD.kp[0] = fD.kp[subIndex];

         if (subIndex < 0)
            fD.kp[0] = fD_OneBefore.kp[8+subIndex];

         fD.adjF107 = obsFluxData[f107index].adjF107;
         fD.adjCtrF107a = obsFluxData[f107index].adjCtrF107a;
      }
   }
   else if (fD.index == -1) // predict data
   {
      fD.obsF107     = fD.F107a[schattenFluxIndex];
      fD.obsCtrF107a = fD.F107a[schattenFluxIndex];
      Real kp = ConvertApToKp(fD.apSchatten[schattenApIndex]);
      for (Integer i = 0; i < 8; i++)
         fD.kp[i] = kp;
   }
#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Data after Kp Prep from read %d:\n", numberReadIndex);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);

      ++numberReadIndex;
   }
#endif
}

//------------------------------------------------------------------------------
// void SolarFluxReader::GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
//                GmatEpoch &pEnd)
//------------------------------------------------------------------------------
/**
 * Accessor to retrieve the epoch data from the files
 *
 * @param hStart Epoch of the first historical data point
 * @param hEnd Epoch of the last historical data point
 * @param PStart Epoch of the first predict data point
 * @param pEnd Epoch of the last predict data point
 */
//------------------------------------------------------------------------------
void SolarFluxReader::GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
               GmatEpoch &pEnd)
{
   hStart = historicStart;
   hEnd = historicEnd;
   pStart = predictStart;
   pEnd = predictEnd;
}


//------------------------------------------------------------------------------
// Real ConvertApToKp(Real ap)
//------------------------------------------------------------------------------
/**
 * Conversion routine to go Ap -> Kp
 *
 * The current implementation performs linear interpolation.  Vallado recommends
 * cubic splines instead.
 *
 * @param ap The input Ap value
 *
 * @return The corresponding Kp
 */
//------------------------------------------------------------------------------
Real SolarFluxReader::ConvertApToKp(Real ap)
{
   Real kp = 0.0;
   Real apl, apr, kpl, kpr;

   // For now, linear interpolate.  Vallado recommeds splines.
   if (ap <= 2.0)
   {
      apl = 0.0;      apr = 2.0;
      kpl = 0.0;      kpr = 0.33;
   }
   else if (ap <= 3.0)
   {
      apl = 2.0;      apr = 3.0;
      kpl = 0.33;     kpr = 0.67;
   }
   else if (ap <= 4.0)
   {
      apl = 3.0;      apr = 4.0;
      kpl = 0.67;     kpr = 1.0;
   }
   else if (ap <= 5.0)
   {
      apl = 4.0;      apr = 5.0;
      kpl = 1.0;      kpr = 1.33;
   }
   else if (ap <= 6.0)
   {
      apl = 5.0;      apr = 6.0;
      kpl = 1.33;     kpr = 1.67;
   }
   else if (ap <= 7.0)
   {
      apl = 6.0;      apr = 7.0;
      kpl = 1.67;     kpr = 2.0;
   }
   else if (ap <= 9.0)
   {
      apl = 7.0;      apr = 9.0;
      kpl = 2.0;      kpr = 2.33;
   }
   else if (ap <= 12.0)
   {
      apl = 9.0;      apr = 12.0;
      kpl = 2.33;     kpr = 2.67;
   }
   else if (ap <= 15.0)
   {
      apl = 12.0;     apr = 15.0;
      kpl = 2.67;     kpr = 3.0;
   }
   else if (ap <= 18.0)
   {
      apl = 15.0;     apr = 18.0;
      kpl = 3.0;      kpr = 3.33;
   }
   else if (ap <= 22.0)
   {
      apl = 18.0;     apr = 22.0;
      kpl = 3.33;     kpr = 3.67;
   }
   else if (ap <= 27.0)
   {
      apl = 22.0;     apr = 27.0;
      kpl = 3.67;     kpr = 4.0;
   }
   else if (ap <= 32.0)
   {
      apl = 27.0;     apr = 32.0;
      kpl = 4.0;      kpr = 4.33;
   }
   else if (ap <= 39.0)
   {
      apl = 32.0;     apr = 39.0;
      kpl = 4.33;     kpr = 4.67;
   }
   else if (ap <= 48.0)
   {
      apl = 39.0;     apr = 48.0;
      kpl = 4.67;     kpr = 5.0;
   }
   else if (ap <= 56.0)
   {
      apl = 48.0;     apr = 56.0;
      kpl = 5.0;      kpr = 5.33;
   }
   else if (ap <= 67.0)
   {
      apl = 56.0;     apr = 67.0;
      kpl = 5.33;     kpr = 5.67;
   }
   else if (ap <= 80.0)
   {
      apl = 67.0;     apr = 80.0;
      kpl = 5.67;     kpr = 6.0;
   }
   else if (ap <= 94.0)
   {
      apl = 80.0;     apr = 94.0;
      kpl = 6.0;      kpr = 6.33;
   }
   else if (ap <= 111.0)
   {
      apl = 94.0;     apr = 111.0;
      kpl = 6.33;     kpr = 6.67;
   }
   else if (ap <= 132.0)
   {
      apl = 111.0;    apr = 132.0;
      kpl = 6.67;     kpr = 7.0;
   }
   else if (ap <= 154.0)
   {
      apl = 132.0;    apr = 154.0;
      kpl = 7.0;      kpr = 7.33;
   }
   else if (ap <= 179.0)
   {
      apl = 154.0;    apr = 179.0;
      kpl = 7.33;     kpr = 7.67;
   }
   else if (ap <= 207.0)
   {
      apl = 179.0;    apr = 207.0;
      kpl = 7.67;     kpr = 8.0;
   }
   else if (ap <= 236.0)
   {
      apl = 207.0;    apr = 236.0;
      kpl = 8.0;      kpr = 8.33;
   }
   else if (ap <= 300.0)
   {
      apl = 236.0;    apr = 300.0;
      kpl = 8.33;     kpr = 8.67;
   }
   else
   {
      apl = 300.0;    apr = 400.0;
      kpl = 8.67;     kpr = 9.0;
   }

   kp = kpl + (ap - apl) * (kpr - kpl) / (apr - apl);

   return kp;
}


//------------------------------------------------------------------------------
// void SetSchattenFlags(Integer timingSet, Integer magnitudeSet)
//------------------------------------------------------------------------------
/**
 * Sets the indices into the data for Schatten predicts
 *
 * @param timingSet  Sets timing selection: -1 => early, 0 => nominal, +1 => late
 * @param magnitudeSet Sets magnitude selection: -1 => -2 sigma, 0 => nominal,
 *                                               +1 =>  +2 sigma
 */
//------------------------------------------------------------------------------
void SolarFluxReader::SetSchattenFlags(Integer timingSet, Integer magnitudeSet)
{
   #ifdef DEBUG_SCHATTEN_SETTINGS
      MessageInterface::ShowMessage("Setting flux reader timing to %d and "
            "error model to %d\n", timingSet, magnitudeSet);
   #endif

   if (timingSet == -1)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 5;
      if (magnitudeSet == 0)
         schattenFluxIndex = 3;
      if (magnitudeSet == 1)
         schattenFluxIndex = 4;
      schattenApIndex = 1;
   }

   if (timingSet == 0)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 2;
      if (magnitudeSet == 0)
         schattenFluxIndex = 0;
      if (magnitudeSet == 1)
         schattenFluxIndex = 1;
      schattenApIndex = 0;
   }

   if (timingSet == 1)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 8;
      if (magnitudeSet == 0)
         schattenFluxIndex = 6;
      if (magnitudeSet == 1)
         schattenFluxIndex = 7;
      schattenApIndex = 2;
   }

   #ifdef DEBUG_FIRSTFEW_READS
      numberReadIndex = 0;
   #endif
}
