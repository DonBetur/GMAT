//$Header$
//------------------------------------------------------------------------------
//                                  ClassName
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/24
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

#include "ConfigManager.hpp"
#include "ConfigManagerException.hpp"

//#define DEBUG_RENAME 1

ConfigManager* ConfigManager::theConfigManager = NULL;


//------------------------------------------------------------------------------
// ConfigManager* Instance(void)
//------------------------------------------------------------------------------
ConfigManager* ConfigManager::Instance(void)
{
   if (!theConfigManager)
      theConfigManager = new ConfigManager;
        
   return theConfigManager;
}


// class constructor
//------------------------------------------------------------------------------
// ConfigManager()
//------------------------------------------------------------------------------
ConfigManager::ConfigManager()
{
   // insert your code here
}

// class destructor
//------------------------------------------------------------------------------
// ~ConfigManager()
//------------------------------------------------------------------------------
ConfigManager::~ConfigManager()
{
   RemoveAllItems();
}

//------------------------------------------------------------------------------
// void AddPhysicalModel(PhysicalModel *pm)
//------------------------------------------------------------------------------
void ConfigManager::AddPhysicalModel(PhysicalModel *pm)
{
   std::string name = pm->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(pm);
      mapping[name] = pm;
   }
}

//------------------------------------------------------------------------------
// void ConfigManager::AddPropagator(Propagator *prop)
//------------------------------------------------------------------------------
void ConfigManager::AddPropagator(Propagator *prop)
{
   std::string name = prop->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(prop);
      mapping[name] = prop;
   }
}


//------------------------------------------------------------------------------
// void AddForceModel(ForceModel *fm)
//------------------------------------------------------------------------------
void ConfigManager::AddForceModel(ForceModel *fm)
{
   std::string name = fm->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(fm);
      mapping[name] = fm;
   }
}


//------------------------------------------------------------------------------
// void AddSubscriber(Subscriber *subs)
//------------------------------------------------------------------------------
void ConfigManager::AddSubscriber(Subscriber *subs)
{
   std::string name = subs->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(subs);
      mapping[name] = subs;
   }
}


//------------------------------------------------------------------------------
// void AddSolarSystem(SolarSystem *solarSys)
//------------------------------------------------------------------------------
void ConfigManager::AddSolarSystem(SolarSystem *solarSys)
{
   throw ConfigManagerException("SolarSystem objects not managed in build 1");
}


//------------------------------------------------------------------------------
// void AddPropSetup(PropSetup* propSetup)
//------------------------------------------------------------------------------
void ConfigManager::AddPropSetup(PropSetup* propSetup)
{
   std::string name = propSetup->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(propSetup);
      mapping[name] = propSetup;
   }
}


//------------------------------------------------------------------------------
// void AddSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
void ConfigManager::AddSpacecraft(SpaceObject *sc)
{
   std::string name = sc->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(sc);
      mapping[name] = sc;
   }
}


//------------------------------------------------------------------------------
// void AddHardware(Hardware *hw)
//------------------------------------------------------------------------------
void ConfigManager::AddHardware(Hardware *hw)
{
   std::string name = hw->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(hw);
      mapping[name] = hw;
   }
}


//------------------------------------------------------------------------------
// void AddStopCondition(StopCondition* stopCond)
//------------------------------------------------------------------------------
void ConfigManager::AddStopCondition(StopCondition* stopCond)
{
   std::string name = stopCond->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(stopCond);
      mapping[name] = stopCond;
   }
}


//------------------------------------------------------------------------------
// void AddParameter(Parameter* parameter)
//------------------------------------------------------------------------------
void ConfigManager::AddParameter(Parameter* parameter)
{
   std::string name = parameter->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(parameter);
      mapping[name] = parameter;
   }
}


//------------------------------------------------------------------------------
// void AddBurn(Burn* burn)
//------------------------------------------------------------------------------
void ConfigManager::AddBurn(Burn* burn)
{
   std::string name = burn->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(burn);
      mapping[name] = burn;
   }
}

