//$Id$
//------------------------------------------------------------------------------
//                              DynamicLibrary
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/04/18
//
/**
 * Implementation for library code loaded at run time.
 */
//------------------------------------------------------------------------------


#include "DynamicLibrary.hpp"

#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "MessageReceiver.hpp"

// Platform specific library loader routines
#ifdef __WIN32__
#include <windows.h>
#else
#include <dlfcn.h>

/// @todo Put in the Mac/Linux switching logic for dynamic library extensions
// Linux version:
#define UNIX_EXTENSION ".so"

// Mac Version:
//#define UNIX_EXTENSION ".dylib"

#endif


//------------------------------------------------------------------------------
// DynamicLibrary(const std::string &name, const std::string &path)
//------------------------------------------------------------------------------
/**
 * Default constructor.  Dynamic libraries MUST specify the library name.
 * 
 * @param name File name for the dynamic library.
 * @param path File path for the library.  If unspecified, ./ is used.
 */
//------------------------------------------------------------------------------
DynamicLibrary::DynamicLibrary(const std::string &name, 
      const std::string &path) :
   libName          (name),
   libPath          (path),
   libHandle        (NULL)
{
}


//------------------------------------------------------------------------------
// ~DynamicLibrary()
//------------------------------------------------------------------------------
/**
 * Destructor.  Closes the library if it was opened.
 */
//------------------------------------------------------------------------------
DynamicLibrary::~DynamicLibrary()
{
   if (libHandle != NULL)
      #ifdef __WIN32__
         FreeLibrary((HINSTANCE)libHandle);
      #else
         dlclose(libHandle);
      #endif
}


//------------------------------------------------------------------------------
// DynamicLibrary(const DynamicLibrary& dlib)
//------------------------------------------------------------------------------
/**
 * Copy constructor.  Sets the name and path, but does not open the library.
 * 
 * @param dlib The instance that is copied.
 */
//------------------------------------------------------------------------------
DynamicLibrary::DynamicLibrary(const DynamicLibrary& dlib):
   libName          (dlib.libName),
   libPath          (dlib.libPath),
   libHandle        (NULL)
{
}


//------------------------------------------------------------------------------
// DynamicLibrary(const DynamicLibrary& dlib)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.  Sets the name and path, but does not open the library.
 * 
 * @param dlib The instance that is copied to this instance.
 */
//------------------------------------------------------------------------------
DynamicLibrary& DynamicLibrary::operator=(const DynamicLibrary& dlib)
{
   if (&dlib != this)
   {
      libName = dlib.libName;
      libPath = dlib.libPath;
      libHandle = NULL;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// bool LoadDynamicLibrary()
//------------------------------------------------------------------------------
/**
 * Loads the library into memory, and sets the MessageReceiver if necessary.
 * 
 * @return true if the library was loaded.
 */
//------------------------------------------------------------------------------
bool DynamicLibrary::LoadDynamicLibrary()
{
   std::string nameWithPath;
   
   nameWithPath = libPath + libName;

   #ifdef __WIN32__
      libHandle = LoadLibrary(libName.c_str());
   #else
      nameWithPath += UNIX_EXTENSION;
      libHandle = dlopen(nameWithPath.c_str(), RTLD_LAZY);
      
      if (libHandle == NULL)
         MessageInterface::ShowMessage("\n%s\n", dlerror());
   #endif
      
   // Set the MessageReceiver if the Library needs it
   try
   {
      void (*SetMR)(MessageReceiver*) = NULL;
      
      SetMR = (void (*)(MessageReceiver*))GetFunction("SetMessageReceiver");
      MessageReceiver* mr = MessageInterface::GetMessageReceiver();
      SetMR(mr);
   }
   catch (GmatBaseException& ex)
   {
      // Just ignore these exceptions
   }
      
   if (libHandle == NULL)
      return false;
   
   return true;
}


//------------------------------------------------------------------------------
// void (*GetFunction(const std::string &funName))()
//------------------------------------------------------------------------------
/**
 * Retrieves the function pointer for a specified function.
 * 
 * @param funName The name of the function.
 * 
 * @return The function pointer, or NULL if the function was not located.  
 *         A NULL vaaalue should never be found, because the method throws if 
 *         the function is not found.
 */
//------------------------------------------------------------------------------
void (*DynamicLibrary::GetFunction(const std::string &funName))()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + 
            " has not been opened successfully; cannot search for function \"" + 
            funName + "\"\n");
   
   void (*func)() = NULL;
   
   #ifdef __WIN32__
      func = (void(*)())GetProcAddress((HINSTANCE)libHandle, funName.c_str());
   #else
      func = (void(*)())dlsym(libHandle, funName.c_str());
   #endif
      
   if (func == NULL)
      throw GmatBaseException("Library " + libName + 
            " cannot locate the function \"" + 
            funName + "\"\n");
   
   return func;
}


//------------------------------------------------------------------------------
// Integer GetFactoryCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of factories in the plugin.
 * 
 * @return The number of factories.
 */
//------------------------------------------------------------------------------
Integer DynamicLibrary::GetFactoryCount()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for factories \n");
   
   Integer (*FactoryCount)() = NULL;

   try
   {
      FactoryCount = (Integer(*)())GetFunction("GetFactoryCount");
   }
   catch (GmatBaseException& ex)
   {
      return 0;
   }
   
   return FactoryCount();   
}


//------------------------------------------------------------------------------
// Factory* GetGmatFactory(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a Factory pointer from the plugin.
 * 
 * @param index Zero based index into the list of factories
 * 
 * @return The pointer to the factory at the specified index.
 */
//------------------------------------------------------------------------------
Factory* DynamicLibrary::GetGmatFactory(Integer index)
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for factories \n");
   
   Factory* (*GetFactory)(Integer) = NULL;
   GetFactory = (Factory*(*)(Integer))GetFunction("GetFactoryPointer");
   
   Factory *theFactory = GetFactory(index);
   if (theFactory == NULL)
      MessageInterface::ShowMessage(
            "Cannot access factory #%d in the \"%s\" library\n", index, 
            libName.c_str());
      
   return theFactory;
}