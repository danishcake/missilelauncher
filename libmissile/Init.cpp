#include "Missile.h"
#include <boost/lexical_cast.hpp>
#include <libusb.h>
#include <string>
#include <vector>
extern void Log(std::string);

libusb_context* usb_context = 0;
std::vector<libusb_device_handle*> device_handles;
CallbackFunction __Log = NULL;

void Log(std::string msg)
{
   if (__Log)
   {
      __Log(msg.c_str());
   }
}

extern "C"
{
EXPORTED 
int InitialiseUSBControl(CallbackFunction log_fn)
{
   __Log = log_fn;

   int errnum = 0;

   Log("Initialising");
   if (errnum = libusb_init(&usb_context))
   {
      Log("Error initialising libusb:" + errnum);
      return false;
   }

   
      Log("libusb version " +
          boost::lexical_cast<std::string, uint16_t>(libusb_get_version()->major) + "." +
          boost::lexical_cast<std::string, uint16_t>(libusb_get_version()->minor) + "." +
          boost::lexical_cast<std::string, uint16_t>(libusb_get_version()->micro) + "." +
          boost::lexical_cast<std::string, uint16_t>(libusb_get_version()->nano) + " " + 
          libusb_get_version()->describe);
   

   libusb_set_debug(usb_context, 3);

   libusb_device** devices;
   ssize_t device_count = libusb_get_device_list(usb_context, &devices);

   Log("Found " + boost::lexical_cast<std::string, ssize_t>(device_count) + " USB devices");
   Log("Looking for devices where idVendor == 0x1130 and idProduct == 0x0202");

   for(int i = 0; i < device_count; i++)
   {
      libusb_device* device = devices[i];
      Log("Device         #:" + boost::lexical_cast<std::string, ssize_t>(i));
      Log("  Bus           :" + boost::lexical_cast<std::string, int>(libusb_get_bus_number(device)));
      Log("  Address       :" + boost::lexical_cast<std::string, int>(libusb_get_device_address(device)));
      Log("  Speed         :" + boost::lexical_cast<std::string, int>(libusb_get_device_speed(device)));

      libusb_device_descriptor device_desc;
      if (errnum = libusb_get_device_descriptor(device, &device_desc))
      {
         Log(std::string("Unable to get device descriptor. Error: ") + libusb_error_name(errnum));
         continue;
      }

      Log("  Vendor ID     :" + boost::lexical_cast<std::string, int>(device_desc.idVendor));
      Log("  Product ID    :" + boost::lexical_cast<std::string, int>(device_desc.idProduct));

      if (device_desc.idVendor = 0x1130 && device_desc.idProduct == 0x0202)
      {
         libusb_device_handle* device_handle;
         if (errnum = libusb_open(device, &device_handle))
         {
            Log(std::string("Unable to open device. Error: ") + libusb_error_name(errnum));
            continue;
         }

         if (libusb_kernel_driver_active(device_handle, 1))
         {
             if (errnum = libusb_detach_kernel_driver(device_handle, 1))
             {
                Log(std::string("Unable to detact kernel driver from interface 1. Error: ") + libusb_error_name(errnum));
                continue;
             }
         }
         
         Log("Detached kernel driver");

         if (errnum =libusb_claim_interface(device_handle, 1))
         {
            Log(std::string("Unable to claim interface. Error: ") + libusb_error_name(errnum));
            libusb_close(device_handle);
            continue;
         }

         Log("Opened USB Missile launcher on Bus " + boost::lexical_cast<std::string, int>(libusb_get_bus_number(device)) +
             " address " + boost::lexical_cast<std::string, int>(libusb_get_device_address(device)));
         device_handles.push_back(device_handle);
      }
   }

   libusb_free_device_list(devices, 1);

   return (int)device_handles.size();
}

EXPORTED 
bool ShutdownUSBControl()
{
   if (usb_context)
   {
      for(std::vector<libusb_device_handle*>::iterator it = device_handles.begin(); it != device_handles.end(); ++it)
      {
         libusb_release_interface(*it, 0);
         libusb_close(*it);
      }
      device_handles.clear();

      libusb_exit(usb_context);
   }
   return true;
}
}