//------------------------------------------------------------------------------
// void AddSolver(Solver* solver)
//------------------------------------------------------------------------------
void ConfigManager::AddSolver(Solver* solver)
{
   std::string name = solver->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   if (mapping.find(name) != mapping.end()) {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else {
      objects.push_back(solver);
      mapping[name] = solver;
   }
}

//------------------------------------------------------------------------------
// void AddAtmosphereModel(AtmosphereModel* atmosModel)
//------------------------------------------------------------------------------
void ConfigManager::AddAtmosphereModel(AtmosphereModel* atmosModel)
{
   std::string name = atmosModel->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (mapping.find(name) != mapping.end())
   {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else
   {
      objects.push_back(atmosModel);
      mapping[name] = atmosModel;
   }
}

//------------------------------------------------------------------------------
// void AddFunction(Function* function)
//------------------------------------------------------------------------------
void ConfigManager::AddFunction(Function* function)
{
   std::string name = function->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (mapping.find(name) != mapping.end())
   {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else
   {
      objects.push_back(function);
      mapping[name] = function;
   }
}

//------------------------------------------------------------------------------
// void AddCoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void ConfigManager::AddCoordinateSystem(CoordinateSystem *cs)
{
   std::string name = cs->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (mapping.find(name) != mapping.end())
   {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else
   {
      objects.push_back(cs);
      mapping[name] = cs;
   }
}

//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool ConfigManager::SetSolarSystemInUse(const std::string &name)
{
   return false;
}

//------------------------------------------------------------------------------
// StringArray& GetListOfAllItems()
//------------------------------------------------------------------------------
StringArray& ConfigManager::GetListOfAllItems()
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// StringArray& GetListOfItems(Gmat::ObjectType itemType)
//------------------------------------------------------------------------------
StringArray& ConfigManager::GetListOfItems(Gmat::ObjectType itemType)
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if ((*current)->GetType() == itemType)
         listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// GmatBase* GetItem(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ConfigManager::GetItem(const std::string &name)
{   
   GmatBase *obj = NULL;
    
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetName() == name)
      {
         obj = mapping[name];
      }
   }
    
   return obj;
}

//------------------------------------------------------------------------------
// bool RenameItem(Gmat::ObjectType type, const std::string &oldName,
//                 const std::string &newName)
//------------------------------------------------------------------------------
bool ConfigManager::RenameItem(Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::RenameItem() type=%d, oldName=%s, newName=%s\n",
       type, oldName.c_str(), newName.c_str());
#endif
   bool renamed = false;
    
   if (mapping.find(oldName) != mapping.end())
   {
      GmatBase *obj = mapping[oldName];
      if (obj->GetType() == type)
      {
         // if newName does not exist, change name (loj: 11/19/04 - added)
         if (mapping.find(newName) == mapping.end())
         {
            mapping.erase(oldName);
            mapping[newName] = obj;
            obj->SetName(newName);
            renamed = true;
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "%s already exist, Please enter different name.\n",
                newName.c_str());
         }
      }
   }
   
   if (!renamed)
   {
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::RenameItem() Unable to rename: oldName not found.\n");
#endif
      return false;
   }
   
   //loj: 11/17/04 - added
   //--------------------------------------------------
   // rename ref. object name used in parameters
   //--------------------------------------------------
   
   if (type == Gmat::SPACECRAFT)
   {
      StringArray params = GetListOfItems(Gmat::PARAMETER);
      StringArray subs = GetListOfItems(Gmat::SUBSCRIBER);
      Subscriber *sub;
      Parameter *param;
      std::string oldParamName;
      std::string newParamName;
      
      for (unsigned int i=0; i<params.size(); i++)
      {
#if DEBUG_RENAME
         MessageInterface::ShowMessage("params[%d]=%s\n", i, params[i].c_str());
#endif
         
         param = GetParameter(params[i]);
         
         // if system parameter
         if (param->GetKey() == GmatParam::SYSTEM_PARAM) //loj: 12/10/04 Changed from Parameter::
         {
            oldParamName = param->GetName();
            // if parameter name has old name (loj: 11/23/04 - added)
            if (oldParamName.find(oldName) != oldParamName.npos)
            {
               // rename ref. object name
               param->RenameRefObject(type, oldName, newName);
               
               // rename actual parameter name
               newParamName = newName + "." + param->GetTypeName();
               
               // rename configured parameter name
               renamed = RenameItem(Gmat::PARAMETER, oldParamName, newParamName);
               
#if DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("newParamName=%s\n", param->GetName().c_str());
               MessageInterface::ShowMessage
                  ("===> Change Subscriber ref object names\n");
#endif
               //--------------------------------------------------
               // rename ref. objects used in subscribers
               //--------------------------------------------------
               for (unsigned int i=0; i<subs.size(); i++)
               {
                  sub = GetSubscriber(subs[i]);
                  if (sub->GetTypeName() == "OpenGLPlot")
                  {
                     sub->RenameRefObject(type, oldName, newName);
                  }
                  else if (sub->GetTypeName() == "XYPlot" ||
                           sub->GetTypeName() == "ReportFile")
                  {
                     sub->RenameRefObject(Gmat::PARAMETER, oldParamName, newParamName);
                  }
               }
            }
         }
         else if (param->GetTypeName() == "Variable")
         {
            // if parameter name has oldName replace with newName
            oldParamName = param->GetName();
            newParamName = oldParamName;
            std::string::size_type pos = newParamName.find(oldName);
            
            if (pos != newParamName.npos)
               newParamName.replace(pos, oldName.size(), newName);
        
            // if parameter expression has oldName replace with newName
            std::string newExp = param->GetStringParameter("Expression");
            pos = newExp.find(oldName);
            
            if (pos != newExp.npos)
            {
               newExp.replace(pos, oldName.size(), newName);
               param->SetStringParameter("Expression", newExp);
               
               // rename ref. parameter name
               StringArray refParamNames = param->GetStringArrayParameter("RefParams");
               for (unsigned int i=0; i<refParamNames.size(); i++)
               {
                  oldParamName = refParamNames[i];
                  newParamName = oldParamName;
                  pos = oldParamName.find(oldName);
                  if (pos != oldParamName.npos)
                  {
                     newParamName.replace(pos, oldName.size(), newName);
                     param->RenameRefObject(Gmat::PARAMETER, oldParamName, newParamName);
                  }
               }
            }
         }//if (param->GetKey() == Parameter::SYSTEM_PARAM)
      } //for (unsigned int i=0; i<params.size(); i++)
   }
   
   
   return renamed;
}


