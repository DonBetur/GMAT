//$Header$
//------------------------------------------------------------------------------
//                                  SolarSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
//
/**
 * This is the implementation of the SolarSystem class.
 *
 * @note It is assumed that no classes will be derived from this one.
 */
//------------------------------------------------------------------------------

// Class automatically generated by Dev-C++ New Class wizard

#include <list>
#include <string>
#include "SolarSystem.hpp" // class's header file
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
SolarSystem::PARAMETER_TEXT[SolarSystemParamCount - GmatBaseParamCount] =
{
   "BodiesInUse",
   "NumberOfBodies",
   "UseTTForEphemeris",
   "EphemerisUpdateInterval",
};

const Gmat::ParameterType
SolarSystem::PARAMETER_TYPE[SolarSystemParamCount - GmatBaseParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::REAL_TYPE,
};


// define default names form solar system bodies
const std::string SolarSystem::SUN_NAME        = "Sun";

const std::string SolarSystem::MERCURY_NAME    = "Mercury";

const std::string SolarSystem::VENUS_NAME      = "Venus";

const std::string SolarSystem::EARTH_NAME      = "Earth";
const std::string SolarSystem::MOON_NAME       = "Luna";

const std::string SolarSystem::MARS_NAME       = "Mars";
const std::string SolarSystem::PHOBOS_NAME     = "Phobos";
const std::string SolarSystem::DEIMOS_NAME     = "Deimos";

const std::string SolarSystem::JUPITER_NAME    = "Jupiter";
const std::string SolarSystem::METIS_NAME      = "Metis";
const std::string SolarSystem::ADRASTEA_NAME   = "Adrastea";
const std::string SolarSystem::AMALTHEA_NAME   = "Amalthea";
const std::string SolarSystem::THEBE_NAME      = "Thebe";
const std::string SolarSystem::IO_NAME         = "Io";
const std::string SolarSystem::EUROPA_NAME     = "Europa";
const std::string SolarSystem::GANYMEDE_NAME   = "Ganymede";
const std::string SolarSystem::CALLISTO_NAME   = "Callisto";

const std::string SolarSystem::SATURN_NAME     = "Saturn";
const std::string SolarSystem::PAN_NAME        = "Pan";
const std::string SolarSystem::ATLAS_NAME      = "Atlas";
const std::string SolarSystem::PROMETHEUS_NAME = "Promethus";
const std::string SolarSystem::PANDORA_NAME    = "Pandora";
const std::string SolarSystem::EPIMETHEUS_NAME = "Epimetheus";
const std::string SolarSystem::JANUS_NAME      = "Janus";
const std::string SolarSystem::MIMAS_NAME      = "Mimas";
const std::string SolarSystem::ENCELADUS_NAME  = "Enceladus";
const std::string SolarSystem::TETHYS_NAME     = "Tethys";
const std::string SolarSystem::TELESTO_NAME    = "Telesto";
const std::string SolarSystem::CALYPSO_NAME    = "Calypso";
const std::string SolarSystem::DIONE_NAME      = "Dione";
const std::string SolarSystem::HELENE_NAME     = "Helene";
const std::string SolarSystem::RHEA_NAME       = "Rhea";
const std::string SolarSystem::TITAN_NAME      = "Titan";
const std::string SolarSystem::IAPETUS_NAME    = "Iapetus";
const std::string SolarSystem::PHOEBE_NAME     = "Phoebe";

const std::string SolarSystem::URANUS_NAME     = "Uranus";
const std::string SolarSystem::CORDELIA_NAME   = "Cordelia";
const std::string SolarSystem::OPHELIA_NAME    = "Ophelia";
const std::string SolarSystem::BIANCA_NAME     = "Bianca";
const std::string SolarSystem::CRESSIDA_NAME   = "Cressida";
const std::string SolarSystem::DESDEMONA_NAME  = "Desdemona";
const std::string SolarSystem::JULIET_NAME     = "Juliet";
const std::string SolarSystem::PORTIA_NAME     = "Portia";
const std::string SolarSystem::ROSALIND_NAME   = "Rosalind";
const std::string SolarSystem::BELINDA_NAME    = "Belinda";
const std::string SolarSystem::PUCK_NAME       = "Puck";
const std::string SolarSystem::MIRANDA_NAME    = "Miranda";
const std::string SolarSystem::ARIEL_NAME      = "Ariel";
const std::string SolarSystem::UMBRIEL_NAME    = "Umbriel";
const std::string SolarSystem::TITANIA_NAME    = "Titania";
const std::string SolarSystem::OBERON_NAME     = "Oberon";

