//$Header$
//------------------------------------------------------------------------------
//                                 Propagate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Definition for the Propagate command class
 */
//------------------------------------------------------------------------------


#ifndef PROPAGATE_HPP
#define PROPAGATE_HPP

#include "Command.hpp"
#include <list>


// Headers for the referenced classes
#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "StopCondition.hpp"


class GMAT_API Propagate : public Command
{
public:
    Propagate(void);
    virtual ~Propagate(void);
    Propagate(const Propagate &p);
    Propagate&              operator=(const Propagate &p);

    // Methods used for configuration
    virtual bool            SetObject(const std::string &name,
                                      const Gmat::ObjectType type,
                                      const std::string &associate = "",
                                      const Gmat::ObjectType associateType =
                                                          Gmat::UNKNOWN_OBJECT);

    virtual bool            SetObject(GmatBase *obj,
                                      const Gmat::ObjectType type);

    // Parameter accessor methods -- overridden from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    
    virtual Integer     GetIntegerParameter(const Integer id) const;
    virtual Integer     SetIntegerParameter(const Integer id,
                                            const Integer value);
    virtual bool        GetBooleanParameter(const Integer id) const;
    virtual bool        SetBooleanParameter(const Integer id,
                                            const bool value);

    // Methods used to run the command
    virtual void            InterpretAction(void);
    
    virtual bool            Initialize(void);
    virtual bool            Execute(void);

protected:
    // We may eventually want to make this a list of propagator names for the
    // propagators driven by this command, like this:
    //    /// List of the names of propagator setups used in this command
    //    std::vector<std::string>  propName;

    /// Name of the propagator setup used in this command
    std::string             propName;
    /// The (1 or more) spacecraft associated with this propagation
    StringArray             satName;
    /// Flag used to determine if the spacecraft are propagated coupled
    bool                    propCoupled;
    /// Frequency used to check for user interrupts of the run
    Integer                 interruptCheckFrequency;
    /// Starting epoch for the propagation
    Real                    baseEpoch;

    // We may eventually want to make this a list of propagators all driven by
    // this command, like this:
    //    /// List of the propagator setups used in this command
    //    std::vector<Propagator>  prop;
    
    /// The propagator used by this command
    PropSetup               *prop;      // Likely to change when the propagator
                                        // infrastructure is coded -- might
                                        // just be a Propagator *
    /// The spacecraft that are propagated
    std::vector<Spacecraft*>
                            sats;
    /// The stopping conditions
    std::vector<StopCondition *>
                            stopWhen;

    // For convenience, set variables for the parameter IDs
    /// ID for the boolean flag used for the prop mode
    const Integer           propCoupledID;
    /// ID for the number of iterations before calling to check for interrupts
    const Integer           interruptCheckFrequencyID;
    
    /// Temporary parameter used to stop on time
    Real                    secondsToProp;
    
private:
    
};


#endif // PROPAGATE_HPP