//------------------------------------------------------------------------------
// bool RemoveAllItems()
//------------------------------------------------------------------------------
bool ConfigManager::RemoveAllItems()
{
   // delete objects

   for (unsigned int i=0; i<objects.size(); i++)
   {
      delete objects[i];
      objects[i] = NULL;
   }
    
   objects.clear();
   mapping.clear();

   return true;
}

//------------------------------------------------------------------------------
// bool RemoveItem(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
bool ConfigManager::RemoveItem(Gmat::ObjectType type, const std::string &name)
{
   bool status = false;

   // remove from objects
   std::vector<GmatBase*>::iterator currentIter =
      (std::vector<GmatBase*>::iterator)(objects.begin());
    
   while (currentIter != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if ((*currentIter)->GetType() == type)
      {
         if ((*currentIter)->GetName() == name)
         {
            objects.erase(currentIter);
            break;
         }
      }
      ++currentIter;
   }
    
   // remove from mapping
   if (mapping.find(name) != mapping.end())
   {
      GmatBase *obj = mapping[name];
      if (obj->GetType() == type)
      {
         mapping.erase(name);
            
         delete obj;
            
         status = true;
      }
   }
    
   return status;
}


//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
PhysicalModel* ConfigManager::GetPhysicalModel(const std::string &name)
{
   PhysicalModel *physicalModel = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::PHYSICAL_MODEL) {
         std::string str = mapping[name]->GetName() +
            " is not a PhysicalModel";
         throw ConfigManagerException(str);
      }
      physicalModel = (PhysicalModel *)mapping[name];
   }
   return physicalModel;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
Propagator* ConfigManager::GetPropagator(const std::string &name)
{
   Propagator *prop = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::PROPAGATOR) {
         std::string str = mapping[name]->GetName() +
            " is not a Propagator";
         throw ConfigManagerException(str);
      }
      prop = (Propagator *)mapping[name];
   }
   return prop;
}


//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
ForceModel* ConfigManager::GetForceModel(const std::string &name)
{
   ForceModel *fm = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::FORCE_MODEL) {
         //std::string str = mapping[name]->GetName() +
         //                  " is not a force model";
         //throw ConfigManagerException(str);
         return NULL;
      }
      fm = (ForceModel *)mapping[name];
   }
   return fm;
}


//------------------------------------------------------------------------------
// Spacecraft* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
SpaceObject* ConfigManager::GetSpacecraft(const std::string &name)
{
   SpaceObject *sc = NULL;
   if (mapping.find(name) != mapping.end()) {
      if ((mapping[name]->GetType() != Gmat::SPACECRAFT) &&
          (mapping[name]->GetType() != Gmat::FORMATION)){
         //std::string str = mapping[name]->GetName() +
         //                  " is not a spacecraft";
         //throw ConfigManagerException(str);
         return NULL;
      }
      sc = (SpaceObject *)mapping[name];
   }
   return sc;
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
Hardware* ConfigManager::GetHardware(const std::string &name)
{
   Hardware *hw = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() == Gmat::HARDWARE) {
         hw = (Hardware *)mapping[name];
      }
   }
   return hw;
}


