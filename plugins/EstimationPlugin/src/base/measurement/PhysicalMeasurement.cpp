//$Id: PhysicalMeasurement.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         PhysicalMeasurement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/16
//
/**
 * Base class for real world measurement primitives
 */
//------------------------------------------------------------------------------


#include "PhysicalMeasurement.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GroundstationInterface.hpp"
#include "MeasurementException.hpp"
#include <sstream>

#ifdef IONOSPHERE
#include "CoordinateConverter.hpp"
#include "Moderator.hpp"
#endif

//#define DEBUG_DERIVATIVES
//#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define DEBUG_MEDIA_CORRECTION
//#define DEBUG_TROPOSPHERE_MEDIA_CORRECTION
//#define DEBUG_IONOSPHERE_MEDIA_CORRECTION
//#define DEBUG_SET_RELATIVITY_CORRECTION
//#define DEBUG_SET_ETMINUSTAI_CORRECTION
//------------------------------------------------------------------------------
// PhysicalMeasurement(const std::string &type, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The type of the PhysicalMeasurement
 * @param nomme The measurement's name
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::PhysicalMeasurement(const std::string &type,
			const std::string &nomme) :
   CoreMeasurement      (type, nomme),
//   frequency            (2090659968.0),			// made changes by TUAN NGUYEN
   frequency            (0.0),						// made changes by TUAN NGUYEN
   freqBand             (0),						// made changes by TUAN NGUYEN
///// TBD: Determine if there is a more generic way to add these
   rangeModulo          (1.0e6),					// made changes by TUAN NGUYEN
   rampTB               (NULL),						// made changes by TUAN NGUYEN
   obsData              (NULL),						// made changes by TUAN NGUYEN
   useRelativityCorrection (false),					// made changes by TUAN NGUYEN
   useETminusTAICorrection (false)					// made changes by TUAN NGUYEN
{
   objectTypeNames.push_back("PhysicalMeasurement");

   troposphere	= NULL;
   #ifdef IONOSPHERE
      ionosphere = NULL;
   #endif
}


//------------------------------------------------------------------------------
// ~PhysicalMeasurement()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::~PhysicalMeasurement()
{
	if (troposphere != NULL)
		delete troposphere;

   #ifdef IONOSPHERE
	   if (ionosphere != NULL)
		   delete ionosphere;
   #endif
}


//------------------------------------------------------------------------------
// PhysicalMeasurement(const PhysicalMeasurement& pm) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param pm The PhysicalMeasurement that is getting copied here
 */
//------------------------------------------------------------------------------
PhysicalMeasurement::PhysicalMeasurement(const PhysicalMeasurement& pm) :
   CoreMeasurement      (pm),
   frequency            (pm.frequency),
   freqBand             (pm.freqBand),						// made changes by TUAN NGUYEN
   rangeModulo          (pm.rangeModulo),					// made changes by TUAN NGUYEN
   rampTB               (pm.rampTB),						// made changes by TUAN NGUYEN
   obsData              (pm.obsData),						// made changes by TUAN NGUYEN
   useRelativityCorrection (pm.useRelativityCorrection),	// made changes by TUAN NGUYEN
   useETminusTAICorrection (pm.useETminusTAICorrection)		// made changes by TUAN NGUYEN
{
   if (pm.troposphere != NULL)
      troposphere = new Troposphere(*(pm.troposphere));
   else
      troposphere = NULL;

   #ifdef IONOSPHERE
      if (pm.ionosphere != NULL)
         ionosphere = new Ionosphere(*(pm.ionosphere));
      else
         ionosphere = NULL;
   #endif
}


//------------------------------------------------------------------------------
// PhysicalMeasurement& operator=(const PhysicalMeasurement& pm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param pm The PhysicalMeasurement that supplies new data for this one
 *
 * @return This PhysicalMeasurement
 */
