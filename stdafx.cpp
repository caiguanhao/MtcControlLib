#include "stdafx.h"

#define CUT_SIZE (20)

std::string string_to_hex(const char *input, size_t len) {
    static const char *const lut = "0123456789ABCDEF";

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

void print_to_log(string prefix, const char *input, size_t len) {
#ifdef DEBUG
    cerr << prefix << string_to_hex(input, len > CUT_SIZE ? CUT_SIZE : len) << endl;
#endif
}