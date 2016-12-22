//******************************************************************************
// PixelStrip
// Version 0.0.1 December, 2016
// Copyright 2016 Lev Rogozhin
//
// Pixel manipulation code based on SimpleNeoPixelDemo by bigjosh
// https://github.com/bigjosh/SimpleNeoPixelDemo
//******************************************************************************

#ifndef PixelStrip_h
#define PixelStrip_h

//------------------------------------------------------------------------------
// Include the right Arduino header
//
#if defined(ARDUINO) && (ARDUINO >= 100)
#	include <Arduino.h>
#else
#	if !defined(IRPRONTO)
#		include <WProgram.h>
#	endif
#endif

//------------------------------------------------------------------------------
// These values depend on which pin your string is connected to and what board you are using 
// More info on how to find these at http://www.arduino.cc/en/Reference/PortManipulation
// These values are for the pin that connects to the Data Input pin on the LED strip. They correspond to...
//
// Arduino Yun:     Digital Pin 8
// DueMilinove/UNO: Digital Pin 12
// Arduino MeagL    PWM Pin 4
//
// You'll need to look up the port/bit combination for other boards. 
//------------------------------------------------------------------------------

#define PS_PIXEL_PORT PORTB  // Port of the pin the pixels are connected to
#define PS_PIXEL_DDR DDRB   // Port of the pin the pixels are connected to
#define PS_PIXEL_BIT 4      // Bit of the pin the pixels are connected to


//------------------------------------------------------------------------------
// These are the timing constraints taken mostly from the WS2812 datasheets
// These are chosen to be conservative and avoid problems rather than for maximum throughput 
//------------------------------------------------------------------------------

#define PS_T1H 800 // Width of a 1 bit in ns
#define PS_T1L 450 // Width of a 1 bit in ns
#define PS_T0H 400 // Width of a 0 bit in ns
#define PS_T0L 850 // Width of a 0 bit in ns
#define PS_RES 50000 // Width of the low gap between bits to cause a frame to latch
// Minimum gap to latch defined in datashit as >50000 ns, but 30000 works good...

//------------------------------------------------------------------------------
// CPU Frequency
//
#ifdef F_CPU
#	define SYSCLOCK F_CPU // main Arduino clock
#else
#	define SYSCLOCK 16000000 // main Arduino clock
#endif

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_CYCLE (1000000000L / SYSCLOCK)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

//------------------------------------------------------------------------------
// Main class for strip manipulate
//
class PixelStrip {
		uint16_t pixels;
	public:
		PixelStrip (uint16_t pixels);

		void sendBit (bool bitVal);
		void sendByte (uint8_t byte);
		void sendPixel (uint8_t r, uint8_t g, uint8_t b);
		void show ();

	private:
		void setup ();
};

#define SEND_BIT(a_on, a_off) \
	asm volatile ( \
		"sbi %[port], %[bit] \n\t" \
		".rept %[on] \n\t" \
		"nop \n\t" \
		".endr \n\t" \
		"cbi %[port], %[bit] \n\t" \
		".rept %[off] \n\t" \
		"nop \n\t" \
		".endr \n\t" \
		:: \
		[port] "I" (_SFR_IO_ADDR(PORTB)), \
		[bit] "I" (PS_PIXEL_BIT), \
		[on]  "M" (NS_TO_CYCLES(a_on) - 2), \
		[off] "M" (NS_TO_CYCLES(a_off) - 2) \
	)

#endif