const std::string SolarSystem::NEPTUNE_NAME    = "Neptune";
const std::string SolarSystem::NAIAD_NAME      = "Naiad";
const std::string SolarSystem::THALASSA_NAME   = "Thalassa";
const std::string SolarSystem::DESPINA_NAME    = "Despina";
const std::string SolarSystem::GALATEA_NAME    = "Galatea";
const std::string SolarSystem::LARISSA_NAME    = "Larissa";
const std::string SolarSystem::PROTEUS_NAME    = "Proteus";
const std::string SolarSystem::TRITON_NAME     = "Triton";

const std::string SolarSystem::PLUTO_NAME      = "Pluto";
const std::string SolarSystem::CHARON_NAME     = "Charon";

// add other moons, asteroids, comets, as needed
// what about libration points?

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SolarSystem(std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SolarSystem class (default constructor).
 *
 * @param <withName> optional parameter indicating the name of the solar
 *                   system (default is "").
 */
//------------------------------------------------------------------------------
SolarSystem::SolarSystem(std::string withName)
   : GmatBase(Gmat::SOLAR_SYSTEM, "SolarSystem", withName)
{
   objectTypes.push_back(Gmat::SOLAR_SYSTEM);
   objectTypeNames.push_back("SolarSystem");
   parameterCount   = SolarSystemParamCount;
   pvSrcForAll      = Gmat::DE_405;
   anMethodForAll   = Gmat::LOW_FIDELITY;
   pE               = NULL;
   overrideTimeForAll  = false;
   ephemUpdateInterval = 0.0;
   
   // create and add the default bodies
   Star* theSun     = new Star(SUN_NAME);
   theSun->SetCentralBody(EARTH_NAME);  // central body here is a reference body
   Planet* theEarth = new Planet(EARTH_NAME,   SUN_NAME);
   Moon* theMoon    = new Moon(MOON_NAME,      EARTH_NAME);
   Planet* mercury  = new Planet(MERCURY_NAME, SUN_NAME);
   Planet* venus    = new Planet(VENUS_NAME,   SUN_NAME);
   Planet* mars     = new Planet(MARS_NAME,    SUN_NAME);
   Planet* jupiter  = new Planet(JUPITER_NAME, SUN_NAME);
   Planet* saturn   = new Planet(SATURN_NAME,  SUN_NAME);
   Planet* uranus   = new Planet(URANUS_NAME,  SUN_NAME);
   Planet* neptune  = new Planet(NEPTUNE_NAME, SUN_NAME);
   Planet* pluto    = new Planet(PLUTO_NAME,   SUN_NAME);

   AddBody(theSun);
   AddBody(theEarth);
   AddBody(theMoon);
   AddBody(mercury);
   AddBody(venus);
   AddBody(mars);
   AddBody(jupiter);
   AddBody(saturn);
   AddBody(uranus);
   AddBody(neptune);
   AddBody(pluto);
   
   // at least for now, give each body a pointer to its central body
   theSun->SetRefObject(theEarth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME); // for reference!!!
   theEarth->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   theMoon->SetRefObject(theEarth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME);
   mercury->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   venus->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   mars->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   jupiter->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   saturn->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   uranus->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   neptune->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   pluto->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
}

//------------------------------------------------------------------------------
//  SolarSystem(const SolarSystem &ss)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SolarSystem class as a copy of the
 * specified SolarSystem class (copy constructor).
 *
 * @param <ss> SolarSystem object to copy.
 */
//------------------------------------------------------------------------------
SolarSystem::SolarSystem(const SolarSystem &ss) :
GmatBase        (ss),
pvSrcForAll     (ss.pvSrcForAll),
anMethodForAll  (ss.anMethodForAll),
pE              (NULL),
overrideTimeForAll (ss.overrideTimeForAll),
ephemUpdateInterval (ss.ephemUpdateInterval),
bodiesInUse     (ss.bodiesInUse), // copy it first
bodyStrings     (ss.bodyStrings)
{
   parameterCount   = SolarSystemParamCount;
   // replace body pointers with clones
   Integer sz = bodiesInUse.size();
   Integer i;
   for (i = 0; i < sz; i++)
   {
      bodiesInUse.push_back(((CelestialBody*)
                           ((bodiesInUse.front())->Clone())));
      bodiesInUse.pop_front();
   }
}

//------------------------------------------------------------------------------
//  SolarSystem& operator= (const SolarSystem& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SolarSystem class.
 *
 * @param ss> the solar system object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" solarsystem with data of input solarsystem ss.
 */
//------------------------------------------------------------------------------
SolarSystem& SolarSystem::operator=(const SolarSystem &ss)
{
   GmatBase::operator=(ss);
   pvSrcForAll    = ss.pvSrcForAll;
   anMethodForAll = ss.anMethodForAll;
   pE             = NULL;
   overrideTimeForAll      = ss.overrideTimeForAll;
   ephemUpdateInterval     = ss.ephemUpdateInterval;
   bodiesInUse.clear();
   bodyStrings.clear();
   bodiesInUse    = ss.bodiesInUse;
   bodyStrings    = ss.bodyStrings;
   // replace body pointers with clones
   Integer sz = bodiesInUse.size();
   Integer i;
   for (i = 0; i < sz; i++)
   {
      bodiesInUse.push_back(((CelestialBody*)
                           ((bodiesInUse.front())->Clone())));
      bodiesInUse.pop_front();
   }
   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool SolarSystem::Initialize()
{
   // Initialize bodies in use
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      (*cbi)->Initialize();      
      ++cbi;
   }

   return true;
}


//------------------------------------------------------------------------------
// void ResetToDefaults()
//------------------------------------------------------------------------------
void SolarSystem::ResetToDefaults()
{
   pvSrcForAll      = Gmat::DE_405;
   anMethodForAll   = Gmat::LOW_FIDELITY;
   pE               = NULL;
   overrideTimeForAll  = false;
   ephemUpdateInterval = 0.0;
   
   // Set it for each of the bodies
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      (*cbi)->SetSource(pvSrcForAll);
      (*cbi)->SetAnalyticMethod(anMethodForAll);
      (*cbi)->SetOverrideTimeSystem(overrideTimeForAll);
      (*cbi)->SetEphemUpdateInterval(ephemUpdateInterval);
      
      ++cbi;
   }
}


