#include "Missile.h"
#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <vector>
extern void Log(std::string);

static int PythonLogger(const char* msg)
{
   std::cout << msg << std::endl;
   return 0;
}

static int PythonInitialise()
{
   return InitialiseUSBControl(&PythonLogger);
}

// Default arguments are lost by function pointers - this macro
// builds thin wrappers for PerformAction with default arguments
BOOST_PYTHON_FUNCTION_OVERLOADS(PerformActionOverloads, PerformAction, 1, 2)

BOOST_PYTHON_MODULE(libmissile)
{
   boost::python::enum_<Action::Enum>("Action").value("Stop",              Action::Stop).
                                                value("RotateLeft",        Action::RotateLeft).
                                                value("RotateRight",       Action::RotateRight).
                                                value("RotateUp",          Action::RotateUp).
                                                value("RotateDown",        Action::RotateDown).
                                                value("RotateLeftUp",      Action::RotateLeftUp).
                                                value("RotateRightUp",     Action::RotateRightUp).
                                                value("RotateLeftDown",    Action::RotateLeftDown).
                                                value("RotateRightDown",   Action::RotateRightDown).
                                                value("Fire",              Action::Fire);
   boost::python::def("InitialiseUSBControl", PythonInitialise);
   boost::python::def("ShutdownUSBControl", ShutdownUSBControl);
   boost::python::def("PerformAction", PerformAction, PerformActionOverloads());

}