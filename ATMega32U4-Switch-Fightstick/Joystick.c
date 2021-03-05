#include "Joystick.h"

#define ECHO_TIMES 3
#define ECHO_INTERVAL 2

volatile uint8_t echo_ms = 0;   // echo counter

USB_JoystickReport_Input_t ReportData;
void EmptyReport(void)
{
  ReportData.LX = STICK_CENTER;
  ReportData.LY = STICK_CENTER;
  ReportData.RX = STICK_CENTER;
  ReportData.RY = STICK_CENTER;
  ReportData.HAT = HAT_CENTER;
}
// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
  // We need to disable watchdog if enabled by bootloader/fuses.
  MCUSR &= ~(1 << WDRF);
  wdt_disable();
  // We need to disable clock division before initializing the USB hardware.
  clock_prescale_set(clock_div_1);
  // Initialize timer interrupt
  TIMSK0 |= (1 << TOIE0);
  //enable interrupts
  sei();
  // set prescaler to 64 and start the timer
  TCCR0B |= (1 << CS01) | (1 << CS00);
  // Initialize report
  EmptyReport();
  // We can then initialize our hardware and peripherals, including the USB stack.
  // The USB stack should be initialized last.
  USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
  // We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
  // We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
  bool ConfigSuccess = true;

  // We setup the HID report endpoints.
  ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
  ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

  // We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {}
void EVENT_USB_Device_ControlRequest1(void) {
  // We can handle two control requests: a GetReport and a SetReport.
  switch (USB_ControlRequest.bRequest)
  {
    // GetReport is a request for data from the device.
    case HID_REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        // We'll create an empty report.
        USB_JoystickReport_Input_t JoystickInputData;
        // We'll then populate this report with what we want to send to the host.
        GetNextReport(&JoystickInputData);
        // Since this is a control endpoint, we need to clear up the SETUP packet on this endpoint.
        Endpoint_ClearSETUP();
        // Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
        Endpoint_Write_Control_Stream_LE(&JoystickInputData, sizeof(JoystickInputData));
        // We then acknowledge an OUT packet on this endpoint.
        Endpoint_ClearOUT();
      }

      break;
    case HID_REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        // We'll create a place to store our data received from the host.
        USB_JoystickReport_Output_t JoystickOutputData;
        // Since this is a control endpoint, we need to clear up the SETUP packet on this endpoint.
        Endpoint_ClearSETUP();
        // With our report available, we read data from the control stream.
        Endpoint_Read_Control_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData));
        // We then send an IN packet on this endpoint.
        Endpoint_ClearIN();
      }

      break;
  }
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
  // If the device isn't connected and properly configured, we can't do anything here.
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;
  // [Optimized] We don't need to receive data at all.
  if (true) {
    // We'll start with the OUT endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
    // We'll check to see if we received something on the OUT endpoint.
    if (Endpoint_IsOUTReceived())
    {
      // If we did, and the packet has data, we'll react to it.
      if (false && Endpoint_IsReadWriteAllowed())
      {
        // We'll create a place to store our data received from the host.
        USB_JoystickReport_Output_t JoystickOutputData;
        // We'll then take in that data, setting it up in our storage.
        while (Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
        // At this point, we can react to this data.
        // However, since we're not doing anything with this data, we abandon it.
      }
      // Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
      Endpoint_ClearOUT();
    }
  }
  // [Optimized] Only send data when changed.
  if (echo_ms == 0)
  {
    // We'll then move on to the IN endpoint.
    Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
    // We first check to see if the host is ready to accept data.
    if (Endpoint_IsINReady())
    {
      // We'll create an empty report.
      USB_JoystickReport_Input_t JoystickInputData;
      // We'll then populate this report with what we want to send to the host.
      GetNextReport(&JoystickInputData);
      // Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
      if (Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) == ENDPOINT_RWSTREAM_NoError) {
        // We then send an IN packet on this endpoint.
        Endpoint_ClearIN();
        /* Clear the report data afterwards */
        // set interval
        //echo_ms = ECHO_INTERVAL;
      }
    }
  }
}
// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const LReportData) {
  //memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
  //EmptyReport();
  //if (echo_ms != 0) echo_ms--;
  memcpy(LReportData, &ReportData, sizeof(USB_JoystickReport_Input_t));
}