//------------------------------------------------------------------------------
//  ~SolarSystem()
//------------------------------------------------------------------------------
/**
 * Destructor for the SolarSystem class.
 */
//------------------------------------------------------------------------------
SolarSystem::~SolarSystem()
{
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      delete (*cbi);       // delete each body first
      ++cbi;
   }
   delete pE;
}


//------------------------------------------------------------------------------
//  bool AddBody(CelestialBody* cb)
//------------------------------------------------------------------------------
/**
 * This method adds to the list of bodies in use for this solar system.
 *
 * @param <cb>  body to add to the list for this solar system.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::AddBody(CelestialBody* cb)
{
   if (cb == NULL)
   {
      return false;    // write some kind of error or throw an exception?
   }

   bodiesInUse.push_back(cb);
   bodyStrings.push_back(cb->GetName());

   // Set the source, source file, analytic method, and override flag for the 
   // new body
   if (!cb->SetSource(pvSrcForAll))  return false; 
   if (!cb->SetAnalyticMethod(anMethodForAll))  return false; 
   if (pE)
      if (!cb->SetSourceFile(pE))  return false; 
   if (!cb->SetOverrideTimeSystem(overrideTimeForAll))  return false; 
   
   return true;
}

//------------------------------------------------------------------------------
//  CelestialBody* GetBody(std::string withName)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the requested celestial body.
 *
 * @param <withName>  name of the requested body.
 *
 * @return a pointer to the requested body.
 *
 */
//------------------------------------------------------------------------------
CelestialBody* SolarSystem::GetBody(std::string withName)
{
   return FindBody(withName);
}

//------------------------------------------------------------------------------
//  bool IsBodyInUse(std::string theBody)
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether the specified celestial body
 * is in use.
 *
 * @param <theBody>  name of the body.
 *
 * @return a flag indicating whether the specified body is in use.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::IsBodyInUse(std::string theBody)
{
   // Search through bodiesInUse for the body with the name withName
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->GetName() == theBody)
      {
         return true;
      }
      ++cbi;
   }
   return false;
}

//------------------------------------------------------------------------------
//  Gmat::PosVelSource GetPosVelSource() const
//------------------------------------------------------------------------------
/**
 * This method returns the source of position and velocity for the bodies in
 * use (assuming all have the same source).
 *
 * @return position/velocity source for the bodies.
 *
 */
