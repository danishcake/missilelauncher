
#include "Tracking.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

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
static wiimote**           sWiimotes = 0;
static int                 sTrackAge = 100;

void change_control_mode(ControlMode::Enum cm)
{
   if (cm == ControlMode::Seeking && sControlMode != ControlMode::Seeking)
   {
      Log("Switching to seeker mode");
      sControlMode = ControlMode::Seeking;
   }

   if (cm == ControlMode::Slaved && sControlMode != ControlMode::Slaved)
   {
      Log("Switching to slaved mode");
      sControlMode = ControlMode::Slaved;
   }
   
   if (cm == ControlMode::Off && sControlMode != ControlMode::Off)
   {
      Log("Switching to off mode");
      sControlMode = ControlMode::Off;
   }
}

void handle_event(struct wiimote_t* wm)
{
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
         sResult.steering_demand.x = 512 - (float)wm->ir.x;
         sResult.steering_demand.y = 384 - (float)wm->ir.y;
         sResult.steering_demand.z = -1;
         // Dead zone of 100 (20%)
         if (fabs(sResult.steering_demand.x) < 100) sResult.steering_demand.x = 0;
         if (fabs(sResult.steering_demand.y) < 100) sResult.steering_demand.y = 0;
         sTrackAge = 0;

         printf("%d dots (%d, %d, %d)\n", wm->ir.num_dots, wm->ir.x, wm->ir.y, wm->ir.z); 
      }

      sTrackAge++;
      if (sTrackAge == 50)
      {
         sResult.steering_demand.x = 0;
         sResult.steering_demand.y = 0;
         printf("Lost track"); 
      }
   }

}

void InitRemote()
{
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
            wiiuse_set_ir(sWiimotes[0], 1);
            wiiuse_set_ir_sensitivity(sWiimotes[0], 5);
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
   
   return sResult;
}
