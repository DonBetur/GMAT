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
 * Class implementation for the Interpreter base class
 */
//------------------------------------------------------------------------------




// Class automatically generated by Dev-C++ New Class wizard

#include "Interpreter.hpp" // class's header file

#include "Moderator.hpp"


/// class constructor
Interpreter::Interpreter() :
    instream            (NULL),
    outstream           (NULL),
    initialized         (false)
{
    moderator = Moderator::Instance();
    
    if (moderator) {
        // Set up the mapping for the core types
        typemap["Spacecraft"] = Gmat::SPACECRAFT;
        typemap["GroundStation"] = Gmat::GROUND_STATION;
        typemap["Command"] = Gmat::COMMAND;
//        typemap["Propagator"] = Gmat::PROPAGATOR;
        typemap["ForceModel"] = Gmat::FORCE_MODEL;
        typemap["Force"] = Gmat::PHYSICAL_MODEL;
        typemap["SolarSystem"] = Gmat::SOLAR_SYSTEM;
        typemap["CelestialBody"] = Gmat::CELESTIAL_BODY;
        typemap["Parameter"] = Gmat::PARAMETER;
//        typemap["StoppingCondition"] = Gmat::STOP_CONDITION;
        typemap["Subscriber"] = Gmat::SUBSCRIBER;
        typemap["Propagator"] = Gmat::PROP_SETUP;
    }
}

/// class destructor
Interpreter::~Interpreter()
{
}



void Interpreter::Initialize(void)
{
    // Build a mapping for all of the defined commands
    StringArray cmds = moderator->GetListOfFactoryItems(Gmat::COMMAND);
    copy(cmds.begin(), cmds.end(), back_inserter(cmdmap));
    
    StringArray props = moderator->GetListOfFactoryItems(Gmat::PROPAGATOR);
    copy(props.begin(), props.end(), back_inserter(propmap));

    StringArray forces = moderator->GetListOfFactoryItems(Gmat::PHYSICAL_MODEL);
    copy(forces.begin(), forces.end(), back_inserter(forcemap));
    
    StringArray subs = moderator->GetListOfFactoryItems(Gmat::SUBSCRIBER);
    copy(subs.begin(), subs.end(), back_inserter(subscribermap));

    StringArray parms = moderator->GetListOfFactoryItems(Gmat::PARAMETER);
    copy(parms.begin(), parms.end(), back_inserter(parametermap));

    StringArray sconds = moderator->GetListOfFactoryItems(Gmat::STOP_CONDITION);
    copy(sconds.begin(), sconds.end(), back_inserter(stopcondmap));

    #ifdef DEBUG_OBJECT_LISTS
        std::cout << "\nCommands:\n   ";
        for (std::vector<std::string>::iterator c = cmds.begin(); c != cmds.end(); ++c)
            std::cout << *c << "\n   ";

        std::cout << "\nPropagators:\n   ";
        for (std::vector<std::string>::iterator p = props.begin(); p != props.end(); ++p)
            std::cout << *p << "\n   ";

        std::cout << "\nForces:\n   ";
        for (std::vector<std::string>::iterator f = forces.begin(); f != forces.end(); ++f)
            std::cout << *f << "\n   ";

        std::cout << "\nSubscribers:\n   ";
        for (std::vector<std::string>::iterator s = subs.begin(); s != subs.end(); ++s)
            std::cout << *s << "\n   ";

        std::cout << "\nParameters:\n   ";
        for (std::vector<std::string>::iterator pm = parms.begin(); pm != parms.end(); ++pm)
            std::cout << *pm << "\n   ";

        std::cout << "\nStopConds:\n   ";
        for (std::vector<std::string>::iterator sc = sconds.begin(); sc != sconds.end(); ++sc)
           std::cout << *sc << "\n   ";

        std::cout << "\n";
    #endif

    initialized = true;
}


bool Interpreter::SetInStream(std::istream *str)
{
    instream = str;
    return true;
}


bool Interpreter::SetOutStream(std::ostream *str)
{
    outstream = str;
    return true;
}


// The "Interpret" methods translate text streams (e.g. scripts and 
// subscripts) into GMAT objects 
bool Interpreter::Interpret(void)
{
    return false;
}


bool Interpreter::InterpretObject(std::string objecttype, std::string objectname)
{
    if (objecttype == "Spacecraft") {
        CreateSpacecraft(objectname);
        return true;
    }
    
    if (objecttype == "Propagator") {
        // PropSetup *prop =
        CreatePropSetup(objectname);
        // // Add the force model container
        // ForceModel *fm = moderator->CreateForceModel("");
        // prop->SetForceModel(fm);
        return true;
    }

    if (objecttype == "ForceModel") {
        // ForceModel *prop =
        CreateForceModel(objectname);
        return true;
    }

    return false;
}


