//$Header$
//------------------------------------------------------------------------------
//                             LinearInterpolator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * A basic linear interpolator.
 */
//------------------------------------------------------------------------------




// Class automatically generated by Dev-C++ New Class wizard

#ifndef LINEARINTERPOLATOR_HPP
#define LINEARINTERPOLATOR_HPP

#include "Interpolator.hpp" // inheriting class's header file

/**
 * A linear interpolator for quick calculation of interpolated data
 */
class LinearInterpolator : public Interpolator
{
public:
    LinearInterpolator(const std::string &name = "", Integer dim = 1); //loj: 3/22/04 added name
    ~LinearInterpolator(void);
    LinearInterpolator(const LinearInterpolator &l);
    LinearInterpolator& operator=(const LinearInterpolator &li);

    virtual bool Interpolate(const Real ind, Real *results);
};


#endif // LINEARINTERPOLATOR_HPP

