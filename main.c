#include <stdio.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "math.h"

// Variables for Display
int ledCycle = 0;
int zero = 192UL;
int one = 249UL;
int two = 164UL;
int three = 176UL;
int four = 153UL;
int five = 146UL;
int six = 130UL;
int seven = 248UL;
int eight = 128UL;
int nine = 152UL;
int decimal = -128UL;
int digit_one = 256UL;
int digit_two = 512UL;
int digit_three = 1024UL;

// Variables for ADC Handler
int volts_hex = 0;
int volts_hex_shifted = 0;
int counter = 0;
int volts_sum = 0;
int volts_avg = 0;
int volts_digits[3] = {0, 0, 0};

/*------------------------------------------------------------------------------
  PIT Timer Configuration
 *------------------------------------------------------------------------------*/
__INLINE static void PIT_Config(void) {
	
	PIT->MCR  = (0UL << 1); 					// Start
	PIT->CHANNEL[0].LDVAL = 255999; // Create an interrupt every 255999 = 5 ms
	PIT->CHANNEL[0].TCTRL = (3UL); // Start interrupt
	PIT->CHANNEL[0].TFLG = (1UL); // Clear interrupt
}

/*------------------------------------------------------------------------------
  Handler for PIT Timer
 *------------------------------------------------------------------------------*/
void PIT0_IRQHandler(void)
{			
	const int convertToBCD[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

	long display_mask[] = {convertToBCD[volts_digits[0]] + decimal + digit_one, convertToBCD[volts_digits[1]] + digit_two, convertToBCD[volts_digits[2]] + digit_three};
	
	PIT->CHANNEL[0].TFLG = (1UL << 0); // Resets flag
		if (ledCycle == 0) {
			PTC->PCOR = (display_mask[1]); // Clear digit 2
			PTC->PCOR = (display_mask[2]); // Clear digit 3
			PTC->PDOR = (display_mask[0]); // Set digit 1
			++ledCycle;
		} else if (ledCycle == 1) {
			PTC->PCOR = (display_mask[0]); // Clear digit 1
			PTC->PCOR = (display_mask[2]); // Clear digit 3
			PTC->PDOR = (display_mask[1]); // Set digit 2
			++ledCycle;
		} else {
			PTC->PCOR = (display_mask[0]); // Clear digit 1
			PTC->PCOR = (display_mask[1]); // Clear digit 2
			PTC->PDOR = (display_mask[2]); // Set digit 3
			ledCycle = 0;
		}
}

/*------------------------------------------------------------------------------
  ADC Configuration
 *------------------------------------------------------------------------------*/
__INLINE static void ADC_Config(void) {
	ADC0->CFG1 = 0x7D;				// Enables bus clock/2, single-ended 16-bit conversion, long sample time, sets divide ratio and clock rate
  ADC0->CFG2 = 0x00; 				// Default longest sample time
	ADC0->SC2 = 0x40;				// Select hardware trigger
	ADC0->SC3 = 0x0C;				// Enables hardware average function and continuous conversion
	ADC0->SC1[0] = 0x4A;			// Selects AD10 as input
	PDB0->SC |= 0x00010000;			// Start PDB

	// DAC Configurations for Requirement 3
	DAC0->DAT[0].DATL = 0x0; 		// Clear register
	DAC0->DAT[0].DATH = 0x0; 		// Clear register
	DAC0->SR = 0x0;				// Clear register
	DAC0->C0 = 0x0;				// Clear register
	DAC0->C1 = 0x0;				// Clear register
	DAC0->C2 = 0x0F;				// Load data
	DAC0->C0 = 0xF0;				// Activate software trigger enable DAC system
	
}

/*------------------------------------------------------------------------------
  Handler for ADC
 *------------------------------------------------------------------------------*/
void ADC0_IRQHandler(void) {
	
		int volts_value;

		volts_hex = ADC0->R[0];                	  // Read input ADC value 16 bits
	
		// DAC calculations for Requirement 3
		volts_hex_shifted = (volts_hex >> 4);		// Convert to 12 bits        
		DAC0->DAT[0].DATL = volts_hex_shifted & 0XFF; 	// Store 8 to lower region
		volts_hex_shifted = volts_hex_shifted >> 8;	// Convert to 4 bits
		DAC0->DAT[0].DATH = volts_hex_shifted & 0XF;	// Store 4 to higher region
	
		volts_sum = volts_sum + volts_hex;
		counter++;
	
		if (counter == 0x1000)														// Count to 4096; 12 bits for 3 digits
		{
			volts_avg = volts_sum / 0x1000;   									// Calculate the average voltage
			volts_value = (volts_avg * 330) / (pow(2, 16) - 1); // Convert 16 bit ADC avg value to volts
			volts_digits[0] = (volts_value/100) % 10;
			volts_digits[1] = (volts_value/10) % 10;
			volts_digits[2] = (volts_value) % 10;
			
			// Reset values
			counter = 0;
			volts_sum = 0;
		}
	
}

/*------------------------------------------------------------------------------
  PDB Configuration
 *------------------------------------------------------------------------------*/
__INLINE static void PDB_Config(void) {
	
	PDB0->MOD = 0x1000;		// When the counter reaches this value, reset to zero
	PDB0->IDLY = 0x0;			// Enables interrupts
	PDB0->CH[0].C1 = 0x00000101; 	// Enables the PDB ADC pre trigger outputs
	PDB0->POEN = 0x01;			// Enables the pulse output
	PDB0->PODLY[0] = 0x0101000; 	// Pulse to one after 0x1000 pulses
	PDB0->SC = 0x00000F83; 		// Prescalar to 0 and mult factor to 1
 
}

/*------------------------------------------------------------------------------
  Handler for PDB
 *------------------------------------------------------------------------------*/
void PDB0_IRQHandler(void) {
	
	ADC0_IRQHandler();
	PDB0->SC &= 0xFFFFFFBF;  	// Clear the interrupt flag
	PDB0->CH[0].S = 0x0;			
	
}

/*------------------------------------------------------------------------------
  Main Function
 *------------------------------------------------------------------------------*/
int main (void)
{
  SystemCoreClockUpdate();               		 /* Get Core Clock Frequency */
	SIM->SCGC6 = (1UL << 23 | 1UL << 27 | 1UL << 22); // PIT, ADC and PDB clock enabled
	SIM->SCGC5 = (1UL << 11 | 1UL << 9);			// Enable Clock gate to Port A and C
	SIM->SOPT7 = 0x0; 																// PDB for ADC0
	SIM->SCGC2 = (1UL << 12); 												// Enable clock for DAC0

	for (int i = 0; i < 16; i++) {
		PORTC->PCR[i] = (1UL << 8);  			// Enable GPIO for PCR pins
	}
	PORTA->PCR[7] = 0x00; 					// Set pin as analog input 
	PTC->PDDR = (2047UL);					// Set pins PTC 0-10 to output
	
	PIT_Config();
	PDB_Config();
	ADC_Config();
	NVIC_EnableIRQ(PIT0_IRQn);				// Enable PIT handler
	NVIC->ISER[1] |= 0x02000000;
	NVIC->ISER[2] |= 0x00000100;
	
/*------------------------------------------------------------------------------
  Display loop
 *------------------------------------------------------------------------------*/
	while(1) 
	{
		
	}
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
