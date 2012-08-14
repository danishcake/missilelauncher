#include <iostream>
#include <time.h>
#include <Missile.h>
#include <Tracking.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>

int Log(const char* msg)
{
   std::cout << msg << std::endl;
   return 0;
}

#ifdef WIN32
void msleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
#endif


static bool last_action_stop = false;
int main(int argc, char* argv[])
{

   /* Launch as daemon */
   pid_t pid, sid;
   pid = fork();
   if (pid < 0)
   {
      exit(EXIT_FAILURE);
   }
   if (pid > 0)
   {
      exit(EXIT_SUCCESS);
   }
   umask(0);
   sid = setsid();
   if (sid < 0) 
   {
      /* Log any failure */
      exit(EXIT_FAILURE);
   }


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
            last_action_stop = false;
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y == 0)
         {
            PerformAction(Action::RotateRight);
            last_action_stop = false;
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateUp);
            last_action_stop = false;
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateDown);
            last_action_stop = false;
         }

         if (cr.steering_demand.x < 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateLeftUp);
            last_action_stop = false;
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateRightDown);
            last_action_stop = false;
         }
         if (cr.steering_demand.x < 0 && cr.steering_demand.y > 0)
         {
            PerformAction(Action::RotateLeftDown);
            last_action_stop = false;
         }
         if (cr.steering_demand.x > 0 && cr.steering_demand.y < 0)
         {
            PerformAction(Action::RotateRightUp);
            last_action_stop = false;
         }
         if (cr.steering_demand.x == 0 && cr.steering_demand.y == 0 && !last_action_stop)
         {
            last_action_stop = true;
            PerformAction(Action::Stop);
         }
         if (cr.fire)
         {
            PerformAction(Action::Fire);
            last_action_stop = false;
         }
         //usleep(1000 * 50);
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

