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
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"


// define default names form solar system bodies
const std::string SolarSystem::SUN_NAME       = "Sun";
const std::string SolarSystem::MERCURY_NAME   = "Mercury";
const std::string SolarSystem::VENUS_NAME     = "Venus";
const std::string SolarSystem::EARTH_NAME     = "Earth";
const std::string SolarSystem::MOON_NAME      = "Luna";
const std::string SolarSystem::MARS_NAME      = "Mars";
const std::string SolarSystem::JUPITER_NAME   = "Jupiter";
const std::string SolarSystem::SATURN_NAME    = "Saturn";
const std::string SolarSystem::URANUS_NAME    = "Uranus";
const std::string SolarSystem::NEPTUNE_NAME   = "Neptune";
const std::string SolarSystem::PLUTO_NAME     = "Pluto";
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
SolarSystem::SolarSystem(std::string withName) :
GmatBase(Gmat::SOLAR_SYSTEM,"",withName),  // what is the typeStr for this?
bodiesInUseID  (parameterCount)
{
   parameterCount  += 1;
   pvSrcForAll      = Gmat::SLP;
   anMethodForAll   = Gmat::TWO_BODY;
   pE               = NULL;
   
   // create and add the default bodies
   Star* theSun     = new Star(SUN_NAME);      // need to set default value for these too
   Planet* theEarth = new Planet(EARTH_NAME, theSun);
   Moon* theMoon    = new Moon(MOON_NAME, theEarth);

   AddBody(theSun);
   AddBody(theEarth);
   AddBody(theMoon);
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
pE              (ss.pE),
bodiesInUse     (ss.bodiesInUse),
bodyStrings     (ss.bodyStrings)
{
   // other stuff?
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
   pE             = ss.pE;
   bodiesInUse    = ss.bodiesInUse;
   bodyStrings    = ss.bodyStrings;
   parameterCount = ss.parameterCount;
   return *this;
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
   //bodiesInUse.~list<CelestialBody*>();
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
   return true;
}

//------------------------------------------------------------------------------
//  StringArray GetBodiesInUse() const
//------------------------------------------------------------------------------
/**
 * This method returns a list of strings representing the bodies in use.
 *
 * @return list of strings representing the bodies in use.
 *
 */
//------------------------------------------------------------------------------
StringArray SolarSystem::GetBodiesInUse() const
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
   SolarSystem* theClone = new SolarSystem(*this);
   return theClone;   // huh??????????????????????????????
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter((const Integer id) const
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
   if (id == bodiesInUseID)
   {
      return bodyStrings;
   }

   return GmatBase::GetStringArrayParameter(id);
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


