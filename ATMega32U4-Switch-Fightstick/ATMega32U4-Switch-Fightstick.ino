#define LUFA_LIB_USE
#include "Joystick.h"

int i = 0;
typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  X,
  Y,
  A,
  B,
  L,
  R,
  ZL,
  ZR,
  NOTHING
} Buttons_t;
typedef struct {
  Buttons_t button;
  uint16_t duration;
} command;

JoystickButtons_t step[] = {
  SWITCH_A,
  SWITCH_RELEASE,
  SWITCH_A,
  SWITCH_RELEASE,
  SWITCH_A,
  SWITCH_RELEASE
};

void setup() {
  SetupHardware();
  GlobalInterruptEnable();
}

void loop() {
  // We need to run our task to process and deliver data for our IN and OUT endpoints.
  HID_Task();
  // We also need to run the main USB management task.
  USB_USBTask();

  pressButton();
}
void pressButton()
{
  ReportData.Button = step[i++];
  delay(50);
  int len = (int)( sizeof(step) / sizeof(step[0])) - 1;
  if (i > len)i = 0;
  //ReportData.Button |= SWITCH_LCLICK;
  //ReportData.Button |= SWITCH_RCLICK;
}