//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* ConfigManager::GetPropSetup(const std::string &name)
{
   PropSetup *ps = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::PROP_SETUP) {
         //std::string str = mapping[name]->GetName() +
         //                  " is not a PropSetup";
         //throw ConfigManagerException(str);
         return NULL;
      }
      ps = (PropSetup *)mapping[name];
   }
   return ps;
}


//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
Subscriber* ConfigManager::GetSubscriber(const std::string &name)
{
   Subscriber *sub = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::SUBSCRIBER) {
         //std::string str = mapping[name]->GetName() +
         //                  " is not a PropSetup";
         //throw ConfigManagerException(str);
         return NULL;
      }
      sub = (Subscriber *)mapping[name];
   }
   return sub;
}


//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetDefaultSolarSystem()
{
   throw ConfigManagerException
      ("ConfigManager::GetDefaultSolarSystem() has not been implemented.\n");
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetSolarSystemInUse()
{
   throw ConfigManagerException
      ("ConfigManager::GetSolarSystemInUse() has not been implemented.\n");
}


//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
StopCondition* ConfigManager::GetStopCondition(const std::string &name)
{
   StopCondition *sc = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::STOP_CONDITION) {
         std::string str = mapping[name]->GetName() +
            " is not a spacecraft";
         throw ConfigManagerException(str);
      }
      sc = (StopCondition *)mapping[name];
   }
   return sc;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
Parameter* ConfigManager::GetParameter(const std::string &name)
{
   Parameter *param = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::PARAMETER) {
         std::string str = mapping[name]->GetName() +
            " is not a spacecraft";
         throw ConfigManagerException(str);
      }
      param = (Parameter *)mapping[name];
   }
   return param;
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
Burn* ConfigManager::GetBurn(const std::string &name)
{
   Burn *burn = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::BURN) {
         std::string str = mapping[name]->GetName() +
            " is not a burn";
         throw ConfigManagerException(str);
      }
      burn = (Burn *)mapping[name];
   }
   return burn;
}

//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
Solver* ConfigManager::GetSolver(const std::string &name)
{
   Solver *solver = NULL;
   if (mapping.find(name) != mapping.end()) {
      if (mapping[name]->GetType() != Gmat::SOLVER) {
         std::string str = mapping[name]->GetName() +
            " is not a solver";
         throw ConfigManagerException(str);
      }
      solver = (Solver *)mapping[name];
   }
   return solver;
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
AtmosphereModel* ConfigManager::GetAtmosphereModel(const std::string &name)
{
   AtmosphereModel *atmosModel = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetType() != Gmat::ATMOSPHERE)
      {
         std::string str = mapping[name]->GetName() +
            " is not an AtmosphereModel type";
         throw ConfigManagerException(str);
      }
      atmosModel = (AtmosphereModel *)mapping[name];
   }
   return atmosModel;
}

//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
Function* ConfigManager::GetFunction(const std::string &name)
{
   Function *function = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetType() != Gmat::FUNCTION)
      {
         std::string str = mapping[name]->GetName() + " is not a Function type";
         throw ConfigManagerException(str);
      }
      function = (Function *)mapping[name];
   }
   return function;
}

//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
CoordinateSystem* ConfigManager::GetCoordinateSystem(const std::string &name)
{
   CoordinateSystem *cs = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetType() != Gmat::COORDINATE_SYSTEM)
      {
         std::string str = mapping[name]->GetName() + " is not a CoordinateSystem type";
         throw ConfigManagerException(str);
      }
      cs = (CoordinateSystem *)mapping[name];
   }
   return cs;
}

//=================================
// Methods I'm not sure we need
//=================================

//------------------------------------------------------------------------------
// void AddCelestialBody(CelestialBody* body)
//------------------------------------------------------------------------------
void ConfigManager::AddCelestialBody(CelestialBody* body)
{
   throw ConfigManagerException
      ("ConfigManager::AddCelestialBody() has not been implemented.\n");
}

//------------------------------------------------------------------------------
// void AddCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
void ConfigManager::AddCommand(GmatCommand *cmd)
{
   throw ConfigManagerException
      ("ConfigManager::AddCommand() has not been implemented.\n");
}

//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
CelestialBody* ConfigManager::GetCelestialBody(const std::string &name)
{
   throw ConfigManagerException
      ("ConfigManager::GetCelestialBody() has not been implemented.\n");
}

//------------------------------------------------------------------------------
// GmatCommand* GetCommand(const std::string name)
//------------------------------------------------------------------------------
GmatCommand* ConfigManager::GetCommand(const std::string name)
{
   throw ConfigManagerException
      ("ConfigManager::GetCommand() has not been implemented.\n");
}
