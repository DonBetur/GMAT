//$Header$
//------------------------------------------------------------------------------
//                              RungeKutta89
//------------------------------------------------------------------------------
// *** File Name : RungeKutta89.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
//                           : 10/21/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
// **************************************************************************

#ifndef RungeKutta89_hpp
#define RungeKutta89_hpp

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

class GMAT_API RungeKutta89 : public RungeKutta
{
public:
//	RungeKutta89(void);
    RungeKutta89(const std::string &nomme = "");
    virtual ~RungeKutta89(void);

    RungeKutta89(const RungeKutta89&);
    RungeKutta89 & operator=(const RungeKutta89&);

    virtual char* GetType(void) const;
    virtual Propagator* Clone(void) const;
protected:
    void SetCoefficients(void);
};

#endif // RungeKutta89_hpp


