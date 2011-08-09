//$Id$
//------------------------------------------------------------------------------
//                            AtmosphereFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/08/12
//
/**
 *  Implementation code for the AtmosphereFactory class, responsible for
 *  creating AtmosphereModel objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AtmosphereFactory.hpp"
#include "AtmosphereModel.hpp"
#include "ExponentialAtmosphere.hpp"
#include "Msise90Atmosphere.hpp"
#include "NRLMsise00Atmosphere.hpp"
#include "JacchiaRobertsAtmosphere.hpp"
#include "SimpleExponentialAtmosphere.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateAtmosphere(std::string ofType, std::string withName, std::string forBody)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Atmosphere class
 *
 * @param <ofType> the Atmosphere object to create and return.
 * @param <forBody> the body for which the atmospher model is requested.
 * @param <withName> the name to give the newly-created Atmosphere object.
 *
 * @note As of 2004/08/12, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 * @note As of 2004/08/30, we are also ignoring the forBody parameter, as none
 *       of the constructors currently have this name as an input (though it may
 *       be useful in the future.
 */
//------------------------------------------------------------------------------
AtmosphereModel*
AtmosphereFactory::CreateAtmosphereModel(const std::string &ofType,
                                         const std::string &withName,
                                         const std::string &forBody)
{
   if (ofType == "Exponential")
      return new ExponentialAtmosphere(withName);
   if (ofType == "Simple")
      return new SimpleExponentialAtmosphere(withName);
   else if (ofType == "MSISE90")
      return new Msise90Atmosphere(withName);
//    else if (ofType == "MSISE86")
//       return new Msise86Atmosphere(withName);
   else if (ofType == "NRLMSISE00")
      return new NRLMsise00Atmosphere(withName);
   else if (ofType == "JacchiaRoberts")
      return new JacchiaRobertsAtmosphere(withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  AtmosphereFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AtmosphereFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
AtmosphereFactory::AtmosphereFactory() :
Factory(Gmat::ATMOSPHERE)
{
   if (creatables.empty())
   {
      creatables.push_back("Exponential");
      creatables.push_back("Simple");
      creatables.push_back("MSISE90");
      //creatables.push_back("MSISE86");
      creatables.push_back("NRLMSISE00");
      creatables.push_back("JacchiaRoberts");
   }
}

//------------------------------------------------------------------------------
//  AtmosphereFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AtmosphereFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
AtmosphereFactory::AtmosphereFactory(StringArray createList) :
Factory(createList,Gmat::ATMOSPHERE)
{
}

//------------------------------------------------------------------------------
//  AtmosphereFactory(const AtmosphereFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AtmosphereFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
AtmosphereFactory::AtmosphereFactory(const AtmosphereFactory& fact) :
Factory(fact)
{
}

//------------------------------------------------------------------------------
//  AtmosphereFactory& operator= (const AtmosphereFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AtmosphereFactory class.
 *
 * @param <fact> the AtmosphereFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AtmosphereFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AtmosphereFactory& AtmosphereFactory::operator= (
                                             const AtmosphereFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~AtmosphereFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the AtmosphereFactory base class.
 *
 */
//------------------------------------------------------------------------------
AtmosphereFactory::~AtmosphereFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





