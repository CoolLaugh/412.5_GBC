#include "CommonDefinitions.h"

bool BitTest(byte value, int bit) {
	return (value & (0x01 << bit)) != 0;
}

bool BitTestReverse(byte value, int bit) {
	return (value & (0x80 >> bit)) != 0;
}

void BitSet(byte& value, int bit) {
	value |= (1 << bit);
}

void BitReset(byte& value, int bit) {
	value &= ~(1 << bit);
}

bool BitTest(word value, int bit) {
	return (value & (1 << bit)) != 0;
}

std::string ToHex(byte value) {

	std::vector<std::string> hex = { "0", "1", "2" , "3" , "4" , "5" , "6" , "7" , "8" , "9", "A", "B", "C", "D", "E", "F" };

	byte lowNibble = value & 0x0F;
	byte highNibble = value >> 4;

	return hex[highNibble] + hex[lowNibble];
}
