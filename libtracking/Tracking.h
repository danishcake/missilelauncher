#pragma once

typedef struct
{
   struct Tracking
   {
      float x;
      float y;
      float z;
   } steering_demand;

   bool fire;
   bool exit;
} ControlResult;


extern void InitRemote();
extern void ShutdownRemote();
extern ControlResult TickRemote();