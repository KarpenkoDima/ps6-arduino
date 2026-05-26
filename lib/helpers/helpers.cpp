#include "helpers.h"

byte step_up(byte value, byte max_exclusive) {
    return (value + 1) % max_exclusive;
}

byte step_down(byte value, byte max_exclusive) {
    if (value == 0) return max_exclusive - 1;
    return value - 1;
}
