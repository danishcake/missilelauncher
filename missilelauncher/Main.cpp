#include <iostream>
#include <time.h>
#include <Missile.h>
#include <Tracking.h>

int Log(const char* msg)
{
   std::cout << msg << std::endl;
   return 0;
}

#ifdef WIN32
void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
#endif

int main(int argc, char* argv[])
{
   int device_count = 0;
   InitRemote();
   if(device_count = InitialiseUSBControl(&Log))
   {
      bool exit = false;
      while(!exit)
      {
         ControlResult cr = TickRemote();
         exit |= cr.exit;

         if (cr.steering_demand.x < 0 && cr.steering_demand.y == 0)
         {
            PerformAction(Action::RotateLeft);
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y == 0)
         {
            PerformAction(Action::RotateRight);
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateUp);
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateDown);
         }

         if (cr.steering_demand.x < 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateLeftUp);
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateRightDown);
         }
         if (cr.steering_demand.x < 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateLeftDown);
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateRightUp);
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y == 0)
         {
            PerformAction(Action::Stop);
         }
         if (cr.fire)
         {
            PerformAction(Action::Fire);
         }
         usleep(1000 * 10);
      }
   }
   else
   {
      Log("Unable to open any devices");
   }


   ShutdownUSBControl();
   ShutdownRemote();
   return 0;
}

