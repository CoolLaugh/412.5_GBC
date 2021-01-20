#include "CommonDefinitions.h"

bool BitTest(byte value, int bit) {
	return (value & (1 << bit)) != 0;
}
