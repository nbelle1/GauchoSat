#include <imu.h>

void setup(void) 
{
  IMUinit();
}

void loop(void) 
{
  IMUdemo();
  toggleLED();
  delay(100);

}
