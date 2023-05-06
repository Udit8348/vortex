#pragma once

#include <iostream>
#include <iostream>
#include <bitset>
#include <cmath>

namespace vortex {
    class BrainFloat {
        private:
            void printBinary(int n, int i);

        public:
            union bfloat16 {
                float f;
                uint32_t t;
                struct {
                    uint16_t dead     : 16;   // unused in bfloat16
                    uint32_t mantissa : 7;    // Mantissa (fractional part) of the number
                    uint32_t exponent : 8;    // Exponent (power of 2) of the number
                    bool sign         : 1;    // Sign bit (bool is a 1-bit datatype)
                } parts;
            } in_mem;

            BrainFloat(float x);
            BrainFloat(uint32_t x);
            BrainFloat(uint8_t mantissa, uint8_t exponent, bool sign);
            
            void print(bfloat16 b);
            float to_float();
            float to_uint();
            void additive_inverse();

            friend BrainFloat operator-(const BrainFloat& a, const BrainFloat& b);
            friend BrainFloat operator+(const BrainFloat& a, const BrainFloat& b);
            friend BrainFloat operator*(const BrainFloat& a, const BrainFloat& b);
            friend BrainFloat operator/(const BrainFloat& a, const BrainFloat& b);

    };
} // namespace vortex