// The "Build" methods take GMAT objects and write out scripts or subscripts
bool Interpreter::Build(void)
{
    return false;
}


bool Interpreter::BuildObject(std::string objectname)
{
    return false;
}



// The "Create" methods make calls, through the Moderator, to the Factories
// to get new instances of the requested objects
Spacecraft* Interpreter::CreateSpacecraft(std::string satname)
{
    return moderator->CreateSpacecraft("Spacecraft", satname);
}


Command* Interpreter::CreateCommand(std::string commandtype)
{
    return moderator->CreateCommand(commandtype, "");
}


Propagator* Interpreter::CreatePropagator(std::string proptype)
{
    return moderator->CreatePropagator(proptype, "");
}


ForceModel* Interpreter::CreateForceModel(std::string modelname)
{
    return moderator->CreateForceModel(modelname);
}


PhysicalModel* Interpreter::CreatePhysicalModel(std::string forcetype)
{
    return moderator->CreatePhysicalModel(forcetype, "");
}


SolarSystem* Interpreter::CreateSolarSystem(std::string ssname)
{
    return moderator->CreateSolarSystem(ssname);
}


CelestialBody* Interpreter::CreateCelestialBody(std::string cbname, std::string type)
{
    return moderator->CreateCelestialBody(cbname, type);
}


Parameter* Interpreter::CreateParameter(std::string name, std::string type)
{
    return moderator->CreateParameter(type, name);
}


//StoppingCondition* Interpreter::CreateStopCond(std::string conditiontype)
//{
//    return moderator->CreateStopCondition(conditiontype);
//}


Subscriber* Interpreter::CreateSubscriber(std::string name, std::string type)
{
    return moderator->CreateSubscriber(type, name);
}


                                                
// The following method signature depends on an open scripting issue: if
// props and force models are named, the following Create method should use
// the names rather than the object pointers
PropSetup* Interpreter::CreatePropSetup(std::string name)
{
    return moderator->CreatePropSetup(name);
}



void Interpreter::ChunkLine(void)
{
    // Break the line into pieces based on while space and special characters
    Integer start = 0, end;
    const char *str = line.c_str();
    std::string phrase;

    start = SkipWhiteSpace(start); // Find the beginning of the text
    while (start != -1) {
        end = start;
        while ((str[end] != ' ') && (str[end] != '\t') && (str[end] != '\r') &&
               (str[end] != '\n') && (str[end] != '%') && (str[end] != '\0'))
            ++end;
        phrase.assign(line, start, (end-start));
        chunks.push_back(new std::string(phrase));
        start = SkipWhiteSpace(end);
    }
}


Integer Interpreter::SkipWhiteSpace(Integer start)
{
    Integer finish = start;
    const char *str = line.c_str();
    while ((str[finish] == ' ') || (str[finish] == '\t')) {
        ++finish;
        if ((str[finish] == '\r') || (str[finish] == '\n') ||
            (str[finish] == '%') || (str[finish] == '\0'))
            return -1;
    }
    if (start >= strlen(str))
        return -1;
    return finish;
}


/**
 * Method to find special characters: = , { } [ ] ( ) ; . %
 */
Integer Interpreter::FindDelimiter(std::string str, std::string specChar)
{
    return -1;
}


std::string Interpreter::GetToken(std::string tokstr)
{
    static std::string str, token = "";
    static Integer start;
    if (tokstr != "") {
        str = tokstr;
        start = 0;
    }

    if (start >= strlen(str.c_str()))
        token = "";
    else {
        Integer loc = str.find(".", start);
        if (loc == std::string::npos)
            loc = strlen(str.c_str());
        token.assign(str, start, loc-start);
        start = loc+1;
    }
    
    return token;
}


GmatBase* Interpreter::FindObject(std::string objName)
{
    GmatBase *obj;
    
    obj = moderator->GetSpacecraft(objName);
    if (obj == NULL)
        obj = moderator->GetForceModel(objName);
    if (obj == NULL)
        obj = moderator->GetPropSetup(objName);

    return obj;
}


bool Interpreter::SetParameter(GmatBase *obj, Integer id, std::string value)
{
    bool retval = false;
    
    Gmat::ParameterType type = obj->GetParameterType(id);
    if (type == Gmat::INTEGER_TYPE){
        obj->SetIntegerParameter(id, atoi(value.c_str()));
        retval = true;
    }
    if (type == Gmat::REAL_TYPE) {
        obj->SetRealParameter(id, atof(value.c_str()));
        retval = true;
    }

    return retval;
}


//bool Interpreter::CheckMemberObject(GmatBase *obj, std::string memberID)
//{
//    return false;
//}