//------------------------------------------------------------------------------
Gmat::PosVelSource   SolarSystem::GetPosVelSource() const
{
   return pvSrcForAll;
}

//------------------------------------------------------------------------------
//  Gmat::AnalyticMethod GetAnalyticMethod() const
//------------------------------------------------------------------------------
/**
 * This method returns the analytic method for the bodies in
 * use (assuming all have the same method), when the source is ANALYTIC_METHOD.
 *
 * @return analytic method for the bodies.
 *
 */
//------------------------------------------------------------------------------
Gmat::AnalyticMethod SolarSystem::GetAnalyticMethod() const
{
   return anMethodForAll;
}

//------------------------------------------------------------------------------
//  std::string GetSourceFileName() const
//------------------------------------------------------------------------------
/**
 * This method returns the source file name for the bodies in
 * use (assuming all have the same source).
 *
 * @return source file name for the bodies.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetSourceFileName() const
{
   if (pE == NULL) return "";
   return pE->GetName();
}

bool SolarSystem::GetOverrideTimeSystem() const
{
   return overrideTimeForAll;
}

Real SolarSystem::GetEphemUpdateInterval() const
{
   return ephemUpdateInterval;
}
 

StringArray SolarSystem::GetValidModelList(Gmat::ModelType m, 
                         const std::string &forBody)
{
   for (std::list<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->GetValidModelList(m);
   }
   throw SolarSystemException("Model list requested for unknown body " 
                               + forBody);
}



//------------------------------------------------------------------------------
//  bool SetSource(Gmat::PosVelSource pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the source for the bodies in
 * use (assuming all have the same source).
 *
 * @param <pvSrc> source (for pos and vel) for all of the bodies.
 *
* @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSource(Gmat::PosVelSource pvSrc)
{
   // Search through bodiesInUse for the body with the name withName
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSource(pvSrc) == false)  return false;
      ++cbi;
   }
   pvSrcForAll = pvSrc;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSource(const std::string &pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the source for the bodies in
 * use (assuming all have the same source).
 *
 * @param <pvSrc> source (for pos and vel) for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSource(const std::string &pvSrc)
{
   Gmat::PosVelSource theSrc = Gmat::PosVelSourceCount;
   for (Integer i = 0; i < Gmat::PosVelSourceCount; i++)
   {
      if (pvSrc == CelestialBody::POS_VEL_STRINGS[i]) 
         theSrc = (Gmat::PosVelSource) i; 
   }
   if (theSrc == Gmat::PosVelSourceCount)
      throw SolarSystemException("Unknown ephemeris source " + pvSrc);
   // Search through bodiesInUse and set the source for all
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSource(theSrc) == false)  return false;
      ++cbi;
   }
   pvSrcForAll = theSrc;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSourceFile(PlanetaryEphem *src)
//------------------------------------------------------------------------------
/**
 * This method sets the source file for the bodies in
 * use (assuming all have the same method).
 *
 * @param <src> planetary ephem - source for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSourceFile(PlanetaryEphem *src)
{
   // Search through bodiesInUse for the body with the name withName
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSourceFile(src) == false)  return false;
      ++cbi;
   }
   pE = src;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetAnalyticMethod(Gmat::AnalyticMethod aM)
//------------------------------------------------------------------------------
/**
* This method sets the analytic method for the bodies in
 * use (assuming all have the same method).
 *
 * @param aMc> analytic method selection for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetAnalyticMethod(Gmat::AnalyticMethod aM)
{
   // Search through bodiesInUse for the body with the name withName
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetAnalyticMethod(aM) == false)  return false;
      ++cbi;
   }
   anMethodForAll = aM;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetAnalyticMethod(const std::string &aM)
//------------------------------------------------------------------------------
/**
 * This method sets the analytic method for the bodies in
 * use (assuming all have the same method).
 *
 * @param aMc> analytic method selection for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetAnalyticMethod(const std::string &aM)
{
   Gmat::AnalyticMethod theMethod = Gmat::AnalyticMethodCount;
   for (Integer i = 0; i < Gmat::AnalyticMethodCount; i++)
   {
      if (aM == CelestialBody::ANALYTIC_METHOD_STRINGS[i]) 
         theMethod = (Gmat::AnalyticMethod) i; 
   }
   if (theMethod == Gmat::AnalyticMethodCount)
      throw SolarSystemException("Unknown analytic method " + aM);
   // Search through bodiesInUse and set it for all
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetAnalyticMethod(theMethod) == false)  return false;
      ++cbi;
   }
   anMethodForAll = theMethod;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetOverrideTimeSystem(bool overrideIt)
//------------------------------------------------------------------------------
/**
 * This method sets the override time system flag.
 *
 * @param <overrideIt> override time system selection for all of the bodies?.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetOverrideTimeSystem(bool overrideIt)
{
   // Set it for each of the bodies
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetOverrideTimeSystem(overrideIt) == false)  return false;
      ++cbi;
   }
   overrideTimeForAll = overrideIt;
   return true;
}

bool SolarSystem::SetEphemUpdateInterval(Real intvl)
{
   if (intvl < 0.0)
   throw SolarSystemException("SolarSystem - ephem update interval must be > 0.0");
   // Set it for each of the bodies
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetEphemUpdateInterval(intvl) == false)  return false;
      ++cbi;
   }
   ephemUpdateInterval = intvl;
   return true;
}


bool SolarSystem::AddValidModelName(Gmat::ModelType m, 
                  const std::string &forBody,
                  const std::string &theModel)
{
   for (std::list<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->AddValidModelName(m, theModel);
   }
   throw SolarSystemException("Cannot set new model for unknown body " 
                              + forBody);
}

bool SolarSystem::RemoveValidModelName(Gmat::ModelType m, 
                  const std::string & forBody,
                  const std::string &theModel)
{
   for (std::list<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->RemoveValidModelName(m, theModel);
   }
   throw SolarSystemException("Cannot remove model for unknown body " 
                              + forBody);
}

//------------------------------------------------------------------------------
//  const StringArray& GetBodiesInUse() const
//------------------------------------------------------------------------------
/**
 * This method returns a list of strings representing the bodies in use.
 *
 * @return list of strings representing the bodies in use.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetBodiesInUse() const
{
   return bodyStrings;
}

//------------------------------------------------------------------------------
//  SolarSystem* Clone(void) const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the SolarSystem.
 *
 * @return a pointer to a clone of the SolarSystem.
 *
 */
