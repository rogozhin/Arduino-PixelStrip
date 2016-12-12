#include "PixelStrip.h"

PixelStrip::PixelStrip (uint16_t pixels) {
	this->pixels = pixels;
	this->setup();
}

void PixelStrip::sendBit (bool bitVal) {
	if (bitVal) {
		// 1 bit
		SEND_BIT(PS_T1H, PS_T1L);
	} else {
		// 0 bit
		SEND_BIT(PS_T0H, PS_T0L);
	}
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
	this->sendByte(g);
	this->sendByte(r);
	this->sendByte(b);
}

void PixelStrip::show () {
	cli();
	delayMicroseconds((PS_RES / 1000UL) + 1);
	sei();
}

void PixelStrip::setup () {
	bitSet(PS_PIXEL_DDR, PS_PIXEL_BIT);
}
