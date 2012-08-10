#include "Missile.h"
#include <libusb.h>
#include <cstring>
#include <string>
#include <vector>
extern void Log(std::string);

extern std::vector<libusb_device_handle*> device_handles;

// Byte sequence to send to the device
static uint8_t INITA[]     = {85, 83, 66, 67,  0,  0,  4,  0};
static uint8_t INITB[]     = {85, 83, 66, 67,  0, 64,  2,  0};
static uint8_t STOP[]      = { 0,  0,  0,  0,  0,  0, 8, 8};
static uint8_t LEFT[]      = { 0,  1,  0,  0,  0,  0, 8, 8};
static uint8_t RIGHT[]     = { 0,  0,  1,  0,  0,  0, 8, 8};
static uint8_t UP[]        = { 0,  0,  0,  1,  0,  0, 8, 8};
static uint8_t DOWN[]      = { 0,  0,  0,  0,  1,  0, 8, 8};
static uint8_t LEFTUP[]    = { 0,  1,  0,  1,  0,  0, 8, 8};
static uint8_t RIGHTUP[]   = { 0,  0,  1,  1,  0,  0, 8, 8};
static uint8_t LEFTDOWN[]  = { 0,  1,  0,  0,  1,  0, 8, 8};
static uint8_t RIGHTDOWN[] = { 0,  0,  1,  0,  1,  0, 8, 8};
static uint8_t FIRE[]      = { 0,  0,  0,  0,  0,  1, 8, 8};


EXPORTED
bool PerformAction(Action::Enum action, int device_index)
{
   if (device_handles.size() == 0)
   {
      Log("Not yet initialised - call InitialiseUSBControl() first");
      Log("If you already have ensure that missile launcher is plugged in!");
      return false;
   }

   uint8_t command_buffer[64] = {0};
   uint8_t* commands[] = {STOP, LEFT, RIGHT, UP, DOWN, LEFTUP, RIGHTUP, LEFTDOWN, RIGHTDOWN, FIRE};
   if (action >= Action::Stop && action <= Action::Fire)
   {
      memcpy(command_buffer, commands[action], 8);

      std::vector<libusb_device_handle*> active_handles;
      if (device_index == -1)
      {
         active_handles = device_handles;
      }
      else if (device_index >= 0 && device_index < (int)device_handles.size())
      {
         active_handles.push_back(device_handles[device_index]);
      }

      for(std::vector<libusb_device_handle*>::iterator it = active_handles.begin(); it != active_handles.end(); ++it)
      {
         int errnum = 0;
         if ((errnum = libusb_control_transfer(*it, 0x21, 0x09, 0x02, 0x01, INITA, sizeof(INITA), 1000) < 0) ||
             (errnum = libusb_control_transfer(*it, 0x21, 0x09, 0x02, 0x01, INITB, sizeof(INITA), 1000) < 0) ||
             (errnum = libusb_control_transfer(*it, 0x21, 0x09, 0x02, 0x01, command_buffer, 64, 1000)))
         {
//            Log(std::string("Unable to write command to device") + libusb_error_name(errnum));
         }
      }
   }

   return true;
}
