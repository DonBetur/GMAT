//$Header$
//------------------------------------------------------------------------------
//                                  Interpreter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class definition for the Interpreter base class
 */
//------------------------------------------------------------------------------


// Class automatically generated by Dev-C++ New Class wizard

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <iostream>
#include <string>
#include <map>

#include "gmatdefs.hpp"
#include "InterpreterException.hpp"
#include "GmatBase.hpp"

// Forward references for GMAT core objects
class Spacecraft;
class Propagator;
class ForceModel;
class PropSetup;
class PhysicalModel;
class SolarSystem;
class CelestialBody;
class Parameter;
class Command;
class Publisher;
class Subscriber;


//#include "Moderator.hpp"
class Moderator;


/**
 * Interpreter is the base class for the GMAT Interpreter subsystem.  
 * 
 * Interpreter defines the interfaces used to parse the text files that control 
 * execution in GMAT.  It also provides the interfaces to write text files out, 
 * either to the file system, the screen, or the GUI.
 */
class Interpreter
{
public:
    Interpreter();
    virtual ~Interpreter();
    
    bool                            SetInStream(std::istream *str);
    bool                            SetOutStream(std::ostream *str);
    
    // The "Interpret" methods translate text streams (e.g. scripts and 
    // subscripts) into GMAT objects 
    virtual bool                    Interpret(void) = 0;
    bool                            InterpretObject(std::string objecttype,
                                                    std::string objectname);
    
    // The "Build" methods take GMAT objects and write out scripts or subscripts
    virtual bool                    Build(void) = 0;
    bool                            BuildObject(std::string &objectname);

protected:
    /// The input stream processed by the interpreter
    std::istream                    *instream;
    /// The iostream processed by the interpreter
    std::ostream                    *outstream;
    /// Pointer to the Moderator for access to the factories and other services
    Moderator                       *moderator;
    /// Flag to tell is the mappings are defined yet
    bool                            initialized;
    
    /// Mapping between the object base class strings and the objecttypes enum
    std::map<std::string, Integer>  typemap;
    
    /// Available commands, obtained from the FactoryManager via the Moderator
    StringArray                     cmdmap;
    /// Available propagators
    StringArray                     propmap;
    /// Available forces
    StringArray                     forcemap;
    /// Available subscribers
    StringArray                     subscribermap;
    /// Available parameters
    StringArray                     parametermap;
    /// Available stopping conditionss
    StringArray                     stopcondmap;

    /// Current line from the script
    std::string                     line;
    std::vector<std::string *>      chunks;
    
    /// Identifies the different types of script lines possible
    enum linetype               { CREATEOBJECT = 7001,
                                  OBJECTPARM,
                                  CONFIGURATION,
                                  TIMELINE,
                                  BLANKLINE,
                                  UNKNOWN };


    void                            Initialize(void);

    // The "Create" methods make calls, through the Moderator, to the Factories
    // to get new instances of the requested objects
    Spacecraft*                     CreateSpacecraft(std::string satname);
    Command*                        CreateCommand(std::string commandtype);
    Propagator*                     CreatePropagator(std::string proptype);
    ForceModel*                     CreateForceModel(std::string modelname);
    PhysicalModel*                  CreatePhysicalModel(std::string forcetype);
    SolarSystem*                    CreateSolarSystem(std::string ssname);
    CelestialBody*                  CreateCelestialBody(std::string cbname, 
                                                        std::string type);
    Parameter*                      CreateParameter(std::string name, 
                                                    std::string type);
//    StoppingCondition*              CreateStopCond(std::string conditiontype);
    Subscriber*                     CreateSubscriber(std::string name, 
                                                     std::string type);
                                                    
    // The following method signature depends on an open scripting issue: if
    // props and force models are named, the following Create method should use
    // the names rather than the object pointers
    PropSetup*                      CreatePropSetup(std::string name);
                                                
    // Methods used to break apart lines of script
    void                            ChunkLine(void);
    Integer                         SkipWhiteSpace(Integer start = 0);
    void                            WriteParameterValue(GmatBase *obj, Integer id);
    Integer                         FindDelimiter(std::string str,
                                                  std::string specChar = "");
    std::string                     GetToken(std::string tokstr = "");
    GmatBase*                       FindObject(std::string objName);
    bool                            SetParameter(GmatBase *obj, Integer id,
                                                 std::string value);
};

#endif // INTERPRETER_HPP