//------------------------------------------------------------------------------
PhysicalMeasurement& PhysicalMeasurement::operator=(
      const PhysicalMeasurement& pm)
{
   if (this != &pm)
   {
      CoreMeasurement::operator=(pm);

      frequency = pm.frequency;
	  freqBand  = pm.freqBand;					// made changes by TUAN NGUYEN
	  rangeModulo = pm.rangeModulo;				// made changes by TUAN NGUYEN
	  rampTB      = pm.rampTB;					// made changes by TUAN NGUYEN
	  obsData     = pm.obsData;					// made changes by TUAN NGUYEN
	  useRelativityCorrection = pm.useRelativityCorrection; // made changes by TUAN NGUYEN
	  useETminusTAICorrection = pm.useETminusTAICorrection; // made changes by TUAN NGUYEN

      // Rebuild the correction models
      if (troposphere != NULL)
         delete troposphere;

      if (pm.troposphere != NULL)
         troposphere = new Troposphere(*(pm.troposphere));
      else
         troposphere = NULL;

      #ifdef IONOSPHERE
         if (ionosphere != NULL)
            delete ionosphere;

         if (pm.ionosphere != NULL)
            ionosphere = new Ionosphere(*(pm.ionosphere));
         else
            ionosphere = NULL;
      #endif
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetConstantFrequency(Real newFreq)
//------------------------------------------------------------------------------
/**
 * Sets a frequency value on the measurement
 *
 * @param newFreq The new frequency value
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::SetConstantFrequency(Real newFreq, Integer index)
{
   if (newFreq > 0.0)
   {
      if (index == 0)
         frequency = frequencyE = newFreq;
	  else
         frequencyE = newFreq;
   }
   else
	  throw MeasurementException("Error: Frequency was set to a non-positive value.\n"); 
}


//------------------------------------------------------------------------------
// Real PhysicalMeasurement::GetConstantFrequency()
//------------------------------------------------------------------------------
/**
 * Retrieves the measurement frequency
 *
 * @return The frequency
 */
//------------------------------------------------------------------------------
Real PhysicalMeasurement::GetConstantFrequency(Integer index)
{
   return ((index == 0) ? frequency : frequencyE);
}


///// TBD: Determine if there is a more generic way to add these
// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetFrequencyBand(Integer frequencyBand, Integer index)
{
	if (index == 0)
		freqBand = freqBandE = frequencyBand;
	else
		freqBandE = frequencyBand;
}

// made changes by TUAN NGUYEN
Integer PhysicalMeasurement::GetFrequencyBand(Integer index)
{
   return ((index == 0) ? freqBand : freqBandE);
}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetRangeModulo(Real rangeMod)
{
	rangeModulo = rangeMod;
}


// made changes by TUAN NGUYEN
Real PhysicalMeasurement::GetRangeModulo()
{
	return rangeModulo;
}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetObsValue(const RealArray& value)
{
	obsValue = value;
}

// made changes by TUAN NGUYEN
RealArray PhysicalMeasurement::GetObsValue()
{
	return obsValue;
}

// made changes by TUAN NGUYEN
//void PhysicalMeasurement::SetFlagToReadFromObservationData(bool isFromObservationData)
//{
//	isFromObsData = isFromObservationData;
//}

// made changes by TUAN NGUYEN
//bool PhysicalMeasurement::GetFlagToReadFromObservationData()
//{
//	return isFromObsData;
//}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetObservationDataRecord(ObservationData* data)
{
	obsData = data;
}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetRampTable(std::vector<RampTableData>* rt)
{
   rampTB = rt;
}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetRelativityCorrection(bool useCorr)
{
#ifdef DEBUG_SET_RELATIVITY_CORRECTION
	MessageInterface::ShowMessage("Start PhysicalMeasurement::SetRelativityCorrection(%s)\n", (useCorr ? "true":"false"));
#endif
   useRelativityCorrection = useCorr;

#ifdef DEBUG_SET_RELATIVITY_CORRECTION
	MessageInterface::ShowMessage("Exit PhysicalMeasurement::SetRelativityCorrection()\n");
#endif
}

// made changes by TUAN NGUYEN
void PhysicalMeasurement::SetETMinusTAICorrection(bool useCorr)
{
#ifdef DEBUG_SET_ETMINUSTAI_CORRECTION
	MessageInterface::ShowMessage("Start PhysicalMeasurement::SetETMinusTAICorrection(%s)\n", (useCorr ? "true":"false"));
#endif

	useETminusTAICorrection = useCorr;

#ifdef DEBUG_SET_ETMINUSTAI_CORRECTION
	MessageInterface::ShowMessage("Exit PhysicalMeasurement::SetETMinusTAICorrection()\n");
#endif
}

//----------------------------------------------------------------------
// void AddCorrection(std::string& modelName)
//----------------------------------------------------------------------
/**
 * This function is used to add media correction model to the measurement
 *
 * @param nodelName	The name of media correction model involving in the
 * 						measurement
 */
//----------------------------------------------------------------------
void PhysicalMeasurement::AddCorrection(const std::string& modelName)
{
   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage("Adding correction named %s\n",
            modelName.c_str());
   #endif

	if (modelName == "HopfieldSaastamoinen")
	{
	   if (troposphere != NULL)
	      delete troposphere;

	   // Create troposphere correction model
	   troposphere = new Troposphere(modelName);

       #ifdef DEBUG_MEDIA_CORRECTION
          MessageInterface::ShowMessage("   Set as troposphere model:   troposphere(%p)\n", troposphere);
       #endif
	}
	else if (modelName == "IRI2007")							// made changes by TUAN NGUYEN
	{
      #ifdef DEBUG_MEDIA_CORRECTION
         MessageInterface::ShowMessage("   Set as ionosphere model\n");
      #endif

///// TBD: Determine if there is a more generic way to add these
	  // Create IRI2007 ionosphere correction model
      #ifdef IONOSPHERE
		   ionosphere = new Ionosphere(modelName);
      #else
		   MessageInterface::ShowMessage("Ionosphere IRI2007 model currently is not "
				         "available.\nIt will be be added to GMAT in a future release.\n");

		   throw new GmatBaseException("Ionosphere IRI2007 model currently is not "
		         "available.\nIt will be be added to GMAT in a future release.\n");
      #endif
	}
	else if (modelName == "None")
	{
	}
	else
	{
		MessageInterface::ShowMessage("Error: '%s' is not allowed for media correction name.\n", modelName.c_str());
		MessageInterface::ShowMessage("Only 2 media correction model names are allowed: HopfieldSaastamoinen and IRI2007\n");
		throw new MeasurementException("Error: media correction model name is not allowed\n"
			"Only 2 media correction model names are allowed: HopfieldSaastamoinen and IRI2007\n");
	}
}


//------------------------------------------------------------------------
//RealArray TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix Ro_j2k)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Troposphere correction.
 *
 * @param freq		The frequency of signal	(unit: MHz)
 * @param rVec		Range vector in J2k coordinate system
 * @param Ro_j2k Rotational matrix converting j2k coordinate system to
 *                  topocentric coordinate system
 *
 */
//------------------------------------------------------------------------
/*
RealArray PhysicalMeasurement::TroposphereCorrection(Real freq, Rvector3 rVec, Rmatrix33 Ro_j2k)
{
   RealArray tropoCorrection;

   if (troposphere != NULL)
   {
   	Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);
   	troposphere->SetWaveLength(wavelength);
	Rvector3 signal_direction = Ro_j2k *rVec.GetUnitVector(); 
   	Real elevationAngle = asin(signal_direction.GetElement(2));
   	troposphere->SetElevationAngle(elevationAngle);
   	troposphere->SetRange(rVec.GetMagnitude()*GmatMathConstants::KM_TO_M);
   	tropoCorrection = troposphere->Correction();
//   	Real rangeCorrection = tropoCorrection[0]/GmatMathConstants::KM_TO_M;

		#ifdef DEBUG_TROPOSPHERE_MEDIA_CORRECTION
			MessageInterface::ShowMessage("       *Run Troposphere media correction:\n");
			MessageInterface::ShowMessage("         .Wave length = %.12lf m\n", wavelength);
			MessageInterface::ShowMessage("         .Elevation angle = %.12lf degree\n", elevationAngle*GmatMathConstants::DEG_PER_RAD);
			MessageInterface::ShowMessage("         .Range correction = %.12lf m\n", tropoCorrection[0]);
		#endif
   }
   else
   {
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   }

   return tropoCorrection;
}
*/



//------------------------------------------------------------------------
//RealArray TroposphereCorrection(Real freq, Real distance, Real elevationAngle)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Troposphere correction.
 *
 * @param freq				The frequency of signal	(unit: MHz)
 * @param distance			Distance from ground station to spacecraft (unit: km)
 * @param elevationAngle    The elevation angle from ground station to spacecraft (unit: radian)
 *                 
 * return Troposphere correction vector
 */
//------------------------------------------------------------------------
RealArray PhysicalMeasurement::TroposphereCorrection(Real freq, Real distance, Real elevationAngle)
{
   RealArray tropoCorrection;

   if (troposphere != NULL)
   {
    // Set troposphere's ref objects						// made changes by TUAN NGUYEN
	troposphere->SetSolarSystem(solarSystem);				// made changes by TUAN NGUYEN

   	Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);
   	troposphere->SetWaveLength(wavelength);
   	troposphere->SetElevationAngle(elevationAngle);
   	troposphere->SetRange(distance*GmatMathConstants::KM_TO_M);
   	tropoCorrection = troposphere->Correction();
//   	Real rangeCorrection = tropoCorrection[0]/GmatMathConstants::KM_TO_M;

		#ifdef DEBUG_TROPOSPHERE_MEDIA_CORRECTION
			MessageInterface::ShowMessage("       *Run Troposphere media correction:\n");
			MessageInterface::ShowMessage("         .Wave length = %.12lf m\n", wavelength);
			MessageInterface::ShowMessage("         .Elevation angle = %.12lf degree\n", elevationAngle*GmatMathConstants::DEG_PER_RAD);
			MessageInterface::ShowMessage("         .Range correction = %.12lf m\n", tropoCorrection[0]);
		#endif
   }
   else
   {
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   	tropoCorrection.push_back(0.0);
   }

   return tropoCorrection;
}



//------------------------------------------------------------------------
//RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Ionosphere correction.
 *
 * @param freq		The frequency of signal		(unit: MHz)
 * @param r1		Position of ground station	(unit: km)
 * @param r2		Position of spacecraft		(unit:km)
 * @param epoch	Time at which the signal is transmitted or received from ground station		(unit: Julian day)
 */
//------------------------------------------------------------------------
#ifdef IONOSPHERE
RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
{
   RealArray ionoCorrection;

   if (ionosphere != NULL)
   {
    // 0. Set ionosphere's ref objects						// made changes by TUAN NGUYEN
    ionosphere->SetSolarSystem(solarSystem);				// made changes by TUAN NGUYEN

   	// 1. Set wave length:
   	Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);		// unit: meter
   	ionosphere->SetWaveLength(wavelength);

   	// 2. Set time:
   	ionosphere->SetTime(epoch);															// unit: Julian day

   	// 3. Set station and spacecraft positions:
   	GroundstationInterface* gs 	= (GroundstationInterface*)participants[0];
   	CoordinateSystem* cs = gs->GetBodyFixedCoordinateSystem();
   	Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
	Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
//   	Rvector bogusOut 		= cs->FromMJ2000Eq(epoch, bogusIn, true);
//   	Rmatrix33 R_g_j2k    = cs->GetLastRotationMatrix().Transpose();
	CoordinateConverter* cv = new CoordinateConverter();
	A1Mjd time(epoch);
	CoordinateSystem* fk5cs = Moderator::Instance()->GetCoordinateSystem("EarthMJ2000Eq");
	cv->Convert(time, inState, cs, outState, fk5cs);				// convert EarthFK5 coordinate system to EarthBodyFixed coordinate system
	Rmatrix33 R_g_j2k    = cv->GetLastRotationMatrix().Transpose();
	Rvector3 r1_ebf = R_g_j2k*r1;
	Rvector3 r2_ebf = R_g_j2k*r2;

//	ionosphere->SetStationPosition(R_g_j2k*r1);											// unit: km
// 	ionosphere->SetSpacecraftPosition(R_g_j2k*r2);										// unit: km
	ionosphere->SetStationPosition(r1_ebf);											// unit: km
   	ionosphere->SetSpacecraftPosition(r2_ebf);										// unit: km

   	// 4. Set earth radius:
	SpacePoint* earth 	= (SpacePoint*)gs->GetRefObject(Gmat::SPACE_POINT, "Earth");
   	Real earthRadius 	= earth->GetRealParameter("EquatorialRadius");
   	ionosphere->SetEarthRadius(earthRadius);											// unit: km

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
	MessageInterface::ShowMessage("      *Run Ionosphere media correction for:\n");
	MessageInterface::ShowMessage("         +Earth radius = %lf km\n", earthRadius);
	MessageInterface::ShowMessage("         +Wave length = %.12lf m\n", wavelength);
	MessageInterface::ShowMessage("         +Time = %.12lf\n", epoch);
	MessageInterface::ShowMessage("         +Station location in Earth body fixed coordinate system (km)   : (%.12lf,  %.12lf,   %.12lf)\n", r1_ebf[0], r1_ebf[1], r1_ebf[2]); 
	MessageInterface::ShowMessage("         +Spacecraft location in Earth body fixed coordinate system (km): (%.12lf,  %.12lf,   %.12lf)\n", r2_ebf[0], r2_ebf[1], r2_ebf[2]);
#endif

	// 5. Run ionosphere correction:
   	ionoCorrection = ionosphere->Correction();
   	Real rangeCorrection = ionoCorrection[0]*GmatMathConstants::M_TO_KM;				// unit: meter

#ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
//	MessageInterface::ShowMessage("      *Ionosphere media correction result:\n");
	MessageInterface::ShowMessage("         +Range correction = %.12lf m\n", rangeCorrection*GmatMathConstants::KM_TO_M);
#endif
   }
   else
   {
   	ionoCorrection.push_back(0.0);
   	ionoCorrection.push_back(0.0);
   	ionoCorrection.push_back(0.0);
   }

   return ionoCorrection;
}
#endif

