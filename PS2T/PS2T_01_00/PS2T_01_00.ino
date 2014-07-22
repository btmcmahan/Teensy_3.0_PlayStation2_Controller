
/*
This is a good resource for anyone who wants to modify this library:
http://store.curiousinventor.com/guides/PS2/

Teensy 3.0 Pin           PS2 Controller                   Power Supply
----------------------------------------------------------------------
MISO (Pin 12) <--------- Data  (Brown,  Pin 1)
MOSI (Pin 11) ---------> Cmd   (Orange, Pin 2)
                         Vib   (Grey,   Pin 3) <--------- 7.2v - 9v
                         Gnd   (Black,  Pin 4) <--------- Ground/Com
                         Pow   (Red,    Pin 5) <--------- 3.3v
CS 0 (Pin 10) ---------> Attn  (Yellow, Pin 6)
SCK  (Pin 13) ---------> Clock (Blue,   Pin 7)
                         Unused(White,  Pin 8)
                         Unused(Green,  Pin 9)
----------------------------------------------------------------------
*/

#include <t3spi.h>
#include <PS2_Teensy3.h>

PS2_Teensy3 ps2T;

void setup(){
  Serial.begin(115200);
 
   ps2T.begin(PS2T_CTAR0, ChipSelect_0);
  delay(50);
    
  //Note that some requests seem to need time to process,
  //thats why there are 200ms delays between commands.
  
  ps2T.Request(enter_config);          //Enter Config Mode
  delay(200);
  
  ps2T.Request(status);                //Request Controller Status
  delay(200);
  
  //********************************************************************************
  //Set the controller to use one of the 3 operating modes:
  //ps2T.Request(digital_mode);             //Set Digital Mode
  //ps2T.Request(analog_mode);              //Set Analog Mode, Lock Analog button
  ps2T.Request(return_all_pressures_mode);  //Set to return all button pressures
  delay(200); 
  //********************************************************************************
  
  ps2T.Request(exit_config);           //Exit Config mode
  delay(200);
  
  Serial.print("Controller Type: ");   
  Serial.println(ps2T.ControllerType); //print Controller Type
  Serial.println();
}


void loop(){
  
  //********************************************************************************
  //Step 1: Request data from the controller, this
  //        request should match the controllers operating mode,
  //        as previously set in the SETUP function.  However,
  //        if you can request less date than what is available
  //        from the from the operation mode.  For example, you
  //        can request digital while in analog mode, but not 
  //        analog while in digital mode.
  
  //        Requesting more data equal more spi transfer time,
  //        So only request the data you need, to save time.
    
  //Digital (mode) just reads the digital state of each button.
  //(5 bytes transferred per controller scan)
  //ps2T.Request(digital); 

  //digital_analog (mode) reads the digital state of each button,
  //as well as the analog joysticks, in 8 bit (0 to 255).
  //(9 bytes tranferred per controller scan)
  //ps2T.Request(digital_analog);
  
  //digital_analog_pressures (mode) reads the digital state of each button, 
  //as well as the analog joysticks, and the analog pressure of each button.
  //(21 bytes tranferred per controller scan)
  ps2T.Request(digital_analog_pressures);
  
//********************************************************************************
  //Step 2: Use the data returned from the controller.
  //        Here are some examples of how to use the data:

    Serial.print("R_X: ");
    Serial.print(ps2T.R_JOY_X);
    Serial.print("  R_Y:"); 
    Serial.println(ps2T.R_JOY_Y);
   
    Serial.print("L_X: ");
    Serial.print(ps2T.L_JOY_X);
    Serial.print("  L_Y:"); 
    Serial.println(ps2T.L_JOY_Y);
    
    if (ps2T.ButtonPressed(L1)){
      Serial.println("L1 pressed");}
    
    if (ps2T.ButtonPressed(L2)){
      Serial.print("L2 pressure: "); 
      Serial.println(ps2T.ANALOG_L2);}
      
    if (ps2T.ButtonPressed(L3)){
      Serial.println("L3 pressed");}
      
    if (ps2T.ButtonPressed(R1)){
      Serial.println("R1 pressed");}
    
    if (ps2T.ButtonPressed(R2)){
      Serial.println("R2 pressed");}
      
    if (ps2T.ButtonPressed(R3)){
      Serial.println("R3 pressed");}
    
    if (ps2T.ButtonPressed(PAD_UP)){
      Serial.print("UP pressure: "); 
      Serial.println(ps2T.ANALOG_UP);}
      
    if (ps2T.ButtonPressed(PAD_RIGHT)){
      Serial.println("Right pad pressed");}
      
    if (ps2T.ButtonPressed(PAD_DOWN)){
      Serial.println("Down pad pressed");}
    
    if (ps2T.ButtonPressed(PAD_LEFT)){
      Serial.println("Left pad pressed");}
    
    if (ps2T.ButtonPressed(CIRCLE)){
      Serial.print("CIRCLE pressure: "); 
      Serial.println(ps2T.ANALOG_CIRCLE);}
    
    if (ps2T.ButtonPressed(CROSS)){
      Serial.println("Cross Pressed");}
      
    if (ps2T.ButtonRise(SQUARE)){
      Serial.println("Square Rising Edge");}
    
    if (ps2T.ButtonFall(TRIANGLE)){
      Serial.println("Triangle Falling Edge");}

    if (ps2T.ButtonPressed(START)){
      Serial.println("Start Pressed");}

    if (ps2T.ButtonRise(SELECT)){
      Serial.println("Changing Modes");
      switchModes();}
      
      
    Serial.println();
    delay(200);
}  

void switchModes(){
  delay(200);
  ps2T.Request(enter_config);
  delay(200);
  if (ps2T.ControllerMode == 0x41){
    ps2T.Request(analog_mode);
    Serial.println("Entering Analog Mode");
  }
  if (ps2T.ControllerMode == 0x73){
    Serial.println("Returning All Pressures Mode");
    ps2T.Request(return_all_pressures_mode);
  }
  if (ps2T.ControllerMode == 0x79){
    ps2T.Request(digital_mode);
    Serial.println("Entering Digital Mode");
  }
  delay(200);
  ps2T.Request(exit_config);
  delay(500);
}