//------------------------------------------------------------------------------
SolarSystem* SolarSystem::Clone(void) const
{
   // clone all objects in the Solar System as well
   
   return new SolarSystem(*this);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SolarSystemParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SolarSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SolarSystem::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SolarSystemParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetParameterTypeString(const Integer id) const
{
   return SolarSystem::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)          return bodiesInUse.size();
   
   return GmatBase::GetIntegerParameter(id); 
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method returns the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

Real SolarSystem::GetRealParameter(const Integer id) const
{
   if (id == EPHEM_UPDATE_INTERVAL) return ephemUpdateInterval;
   return GmatBase::GetRealParameter(id);
}


Real SolarSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real SolarSystem::SetRealParameter(const Integer id,
                                   const Real value)
{
   if (id == EPHEM_UPDATE_INTERVAL)
   {
      SetEphemUpdateInterval(value);
      return true;
   }
   return GmatBase::SetRealParameter(id, value);
}
                                   
Real SolarSystem::SetRealParameter(const std::string &label,
                                  const Real value)
{
   return SetRealParameter(GetParameterID(label),value);
}


bool SolarSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_TIME_SYSTEM) return overrideTimeForAll;
   return GmatBase::GetBooleanParameter(id); 
}

bool SolarSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

bool SolarSystem::SetBooleanParameter(const Integer id,
                                      const bool value)
{
   if (id == OVERRIDE_TIME_SYSTEM)
   {
      SetOverrideTimeSystem(value);
      return true;
   }
   return GmatBase::SetBooleanParameter(id, value);
}

bool SolarSystem::SetBooleanParameter(const std::string &label,
                                      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetStringArrayParameter(const Integer id) const
{
   if (id == BODIES_IN_USE) return bodyStrings;

   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
SolarSystem::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CelestialBody* FindBody(std::string withName)
//------------------------------------------------------------------------------
/**
* Return a pointer to a CelestialBody with the name withName.
 *
 * @param <withName>  name of body requested.
 *
 * @return pointer to a CelestialBody wiith the requested name.
 */
//------------------------------------------------------------------------------
CelestialBody* SolarSystem::FindBody(std::string withName) 
{
   // Search through bodiesInUse for the body with the name withName
   std::list<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->GetName() == withName)
      {
         return (*cbi);
      }
      ++cbi;
   }
   return NULL;
}


