//$Header$ 
//------------------------------------------------------------------------------
//                              StateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/03/22
//
/**
 * Implements the StateConverter class
 */
//------------------------------------------------------------------------------

#include "StateConverter.hpp"

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  StateConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
StateConverter::StateConverter()
// :   Converter("Cartesian") 
{
   type = "Cartesian";
   mu = DEFAULT_MU;
}

//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
StateConverter::StateConverter(const std::string &newType) 
// :   Converter(type) 
{
   type = newType;
   mu = DEFAULT_MU;
}

//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype, const Real newMu)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
StateConverter::StateConverter(const std::string &newType, const Real newMu) 
{
   type = newType;
   mu = newMu;
}

//---------------------------------------------------------------------------
//  StateConverter(const StateConverter &stateConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base StateConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
StateConverter::StateConverter(const StateConverter &stateConverter) 
//    : Converter (stateConverter.type)
{
   type = stateConverter.type;
   mu   = stateConverter.mu; 
}

//---------------------------------------------------------------------------
//  ~StateConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
StateConverter::~StateConverter()
{
}

//---------------------------------------------------------------------------
//  StateConverter& operator=(const StateConverter &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
StateConverter& StateConverter::operator=(const StateConverter &converter)
{
    // Don't do anything if copying self
    if (&converter == this)
        return *this;

    // Will fix it later
    return *this;
}

//---------------------------------------------------------------------------
//  Real StateConverter::GetMu() const
//---------------------------------------------------------------------------
/**
 * Get the mu 
 *
 * @return mu's value
 */
Real StateConverter::GetMu() const
{
   return mu;
}

//---------------------------------------------------------------------------
//  bool StateConverter::SetMu(const SolarSystem *solarSystem, 
//                             const std::string &body)
//---------------------------------------------------------------------------
/**
 * Set the mu from the Celestial body's gravitational constant
 *
 * @return true if successful; otherwise, return false
 *
 */
bool StateConverter::SetMu(SolarSystem *solarSystem, 
                           const std::string &body)
{
   if (solarSystem == NULL) return false;

   CelestialBody *centralBody = solarSystem->GetBody(body);
   if (centralBody == NULL) return false;

   // Get the gravitational constant and set new value for mu
   mu = centralBody->GetGravitationalConstant();

   return true;
}


//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(const Real *state,   
//                                   const std::string &fromElementType,
//                                   const std::string &toElementType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <toElementType> Element Type
 *
 * @return Converted states from the specific element type 
 */
Rvector6 StateConverter::Convert(const Real *state,
                                 const std::string &toElementType)
{
   return Convert(state,type,toElementType);
}

//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(const Real *state,   
//                                   const std::string &fromElementType,
//                                   const std::string &toElementType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <fromElementType>  Element Type 
 * @param <toElementType> Element Type
 *
 * @return Converted states from the specific element type 
 */
Rvector6 StateConverter::Convert(const Real *state,   
                                 const std::string &fromElementType,
                                 const std::string &toElementType)
{
    Rvector6 newState;
    newState.Set(state[0],state[1],state[2],state[3],state[4],state[5]); 

    // Determine the input of coordinate representation 
    if (fromElementType == "Cartesian" && toElementType != "Cartesian")
    {
       if (toElementType == "Keplerian")
       {
          Real meanAnomaly;   // why use MeanAnomaly?
          return(CartesianToKeplerian(newState,mu,&meanAnomaly));
       } 
    }
    else if (fromElementType == "Keplerian" && toElementType != "Keplerian")
    {
       if (toElementType == "Cartesian")
          return(KeplerianToCartesian(newState,mu,CoordUtil::TA));
    }
 
    return newState;
}