//------------------------------------------------------------------------
// RealArray CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
//------------------------------------------------------------------------
/**
 * This function is used to calculate media corrections.
 *
 * @param freq		The frequency of signal	(unit: MHz)
 * @param r1		Position of ground station
 * @param r2		Position of spacecraft
 * @param epoch	    The time at which signal is transmitted from or received at ground station
 */
//------------------------------------------------------------------------
RealArray PhysicalMeasurement::CalculateMediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch)
{
   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage("start PhysicalMeasurement::CalculateMediaCorrection()\n");
   #endif

   RealArray mediaCorrection;

   // 1. Run Troposphere correction:
   UpdateRotationMatrix(epoch, "o_j2k");
   Rvector3 rangeVector = r2 - r1;										           // vector pointing from ground station to spacecraft
   Real elevationAngle = asin((R_o_j2k*(rangeVector.GetUnitVector())).GetElement(2)); 
//   Real elevationAngle = GmatMathConstants::PI/2 - acos(rangeVector.GetUnitVector() * r1.GetUnitVector()); 
   //mediaCorrection = TroposphereCorrection(freq, rangeVector, R_o_j2k);
   mediaCorrection = TroposphereCorrection(freq, rangeVector.GetMagnitude(), elevationAngle);

   #ifdef DEBUG_MEDIA_CORRECTION
      MessageInterface::ShowMessage(" frequency = %le MHz, epoch = %lf,     r2-r1 = ('%s')km\n", freq, epoch, (r2-r1).ToString().c_str());
	  MessageInterface::ShowMessage(" TroposhereCorrection = (%lf m,  %lf arcsec,   %le s)\n", mediaCorrection[0], mediaCorrection[1], mediaCorrection[2]);
   #endif

   #ifdef IONOSPHERE
      // 2. Run Ionosphere correction:
      RealArray ionoCorrection = this->IonosphereCorrection(freq, r1, r2, epoch);

      // 3. Combine effects:
      mediaCorrection[0] += ionoCorrection[0];
      mediaCorrection[1] += ionoCorrection[1];
      mediaCorrection[2] += ionoCorrection[2];
   #endif

   return mediaCorrection;
}


