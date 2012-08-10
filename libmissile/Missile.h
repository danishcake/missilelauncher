#pragma once
#include <string>


#ifdef AS_DLL
 #ifdef _LIB
   #define EXPORTED __declspec(dllexport)
 #else
  #define EXPORTED __declspec(dllimport)
 #endif
#else
 #define EXPORTED
#endif

typedef int (*CallbackFunction)(const char*);

namespace Action
{
   EXPORTED enum Enum
   {
      Stop,
      RotateLeft, RotateRight, RotateUp, RotateDown, 
      RotateLeftUp, RotateRightUp,
      RotateLeftDown, RotateRightDown,
      Fire
   };
}

extern "C"
{
EXPORTED int InitialiseUSBControl(CallbackFunction log_fn);
EXPORTED bool ShutdownUSBControl();
EXPORTED bool PerformAction(Action::Enum action, int device_index = -1);
}