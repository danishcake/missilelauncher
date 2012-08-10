#include "Tracking.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <time.h>
#include <boost/lexical_cast.hpp>

#ifndef WIN32
	#include <unistd.h>
#endif

extern "C"
{
#include <wiiuse.h>
}

namespace ControlMode
{
   enum Enum
   {
      Off, Seeking, Slaved
   };
}

static ControlMode::Enum   sControlMode = ControlMode::Off;
static ControlResult       sResult;
static bool                sTrackValid = false;
static wiimote**           sWiimotes = 0;
static clock_t             sTrackAge;
static int                 sSensitivity = 3;

extern void Log(std::string);

void change_control_mode(ControlMode::Enum cm)
{
   if (cm == ControlMode::Seeking && sControlMode != ControlMode::Seeking)
   {
      Log("Switching to seeker mode\n");
      sTrackValid = false;
      sControlMode = ControlMode::Seeking;
   }

   if (cm == ControlMode::Slaved && sControlMode != ControlMode::Slaved)
   {
      Log("Switching to slaved mode\n");
      sTrackValid = false;
      sControlMode = ControlMode::Slaved;
   }
   
   if (cm == ControlMode::Off && sControlMode != ControlMode::Off)
   {
      Log("Switching to off mode\n");
      sTrackValid = false;
      sControlMode = ControlMode::Off;
   }
}

void handle_event(struct wiimote_t* wm)
{
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
   {
      sSensitivity++;
      if (sSensitivity > 5) sSensitivity = 5;
      wiiuse_set_ir_sensitivity(wm, sSensitivity);
      Log("Sensitivity set to " + boost::lexical_cast<std::string, int>(sSensitivity) + "\n");
   }
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
   {
      sSensitivity--;
      if (sSensitivity < 0) sSensitivity = 0;
      wiiuse_set_ir_sensitivity(wm, sSensitivity);
      Log("Sensitivity set to " + boost::lexical_cast<std::string, int>(sSensitivity) + "\n");
   }


   // Button A switches to track mode
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))
   {
      change_control_mode(ControlMode::Seeking);
   }

   // B fires the missile
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))
   {
      sResult.fire = true;
   }
   // Manually steer
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))
   {
      sResult.steering_demand.y = -1;
      sResult.steering_demand.z = -1;
      change_control_mode(ControlMode::Slaved);
   }
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
   {
      sResult.steering_demand.y =  1;
      sResult.steering_demand.z = -1;
      change_control_mode(ControlMode::Slaved);
   }
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))
   {
      sResult.steering_demand.x = -1;
      sResult.steering_demand.z = -1;
      change_control_mode(ControlMode::Slaved);
   }
   if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))
   {
      sResult.steering_demand.x =  1;
      sResult.steering_demand.z = -1;
      change_control_mode(ControlMode::Slaved);
   }

   /*
    *	If IR tracking is enabled then print the coordinates
    *	on the virtual screen that the wiimote is pointing to.
    *
    *	Also make sure that we see at least 1 dot.
    */
   if (sControlMode == ControlMode::Seeking)
   {
      if (wm->ir.num_dots > 0)
      {
         sResult.steering_demand.x = 512 - (float)wm->ir.ax;
         sResult.steering_demand.y = 384 - (float)wm->ir.ay;
         sResult.steering_demand.z = -1;
         // Dead zone of 25
         if (fabs(sResult.steering_demand.x) < 50) sResult.steering_demand.x = 0;
         if (fabs(sResult.steering_demand.y) < 50) sResult.steering_demand.y = 0;
         sTrackValid = true;
         sTrackAge = clock();
         Log(boost::lexical_cast<std::string, int>(wm->ir.num_dots) + " dots. Centre (" + 
             boost::lexical_cast<std::string, int>(wm->ir.ax) + "," +
             boost::lexical_cast<std::string, int>(wm->ir.ay) + ")\n");
      }
   }

}

void InitRemote()
{
   sTrackAge = clock();
   if (sWiimotes == 0)
   {
      sWiimotes = wiiuse_init(1);
   }
}

void ShutdownRemote()
{
   if (sWiimotes != 0)
   {
      wiiuse_cleanup(sWiimotes, 1);
      sWiimotes = 0;
   }
}

/**
 *	@brief main()
 *
 *	Connect to up to two wiimotes and print any events
 *	that occur on either device.
 */
ControlResult TickRemote()
{
   // If not connected then attempt to connect
   if (sControlMode == ControlMode::Off)
   {
      sResult.steering_demand.x = 0;
      sResult.steering_demand.y = 0;
      sResult.steering_demand.z = 0;
      sResult.fire = false;
      sResult.exit = false;

      int found = wiiuse_find(sWiimotes, 1, 5);
      
      if (found > 0)
      {
         int connected = wiiuse_connect(sWiimotes, 1);
         if (connected > 0)
         {
            change_control_mode(ControlMode::Slaved);
            wiiuse_set_leds(sWiimotes[0], WIIMOTE_LED_1);
            wiiuse_motion_sensing(sWiimotes[0], 0);
            usleep(10000);
            wiiuse_set_ir(sWiimotes[0], 1);
            wiiuse_set_ir_sensitivity(sWiimotes[0], 1);
         }
      }
   }

   bool first = true;
   while (sControlMode != ControlMode::Off &&
          wiiuse_poll(sWiimotes, 1)) // Returns number of events to be processed
   {
      if (first)
      {
         first = false;
         sResult.steering_demand.x = 0;
         sResult.steering_demand.y = 0;
         sResult.steering_demand.z = 0;
         sResult.fire = false;
         sResult.exit = false;
      }

      switch (sWiimotes[0]->event) {
         case WIIUSE_EVENT:
            /* a generic event occured */
            handle_event(sWiimotes[0]);
            break;

         case WIIUSE_DISCONNECT:
         case WIIUSE_UNEXPECTED_DISCONNECT:
            /* the wiimote disconnected */
            wiiuse_disconnected(sWiimotes[0]);
            change_control_mode(ControlMode::Off);
            break;

         default:
            break;
      }
   }

   if (sControlMode == ControlMode::Seeking)
   {
      clock_t current_time = clock();
      if (current_time > sTrackAge + CLOCKS_PER_SEC && sTrackValid)
      {
         sResult.steering_demand.x = 0;
         sResult.steering_demand.y = 0;
         sTrackValid = false;
         Log("Lost track"); 
      }
   }
   
   return sResult;
}