//------------------------------------------------------------------------------
// void InitializeMeasurement()
//------------------------------------------------------------------------------
/**
 * Initializes the measurement for use in estimation or simulation
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::InitializeMeasurement()
{
   CoreMeasurement::InitializeMeasurement();
}


//------------------------------------------------------------------------------
// void GetRangeDerivative(Event &ev, const Rmatrix &stmInv, Rvector &deriv,
//       bool wrtP1, Integer p1Index, Integer p2Index, bool wrtR, bool wrtV)
//------------------------------------------------------------------------------
/**
 * Calculates the range derivative of a leg of a measurement
 *
 * @param ev The event associated with the leg
 * @param stmInv The STM inverse
 * @param deriv The structure that gets filled with derivative data
 * @param wrtP1 Flag indicating if the derivative is w.r.t. participant 1 or 2
 * @param p1Index Index of the "transmitting" participant
 * @param p2Index Index of the "receiving" participant
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetRangeDerivative(Event &ev, const Rmatrix &stmInv,
      Rvector &deriv, bool wrtP1, Integer p1Index, Integer p2Index, bool wrtR,
      bool wrtV)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("PhysicalMeasurement::GetRangeDerivative(%s, "
            "stmInv, deriv, %d <%s>, %d <%s>, %s, %s)\n", ev.GetName().c_str(),
            p1Index, participants[p1Index]->GetName().c_str(), p2Index,
            participants[p2Index]->GetName().c_str(),
            (wrtR == true ? "true" : "false"),
            (wrtV == true ? "true" : "false"));
   #endif

   Rmatrix derivMatrix;
   if (wrtR && wrtV)
      derivMatrix.SetSize(6,6);
   else
      derivMatrix.SetSize(3,3);

   GetRangeVectorDerivative(ev, stmInv, derivMatrix, wrtP1, p1Index, p2Index,
         wrtR, wrtV);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Derivative matrix = \n");
      for (Integer i = 0; i < derivMatrix.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("      [");
         for (Integer j = 0; j < derivMatrix.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", derivMatrix(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   Rvector3 temp;
   Rmatrix33 mPart;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();

   if (wrtR)
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i,j);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i] = temp(i);
   }
   if (wrtV)
   {
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i+offset, j+offset);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i+offset] = temp(i);
   }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("Range unit vector = %s\n",unitRange.ToString()); 
         MessageInterface::ShowMessage("   Derivative = [");
         for (Integer i = 0; i < deriv.GetSize(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", deriv[i]);
         MessageInterface::ShowMessage("]\n");
      #endif
}


//------------------------------------------------------------------------------
// void GetRangeVectorDerivative(Event &ev, const Rmatrix &stmInv,
//       Rmatrix &deriv, bool wrtP1, Integer p1Index, Integer p2Index,
//       bool wrtR, bool wrtV)
//------------------------------------------------------------------------------
/**
 * Calculates the range vector derivative of a leg of a measurement
 *
 * @param ev The event associated with the leg
 * @param stmInv The STM inverse
 * @param deriv The structure that gets filled with derivative data
 * @param wrtP1 Flag indicating if the derivative is w.r.t. participant 1 or 2
 * @param p1Index Index of the "transmitting" participant
 * @param p2Index Index of the "receiving" participant
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetRangeVectorDerivative(Event &ev,
      const Rmatrix &stmInv, Rmatrix &deriv, bool wrtP1, Integer p1Index,
      Integer p2Index, bool wrtR, bool wrtV)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("PhysicalMeasurement::GetRangeVector"
            "Derivative(%s, stmInv, deriv, %d <%s>, %d <%s>, %s, %s)\n",
            ev.GetName().c_str(), p1Index,
            participants[p1Index]->GetName().c_str(), p2Index,
            participants[p2Index]->GetName().c_str(),
            (wrtR == true ? "true" : "false"),
            (wrtV == true ? "true" : "false"));
   #endif

   EventData p1Data = ev.GetEventData(participants[p1Index]);
   EventData p2Data = ev.GetEventData(participants[p2Index]);
   
   /// Calculate range vector in Solar System Barycenter coordinate system:
   // rangeVec = p2Data.position - p1Data.position;												// made changes by TUAN NGUYEN
   CelestialBody* cb1 = (CelestialBody*)p1Data.cs_origin;										// made changes by TUAN NGUYEN
   CelestialBody* cb2 = (CelestialBody*)p2Data.cs_origin;										// made changes by TUAN NGUYEN
   Real t1 = p1Data.epoch;																		// made changes by TUAN NGUYEN
   Real t2 = p2Data.epoch;																		// made changes by TUAN NGUYEN
   SpecialCelestialPoint* ssb = (SpecialCelestialPoint*)solarSystem->GetSpecialPoint("SolarSystemBarycenter");		// made changes by TUAN NGUYEN

   // vector from SSB to the central body at time t1 and t2										// made changes by TUAN NGUYEN
   Rvector3 BE1 = cb1->GetMJ2000Position(t1) - ssb->GetMJ2000Position(t1);						// made changes by TUAN NGUYEN
   Rvector3 BE2 = cb2->GetMJ2000Position(t2) - ssb->GetMJ2000Position(t2);						// made changes by TUAN NGUYEN
   Rvector3 p1B = BE1 + p1Data.position;														// made changes by TUAN NGUYEN
   Rvector3 p2B = BE2 + p2Data.position;														// made changes by TUAN NGUYEN
   rangeVec = p2B - p1B;																		// made changes by TUAN NGUYEN

   // Be sure to use the correct rotation matrices, etc
   EventData dataToUse = (wrtP1 ? p1Data : p2Data);
   // p1 derivatives pick up a minus sign, handled with this variable
   Real sign = (wrtP1 ? -1.0 : 1.0);


   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("p1Position in Solar System Barycenter coordinate system: %s\n", p1B.ToString().c_str());
      MessageInterface::ShowMessage("p2Position in Solar System Barycenter coordinate system: %s\n", p2B.ToString().c_str());

      MessageInterface::ShowMessage("   Range vector = P2B - P1B = : [%.12lf %.12lf %.12lf]\n",
            rangeVec[0], rangeVec[1], rangeVec[2]);
   #endif

   Rmatrix phi = dataToUse.stm * stmInv;

   Rmatrix33 A, B;
   for (Integer i = 0; i < 3; ++i)
      for (Integer j = 0; j < 3; ++j)
      {
         if (wrtR)
            A(i,j) = phi(i,j);
         if(wrtV)
            B(i,j) = phi(i,j+3);
      }

   Rmatrix33 temp;

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("Phi: %s\n", phi.ToString().c_str());
      MessageInterface::ShowMessage("  A: %s\n", A.ToString().c_str());
      MessageInterface::ShowMessage("  B: %s\n", B.ToString().c_str());
   #endif

   Rmatrix33 body2FK5_matrix = dataToUse.rInertial2obj.Transpose();		// made change by TUAN NGUYEN:  get rotation matrix from object frame to inertial frame

   if (wrtR)
   {
      // temp = dataToUse.rInertial2obj * A;
	  temp = body2FK5_matrix * A;		// made change by TUAN NGUYEN

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            deriv(i,j) = sign * temp(i,j);
   }
   if (wrtV)
   {
      // temp = dataToUse.rInertial2obj * B;
	  temp = body2FK5_matrix * B;		// made change by TUAN NGUYEN

      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            deriv(i+offset,j+offset) = sign * temp(i,j);
   }

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Rotation matrix from body to FK5 for %s:\n",
            participants[p2Index]->GetName().c_str());
      for (Integer i = 0; i < 3; ++i)
      {
         MessageInterface::ShowMessage("         [");
         for (Integer j = 0; j < 3; ++j)
            MessageInterface::ShowMessage(" %.12lf ",
                  body2FK5_matrix(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif
}

//------------------------------------------------------------------------------
// void GetInverseSTM(GmatBase *forObject, Rmatrix &stmInv)
//------------------------------------------------------------------------------
/**
 * Builds the inverse state transition matrix
 *
 * @param forObject The object supplying the STM
 * @param stmInverse The matrix that gets filled with the STM inverse
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::GetInverseSTM(GmatBase *forObject, Rmatrix &stmInv)
{
   Integer stmId;
   try
   {
      stmId = forObject->GetParameterID("CartesianX");
   }
   catch (BaseException &)
   {
      stmId = -1;
   }

   if (stmId >= 0)
   {
      stmInv = (forObject->GetParameterSTM(stmId))->Inverse();
   }
   else
   {
      // Use identity if there is no STM
      stmInv(0,0) = stmInv(1,1) = stmInv(2,2) = stmInv(3,3)
                  = stmInv(4,4) = stmInv(5,5) = 1.0;
      stmInv(0,1) = stmInv(0,2) = stmInv(0,3) = stmInv(0,4) = stmInv(0,5)
                  = stmInv(1,0) = stmInv(1,2) = stmInv(1,3) = stmInv(1,4)
                  = stmInv(1,5) = stmInv(2,0) = stmInv(2,1) = stmInv(2,3)
                  = stmInv(2,4) = stmInv(2,5) = stmInv(3,0) = stmInv(3,1)
                  = stmInv(3,2) = stmInv(3,4) = stmInv(3,5) = stmInv(4,0)
                  = stmInv(4,1) = stmInv(4,2) = stmInv(4,3) = stmInv(4,5)
                  = stmInv(5,0) = stmInv(5,1) = stmInv(5,2) = stmInv(5,3)
                  = stmInv(5,4) = 0.0;
   }

   #ifdef DEBUG_STM_INVERTER
      Rmatrix mat = *forObject->GetParameterSTM(stmId);
      MessageInterface::ShowMessage("STM:\n");
      for (Integer i = 0; i < mat.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("   [");
         for (Integer j = 0; j < mat.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", mat(i,j));
         MessageInterface::ShowMessage("]\n");
      }

      mat = stmInv;
      MessageInterface::ShowMessage("STM Inverse:\n");
      for (Integer i = 0; i < mat.GetNumRows(); ++i)
      {
         MessageInterface::ShowMessage("   [");
         for (Integer j = 0; j < mat.GetNumColumns(); ++j)
            MessageInterface::ShowMessage(" %.12lf ", mat(i,j));
         MessageInterface::ShowMessage("]\n");
      }
   #endif
}


//------------------------------------------------------------------------------
// void SetHardwareDelays()
//------------------------------------------------------------------------------
/**
 * Retrieves delay vales and passes them into the events that need them.
 *
 * The default implementation does nothing but define the interface used in
 * derived classes that override it.
 */
