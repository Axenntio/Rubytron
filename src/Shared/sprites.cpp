#include <Shared/sprites.hh>

unsigned char spr_cursor[SPR_CURSOR_HEIGHT] = {
	0b10000000,
	0b11000000,
	0b11100000,
	0b11110000,
	0b11000000,
	0b00100000
};

unsigned char spr_close_full[SPR_CLOSE_FULL_HEIGHT] = {
	0b10001000,
	0b01010000,
	0b00100000,
	0b01010000,
	0b10001000
};

unsigned char spr_maximise_full[SPR_CLOSE_FULL_HEIGHT] = {
	0b11111000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b11111000
};

unsigned char spr_close_minimal[SPR_CLOSE_MINIMAL_HEIGHT] = {
	0b10100000,
	0b01000000,
	0b10100000
};

unsigned char spr_maximise_minimal[SPR_CLOSE_MINIMAL_HEIGHT] = {
	0b11100000,
	0b10100000,
	0b11100000
};
