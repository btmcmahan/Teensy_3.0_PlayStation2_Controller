
#include "PS2_Teensy3.h"
//#include "t3spi.h"

PS2_Teensy3::PS2_Teensy3() {
	SIM_SCGC6 |= SIM_SCGC6_SPI0;	// enable clock to SPI.
}

void PS2_Teensy3::begin(bool PS2T_CTARn, uint8_t PS2T_CS){
	prevButtonData = 0;

	PS2T_ChipSelect = PS2T_CS;
	SPI0_MCR |= SPI_MCR_HALT | SPI_MCR_MDIS;
	SPI0_MCR=0x80000000;
	CORE_PIN13_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set SCK  to pin 13
	CORE_PIN11_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set MOSI to pin 11
	CORE_PIN12_CONFIG = PORT_PCR_MUX(2);					//Set MISO to pin 12
	
	if (PS2T_CS   == 0x01){
		CORE_PIN10_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin10 Output & CS0
		SPI0_MCR |= 0x00010000;}
	if (PS2T_CS   ==  0x02){
		CORE_PIN9_CONFIG  = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin9  Output & CS1
		SPI0_MCR |= 0x00020000;}
	if (PS2T_CS   == 0x04){
		CORE_PIN20_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin20 Output & CS2
		SPI0_MCR |= 0x00040000;}
	if (PS2T_CS   == 0x08){
		CORE_PIN21_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin21 Output & CS3
		SPI0_MCR |= 0x00080000;}
	if (PS2T_CS   == 0x10){
		CORE_PIN15_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin15 Output & CS4
		SPI0_MCR |= 0x00100000;}
	if (PS2T_CS   ==  0x81){
		CORE_PIN2_CONFIG  = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin2  Output & (alt) CS0
		SPI0_MCR |= 0x00010000;}
	if (PS2T_CS   ==  0x82){
		CORE_PIN6_CONFIG  = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin6  Output & (alt) CS1
		SPI0_MCR |= 0x00020000;}
	if (PS2T_CS   == 0x84){
		CORE_PIN23_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin23 Output & (alt) CS2
		SPI0_MCR |= 0x00040000;}
	if (PS2T_CS   == 0x88){
		CORE_PIN22_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);		//Set Pin22 Output & (alt) CS3
		SPI0_MCR |= 0x00080000;}
	
	if (PS2T_CTARn == 0){
		SPI0_CTAR0=0;
		SPI0_CTAR0 |= SPI_CTAR_FMSZ(7);
		SPI0_CTAR0 = SPI0_CTAR0 & ~(SPI_CTAR_CPOL | SPI_CTAR_CPHA) | 0x03 << 25;
		SPI0_CTAR0 |= SPI_CTAR_LSBFE;
		SPI0_CTAR0 |= SPI_CTAR_DBR | SPI_CTAR_CSSCK(0b1000) | SPI_CTAR_BR(0b1000);
		}
		
	if (PS2T_CTARn == 1){
		SPI0_CTAR1=0;
		SPI0_CTAR1 |= SPI_CTAR_FMSZ(7);
		SPI0_CTAR1 = SPI0_CTAR1 & ~(SPI_CTAR_CPOL | SPI_CTAR_CPHA) | 0x03 << 25;
		SPI0_CTAR0 |= SPI_CTAR_LSBFE;
		SPI0_CTAR1 |= SPI_CTAR_DBR | SPI_CTAR_CSSCK(0b1000) | SPI_CTAR_BR(0b1000);
		}
		
	SPI0_MCR &= ~SPI_MCR_HALT & ~SPI_MCR_MDIS;
}

void PS2_Teensy3::Request(uint8_t *requestType){
	int spi_pos = 0;
	int spi_popr_pos = 0;
	
	for (int i=1; i<= requestType[0]; i++){
		if (i != requestType[0]){
			PS2T_SPI_WRITE_8c(requestType[i], PS2T_CTAR0, PS2T_ChipSelect);
		}
		if (i == requestType[0]){
			PS2T_SPI_WRITE_8(requestType[i], PS2T_CTAR0, PS2T_ChipSelect);
		}
		
		spi_pos++;
		spi_popr_pos++;
		
		if ((spi_popr_pos == 4) || (spi_pos == requestType[0])){
			delayMicroseconds(75);
			spi_popr_pos--;
			returnData[(spi_pos - spi_popr_pos)] = SPI0_POPR;
			while (spi_popr_pos > 0){
				spi_popr_pos--;
				delayMicroseconds(75);
				returnData[(spi_pos - spi_popr_pos)] = SPI0_POPR;
			}
		}
		delayMicroseconds(10);
	}
	ControllerMode = returnData[2];
	
	if ((ControllerMode == 0x41) || (ControllerMode==0x73) || (ControllerMode==0x79)){ 
		processButtonData();
	}
	
	if (requestType[2] == 0x45){
		ControllerType = returnData[4];
	}
}



void PS2_Teensy3::processButtonData(){
	prevButtonData = ButtonData;
	uint16_t buttonMerge = ((returnData[5] << 8) | returnData[4]);
	ButtonData = ~buttonMerge;
	
	if ((ControllerMode==0x73) || (ControllerMode==0x79)){
		R_JOY_X = returnData[6];
		R_JOY_Y = returnData[7];
		L_JOY_X = returnData[8];
		L_JOY_Y = returnData[9];
	}
	
	if (ControllerMode==0x79){
		ANALOG_RIGHT	= returnData[10];
		ANALOG_LEFT		= returnData[11];
		ANALOG_UP		= returnData[12];
		ANALOG_DOWN		= returnData[13];
		ANALOG_TRIANGLE	= returnData[14];
		ANALOG_CIRCLE	= returnData[15];
		ANALOG_CROSS	= returnData[16];
		ANALOG_SQUARE	= returnData[17];
		ANALOG_L1		= returnData[18];	
		ANALOG_R1		= returnData[19];
		ANALOG_L2		= returnData[20];
		ANALOG_R2		= returnData[21];
	}
}

bool PS2_Teensy3::ButtonPressed(uint16_t buttonMask){
	if (ButtonData & buttonMask){
		return(1);}
	else {
		return(0);}
}

bool PS2_Teensy3::ButtonRise(uint16_t buttonMask){
	if ((ButtonData & buttonMask) && !(prevButtonData & buttonMask)){
		return(1);}
	else {
		return(0);}
}

bool PS2_Teensy3::ButtonFall(uint16_t buttonMask){
	if (!(ButtonData & buttonMask) && (prevButtonData & buttonMask)){
		return(1);}
	else {
		return(0);}
}