//------------------------------------------------------------------------------
void PhysicalMeasurement::SetHardwareDelays(bool loadEvents)
{
}


///// TBD: Determine if there is a more generic way to add these
///// These methods need to be documented
Integer PhysicalMeasurement::GetUplinkBandFromRampTable(Real t)
{
   // search for interval which contains time t:
   Real upBand = 0;
   for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   {
      if (t < (*rampTB)[i].epoch)
	  {
         upBand = (*rampTB)[i-1].uplinkBand;      
		 break;
	  }
   }

   return upBand;
}

Real PhysicalMeasurement::GetFrequencyFromRampTable(Real t)
{
   // search for interval which contains time t:
   UnsignedInt interval_index = 0;
   for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   {
      if (t < (*rampTB)[i].epoch)
	  {
         interval_index = i-1;      
		 break;
	  }
   }

   // specify frequency at time t:
   Real t_start = (*rampTB)[interval_index].epoch;
   Real f0 = (*rampTB)[interval_index].rampFrequency;
   Real f_dot = (*rampTB)[interval_index].rampRate;
	   
   Real f = f0 + f_dot*(t - t_start);

   return f;
}

//------------------------------------------------------------------------------
// Integer GetFrequencyBand(Real frequency)
//------------------------------------------------------------------------------
/**
 * Get frequency band corresponding to a given frequency
 *
 * @param frequency   frequency in Hz
 *
 * @return an integer corresponding to frequency band
 */
