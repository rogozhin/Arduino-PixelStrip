#include "PixelStrip.h"

PixelStrip::PixelStrip (uint8_t pixels) {
	params.pixels = pixels;
	params.pin = PS_PIXEL_PORT;
	params.pinBit = PS_PIXEL_BIT;
	this.setup();
}

PixelStrip::PixelStrip (uint8_t pixels, uint8_t pin) {
	params.pixels = pixels;
	params.pin = pin;
	params.pinBit = PS_PIXEL_BIT;
	this.setup();
}

PixelStrip::PixelStrip (uint8_t pixels, uint8_t pin, uint8_t pinBit) {
	params.pixels = pixels;
	params.pin = pin;
	params.pinBit = pinBit;
	this.setup();
}

void PixelStrip::sendBit (bool bitVal) {
	long onCycles;
	long offCycles;

	if (bitVal) {
		// 1 bit
		onCycles = NS_TO_CYCLES(PS_T1H) - 2;
		offCycles = NS_TO_CYCLES(PS_T1L) - 2;
	} else {
		// 0 bit
		onCycles = NS_TO_CYCLES(PS_T0H) - 2;
		offCycles = NS_TO_CYCLES(PS_T0L) - 2;
	}

	asm volatile (
		"sbi %[port], %[bit] \n\t" // Set the output bit
		".rept %[onCycles] \n\t" // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
		"nop \n\t"
		".endr \n\t"
		"cbi %[port], %[bit] \n\t" // Clear the output bit
		".rept %[offCycles] \n\t"
		"nop \n\t"
		".endr \n\t"
		::
		[port]    "I" (_SFR_IO_ADDR(params.pin)),
		[bit]   "I" (params.pinBit),
		[onCycles]  "I" (onCycles),
		[offCycles] "I" (offCycles)
	);
}

void PixelStrip::sendByte (uint8_t byte) {
	for (unsigned char bit = 0; bit < 8; bit++) {
		// WS2812 wants bit in highest-to-lowest order
		sendBit(bitRead(byte, 7));
		byte <<= 1;
	}
}

void PixelStrip::sendPixel (uint8_t r, uint8_t g, uint8_t b) {
	// WS2812 wants colors in green then red then blue order
	this.sendByte(g);
	this.sendByte(r);
	this.sendByte(b);
}

void PixelStrip::show () {
	_delay_us((TS_RES / 1000UL) + 1);
}

void PixelStrip::setup () {
	bitSet(PIXEL_DDR, params.pinBit);
}
