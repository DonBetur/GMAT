//$Header$
//------------------------------------------------------------------------------
//                                  Toggle
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Insert descriptive text here.
 *
 * @note Any notes here.
 */
//------------------------------------------------------------------------------




// Class automatically generated by Dev-C++ New Class wizard

#ifndef TOGGLE_HPP
#define TOGGLE_HPP

#include "Command.hpp" // inheriting class's header file
#include <list>


/**
 * Command used to turn subscribers on and off
 */
class Toggle : public GmatCommand
{
    public:
        Toggle();
        virtual ~Toggle();

        Toggle(const Toggle& t);
		Toggle&               operator=(const Toggle& t);
		
      // inherited from GmatBase
      virtual GmatBase* Clone(void) const;

      // Access methods derived classes can override
        virtual std::string GetParameterText(const Integer id) const;
        virtual Integer     GetParameterID(const std::string &str) const;
        virtual Gmat::ParameterType
                            GetParameterType(const Integer id) const;
        virtual std::string GetParameterTypeString(const Integer id) const;

        virtual std::string GetStringParameter(const Integer id) const;
        virtual bool        SetStringParameter(const Integer id,
                                               const std::string &value);

        // Methods used to run the command
        virtual bool        InterpretAction(void);
        virtual bool        Execute(void);
        
    protected:
        /// State for the toggle
        bool                toggleState;
        /// Subscriber list for the toggle
        StringArray         subNames;
        /// Corresponding subscribers
        std::list<Subscriber *>  subs;
        /// Parameter ID for the subscriber names
        const Integer       subscriberID;
};


#endif // TOGGLE_HPP