//------------------------------------------------------------------------------
Integer PhysicalMeasurement::FrequencyBand(Real frequency)
{
   Integer freqBand = 0;

   // S-band
   if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
	  freqBand = 1;					// 1 for S-band

   // X-band (Band values from Wikipedia; check them!
   if ((frequency >= 7900000000.0) && (frequency <= 8400000000.0))
      freqBand = 2;					// 2 for X-band

   if (freqBand == 0)
   {
	  std::stringstream strs;
	  strs << "Error in PhysicalMeasurement::FrequencyBand():  GMAT cannot specify frequency band for frequency = " << frequency <<" Hz\n";
	  throw MeasurementException(strs.str());
   }

   return freqBand;
}



//------------------------------------------------------------------------------
// Real RampedFrequencyIntergration(Real t0, Real t1)
//------------------------------------------------------------------------------
/**
 * Calculate the tetegration of ramped frequency in range from time t0 to time t1
 *
 * @param t1         The end time for integration (unit: A1Mjd)
 * @param delta_t    Elapse time (unit: second)
 *
 * @return The integration of ramped frequency.
 * Assumptions: ramp table had been sorted by epoch 
 */
//------------------------------------------------------------------------------
//#define DEBUG_INTEGRAL_RAMPED_FREQUENCY
Real PhysicalMeasurement::IntegralRampedFrequency(Real t1, Real delta_t)
{
#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("Enter PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

   if (delta_t < 0)
      throw MeasurementException("Error: Elapse time has to be a non negative number\n");

   if (rampTB == NULL)
	  throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   else if ((*rampTB).size() == 0)
	  throw MeasurementException("Error: No data in ramp table\n");
   

   Real time_min = (*rampTB)[0].epoch;
   Real time_max = (*rampTB)[(*rampTB).size() -1 ].epoch;
   if ((t1 < time_min)||(t1 > time_max))
   {
	  char s[200];
	  sprintf(&s[0], "Error: End epoch t3R = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t1, time_min, time_max);
	  std::string st(&s[0]);

	  throw MeasurementException(st);
   }

   Real t0 = t1 - delta_t/GmatTimeConstants::SECS_PER_DAY; 
   if ((t0 < time_min)||(t0 > time_max))
   {
	  char s[200];
	  sprintf(&s[0], "Error: Start epoch t1T = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t0, time_min, time_max);
	  std::string st(&s[0]);

	  throw MeasurementException(st);
   }

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage(" Start epoch t1T = %.12lf A1Mjd\n", t0);
   MessageInterface::ShowMessage(" End epoch t3R   = %.12lf A1Mjd\n", t1);
   MessageInterface::ShowMessage(" elapse time   = %.12lf s\n", delta_t);
#endif
   // search for end interval:
   UnsignedInt end_interval = 0;
   for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   {
      if (t1 < (*rampTB)[i].epoch)
	  {
         end_interval = i-1;      
		 break;
	  }
   }

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("\n End interval: i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n", end_interval, (*rampTB)[end_interval].epoch, (*rampTB)[end_interval].rampFrequency, (*rampTB)[end_interval].rampRate);
   MessageInterface::ShowMessage("               i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n\n", end_interval+1, (*rampTB)[end_interval+1].epoch, (*rampTB)[end_interval+1].rampFrequency, (*rampTB)[end_interval+1].rampRate);
#endif

   // search for end interval:
   Real f0, f1, f_dot;
   Real value1;
   Real interval_len;

   Real value = 0.0;
   Real dt = delta_t;
   Integer i = end_interval;
   while (dt > 0)
   {
      f_dot = (*rampTB)[i].rampRate;

	  // Specify lenght of the current interval
	  if (i == end_interval)
		 interval_len = (t1 - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;
	  else
		 interval_len = ((*rampTB)[i+1].epoch - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;

      f0 = (*rampTB)[i].rampFrequency;
      if (dt < interval_len)
      {
         f0 = f0 + f_dot*(interval_len - dt);
         interval_len = dt;
      }
      // Specify frequency at the end of the current interval
	  f1 = f0 + f_dot*interval_len;

	  // Take integral for the current interval
	  value1 = (f0 + f1) * interval_len / 2;
	  value  = value + value1;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
	  MessageInterface::ShowMessage("interval i = %d:    value1 = %.12lf    f0 = %.12lf   f1 = %.12lf     f_ave = %.12lfHz   width = %.12lfs \n", i, value1, f0, f1, (f0+f1)/2, interval_len);
      MessageInterface::ShowMessage("interval i = %d: epoch = %.12lf     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n", i,
	   (*rampTB)[i].epoch,  (*rampTB)[i].uplinkBand, (*rampTB)[i].rampType, (*rampTB)[i].rampFrequency, (*rampTB)[i].rampRate);
#endif

      // Specify dt 
 	  dt = dt - interval_len;

	  i--;
   }

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("value = %.12lf\n", value);
   MessageInterface::ShowMessage("Exit PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

   return value;

}

