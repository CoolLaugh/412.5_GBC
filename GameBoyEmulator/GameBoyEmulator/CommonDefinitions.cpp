#include "CommonDefinitions.h"

bool BitTest(byte value, int bit) {
	return (value & (1 << bit)) != 0;
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