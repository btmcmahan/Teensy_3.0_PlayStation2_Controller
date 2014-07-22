#ifndef PS2_Teensy3_h
#define PS2_Teensy3_h

#include "mk20dx128.h"
#include "core_pins.h"

#define PS2T_CTAR0			0
#define PS2T_CTAR1			1
#define ChipSelect_0		0x01
#define ChipSelect_1		0x02
#define ChipSelect_2		0x04
#define ChipSelect_3		0x08
#define ChipSelect_4		0x10
#define ChipSelect_Alt0		0x81
#define ChipSelect_Alt1		0x82
#define ChipSelect_Alt2		0x84
#define ChipSelect_Alt3		0x88
#define PS2T_SPI_SR_TXCTR	0x0000f000 //Mask isolating the TXCTR

#define SELECT				0x0001 //bit  0
#define L3					0x0002 //bit  1
#define R3					0x0004 //bit  2
#define START				0x0008 //bit  3
#define PAD_UP				0x0010 //bit  4
#define PAD_RIGHT			0x0020 //bit  5
#define PAD_DOWN			0x0040 //bit  6
#define PAD_LEFT			0x0080 //bit  7
#define L2					0x0100 //bit  8
#define R2					0x0200 //bit  9
#define L1					0x0400 //bit 10
#define R1					0x0800 //bit 11
#define TRIANGLE			0x1000 //bit 12
#define CIRCLE				0x2000 //bit 13
#define CROSS				0x4000 //bit 14
#define SQUARE				0x8000 //bit 15

#define PS2T_SPI_WRITE_8(c,CTARn, PCS) \	
	do { \
		while ((SPI0_SR & PS2T_SPI_SR_TXCTR) >= 0x00004000); \
		SPI0_PUSHR = ((c)&0xff)   | SPI_PUSHR_CTAS(CTARn) | SPI_PUSHR_PCS(0x1f & PCS); \
	} while(0)
	
// with Continuous assertion of ChipSelect pin
#define PS2T_SPI_WRITE_8c(c,CTARn, PCS) \ 
	do { \
		while ((SPI0_SR & PS2T_SPI_SR_TXCTR) >= 0x00004000); \
		SPI0_PUSHR = ((c)&0xff) | SPI_PUSHR_CONT | SPI_PUSHR_CTAS(CTARn) | SPI_PUSHR_PCS(0x1f & PCS); \
	} while(0)

//PS2 Controller Request Types
static uint8_t digital[]={5, 0x01,0x42,0x00,0xFF,0xFF};
static uint8_t digital_analog[]={9, 0x01,0x42,0x00,0xFF,0xFF,0x5A,0x5A,0x5A,0x5A};
static uint8_t digital_analog_pressures[]={21, 0x01,0x42,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
										   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static uint8_t enter_config[]={5, 0x01,0x43,0x00,0x01,0x00};
static uint8_t analog_mode[]={9, 0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
static uint8_t digital_mode[]={9, 0x01,0x44,0x00,0x00,0x03,0x00,0x00,0x00,0x00};
static uint8_t return_all_pressures_mode[]={9, 0x01,0x4F,0x00,0xFF,0xFF,0x03,0x00,0x00,0x00};
static uint8_t exit_config[]={9, 0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
static uint8_t enable_rumble[]={5, 0x01,0x4D,0x00,0x00,0x01};
static uint8_t status[]={9, 0x01,0x45,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};


class PS2_Teensy3 {
public:
PS2_Teensy3();
void begin(bool PS2T_CTARn, uint8_t PS2T_CS);
uint8_t PS2T_ChipSelect;
void Request(uint8_t *requestType);
void poll_Digital();
void poll_DigitalWithHats();
void poll_DigitalWithAllAnalog();
uint8_t ControllerMode;
uint8_t ControllerType;
uint8_t	L_JOY_X;
uint8_t	L_JOY_Y;
uint8_t	R_JOY_X;
uint8_t	R_JOY_Y;
uint8_t	ANALOG_RIGHT;
uint8_t	ANALOG_LEFT;
uint8_t	ANALOG_UP;
uint8_t	ANALOG_DOWN;
uint8_t	ANALOG_TRIANGLE;
uint8_t	ANALOG_CIRCLE;
uint8_t	ANALOG_CROSS;
uint8_t	ANALOG_SQUARE;
uint8_t	ANALOG_L1;	
uint8_t	ANALOG_R1;
uint8_t	ANALOG_L2;
uint8_t	ANALOG_R2;

//private:
void processButtonData();
bool ButtonPressed(uint16_t buttonMask);
bool ButtonRise(uint16_t buttonMask);
bool ButtonFall(uint16_t buttonMask);
uint8_t returnData[22];
uint16_t ButtonData;
uint16_t prevButtonData;

};

#endif /* PS2_Teensy3_h */
