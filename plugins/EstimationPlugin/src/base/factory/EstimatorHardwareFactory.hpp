//$Id: EstimatorHardwareFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             EstimatorHardwareFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the hardware factory class used by the estimation
 * subsystem.
 */
//------------------------------------------------------------------------------


#ifndef EstimatorHardwareFactory_hpp
#define EstimatorHardwareFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"


/**
 * The local hardware factory used to create hardware specific to estimation.
 */
class ESTIMATION_API EstimatorHardwareFactory : public Factory
{
public:
   EstimatorHardwareFactory();
   virtual ~EstimatorHardwareFactory();
   EstimatorHardwareFactory(StringArray createList);
   EstimatorHardwareFactory(const EstimatorHardwareFactory& fact);
   EstimatorHardwareFactory& operator= (const EstimatorHardwareFactory& fact);

   virtual Hardware* CreateHardware(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* EstimatorHardwareFactory_hpp */
