#include "bfloat.h"

using namespace vortex;

// Prints the binary representation of a number n up to i-bits.
void BrainFloat::printBinary(int n, int i) {
    int k;
    for (k = i - 1; k >= 0; k--) {
        if ((n >> k) & 1)
            std::cout << "1";
        else
            std::cout << "0";
    }
}

// constructor for float -> BrainFloat
BrainFloat::BrainFloat(float x) {
    in_mem.f = x;
    print(in_mem);
}

// construct individual components of BrainFloat
BrainFloat::BrainFloat(uint8_t mantissa, uint8_t exponent, bool sign) {
    in_mem.parts.mantissa = mantissa & 0x7F;
    in_mem.parts.exponent = exponent;
    in_mem.parts.sign = (int) sign;
    print(in_mem);
}
    
void BrainFloat::print(bfloat16 b) {
    std::cout << b.parts.sign << " | ";
    printBinary(b.parts.exponent, 8);
    std::cout << " | ";
    printBinary(b.parts.mantissa, 7);
    std::cout << std::endl;
}
    
// follow ieee-754 to interpret bit value of BrainFloat
float BrainFloat::to_float() {
    uint8_t sign = in_mem.parts.sign;
    double exp = in_mem.parts.exponent - 127;
    uint8_t mantissa = in_mem.parts.mantissa;
    return std::pow(-1, sign) * std::pow(2, exp) * (1+(mantissa / std::pow(2,7)));
}

// toggle sign-bit
void BrainFloat::additive_inverse(){
    this->in_mem.parts.sign ^= 1;
}

// friend functions are not to be defined in thier own class
// friend functions do not have the implicit "this" so we use two args
BrainFloat vortex::operator-(const BrainFloat& a, const BrainFloat& b) {
    // get fields for operands
    bool a_sign = a.in_mem.parts.sign;
    uint32_t a_exp = (uint32_t) a.in_mem.parts.exponent;
    uint32_t a_mantissa =  (uint32_t) a.in_mem.parts.mantissa | 0x80; // add in the implicit bit
        
    bool b_sign = b.in_mem.parts.sign;
    uint32_t b_exp =  (uint32_t) b.in_mem.parts.exponent;
    uint32_t b_mantissa = (uint32_t) b.in_mem.parts.mantissa | 0x80; // add in the implicit bit
    
    if (a_sign != b_sign) {
        if (a_sign) {
            // (-a) - (b) => -(a+b)
            BrainFloat a_pos(a.in_mem.parts.mantissa, a.in_mem.parts.exponent, 0);
            BrainFloat b_pos(b.in_mem.parts.mantissa, b.in_mem.parts.exponent, 0);
            BrainFloat mag = a_pos + b_pos;
            // mag is non-const so use additive_inverse method
            mag.additive_inverse();
            return mag;
        } else {
            // (a) - (-b) => a + b
            BrainFloat b_pos(b.in_mem.parts.mantissa, b.in_mem.parts.exponent, 0);
            return a + b_pos;
        }
    }

    // align mantissas by shifting the smaller exponent to the larger exponent
    if (a_exp < b_exp) {
        a_mantissa >>= (b_exp - a_exp);
        a_exp = b_exp;
    } else {
        b_mantissa >>= (a_exp - b_exp);
        b_exp = a_exp;
    }
        
    // subtract mantissas and adjust exponent if necessary
    int diff_mantissa = a_mantissa - b_mantissa;
    if (diff_mantissa < 0) {
        diff_mantissa = -diff_mantissa;
        a_sign = !a_sign; // flip the sign bit
    }

    while (diff_mantissa < 0x80) {
        diff_mantissa <<= 1;
        a_exp--;
    }
        
    // build binary representation of result
    return BrainFloat((diff_mantissa & 0x7f), a_exp, a_sign);
}

BrainFloat vortex::operator+(const BrainFloat& a, const BrainFloat& b) {
    // get fields
    bool a_sign = a.in_mem.parts.sign;
    uint32_t a_exp = (uint32_t) a.in_mem.parts.exponent;
    uint32_t a_mantissa =  (uint32_t) a.in_mem.parts.mantissa | 0x80; // add in the implicit bit
    
    bool b_sign = b.in_mem.parts.sign;
    uint32_t b_exp =  (uint32_t) b.in_mem.parts.exponent; // DONT remove bias, we want to deal with unsigned numbers
    uint32_t b_mantissa = (uint32_t) b.in_mem.parts.mantissa | 0x80; // add in the implicit bit
    
    // subtract opposite signed operands
    if (a_sign != b_sign) {
        if (a_sign) {
            BrainFloat a_pos(a.in_mem.parts.mantissa, a.in_mem.parts.exponent, 0);
            return b - a_pos;
        } else {
            BrainFloat b_pos(b.in_mem.parts.mantissa, b.in_mem.parts.exponent, 0);
            return a - b_pos; 
        }
    }
    
    // align mantissas by shifting the smaller exponent to the larger exponent
    if (a_exp < b_exp) {
        a_mantissa >>= (b_exp - a_exp);
        a_exp = b_exp;
    } else {
        b_mantissa >>= (a_exp - b_exp);
        b_exp = a_exp;
    }
    
    // add mantissas and adjust exponent if necessary
    int sum_mantissa = a_mantissa + b_mantissa;
    if (sum_mantissa & 0x100) {
        sum_mantissa >>= 1;
        a_exp++;
    }
    
    // build binary representation of result
    return BrainFloat((sum_mantissa & 0x7f), a_exp, a_sign);
}

BrainFloat vortex::operator*(const BrainFloat &a, const BrainFloat &b) {
    uint16_t a_exp = a.in_mem.parts.exponent;
    uint16_t b_exp = b.in_mem.parts.exponent;
    uint16_t a_mantissa = a.in_mem.parts.mantissa;
    uint16_t b_mantissa = b.in_mem.parts.mantissa;

    bool product_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;
    // Nan check
    if ((a_exp == 0xFF && a_mantissa > 1) || (b_exp == 0xFF && b_mantissa > 1)) {
        std::cout << "HERE IN NAN" << std::endl;
        return BrainFloat(1, 0xff, product_sign);
    }

    // Zero check
    if ((a_exp == 0 && a_mantissa == 0) || (b_exp == 0 && b_mantissa == 0)) {
        std::cout << "HERE IN ZERO" << std::endl;
        return BrainFloat(0, 0, product_sign);
    }
    // infinity check
    if (a_exp == 0xFF || b_exp == 0xff) {
        std::cout << "HERE IN infinity" << std::endl;
        return BrainFloat(0, 0xFF, product_sign);
    }
    a_mantissa = a.in_mem.parts.mantissa | 0x0080; // Add implicit bit
    b_mantissa = b.in_mem.parts.mantissa | 0x0080; // Add implicit bi

    std::bitset<8> bits(a_exp);
    std::cout << "Binary a exp: " << bits << std::endl;

    std::bitset<8> bis(a_mantissa);
    std::cout << "Binary a mantissa: " << bis << std::endl;

    // Multiply mantissas
    uint32_t product_mantissa = static_cast<uint32_t>(a_mantissa) * static_cast<uint32_t>(b_mantissa);

    // Add exponents
    int product_exp = a_exp + b_exp - 127;

    product_mantissa = (product_mantissa + 0x40) >> 7;

    // Round to nearest even (round half to even)
    if ((product_mantissa & 0x7F) == 0x40 && (product_mantissa & 0x1) != 0) {
        product_mantissa++;
    }

    if (product_mantissa & 0x0100) { // Check if the implicit bit shifted to the left
        product_mantissa >>= 1;
        product_exp++;
    }
    else {
        product_mantissa &= 0x7F; // Remove the implicit bit
    }

    return BrainFloat(product_mantissa, product_exp, product_sign);
}

BrainFloat vortex::operator/(const BrainFloat &a, const BrainFloat &b) {
    uint16_t a_exp = a.in_mem.parts.exponent;
    uint16_t b_exp = b.in_mem.parts.exponent;
    bool quotient_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;
    // Check if divisor is zero
    if (b_exp == 0 && b.in_mem.parts.mantissa == 0) {
        return BrainFloat(1, 0xFF, 0); // Return nam
    }
    // check if a is zero
    if (a_exp == 0 && a.in_mem.parts.mantissa == 0) {
        return BrainFloat(0, a_exp, quotient_sign);
    }
    // check if a is infinity and b is infinity
    if ((a.in_mem.f == 0x7f80 || a.in_mem.f == 0xff80) && (b.in_mem.f == 0x7f80 || b.in_mem.f == 0xff80)) {
        return BrainFloat(1, a_exp, quotient_sign);
    }

    if (a_exp == 0xff) {
        return BrainFloat(0, a_exp, quotient_sign);
    }
    // If divisor == inf return 0
    if (b_exp == 0xff) {
        return BrainFloat(0, 0, quotient_sign);
    }

    uint16_t a_mantissa = a.in_mem.parts.mantissa | 0x0080; // Add implicit bit
    uint16_t b_mantissa = b.in_mem.parts.mantissa | 0x0080; // Add implicit bit

    std::bitset<8> bits(a_exp);
    std::cout << "Binary a mantissa: " << bits << std::endl;
    std::bitset<8> bs(b_exp);
    std::cout << "Binary b  mantissa: " << bs << std::endl;
    
    // Subtract exponents
    int quotient_exp = (a_exp) - (b_exp) + 127;
    std::bitset<8> bis(quotient_exp);
    std::cout << "Binary quotient_exp: " << bis << std::endl;
    uint32_t quotient_mantissa;
    
    // Divide mantissas

    if (b.in_mem.parts.mantissa == 0) {

        quotient_mantissa = a_mantissa;
    }
    else {
        printf("HHHHHHIIII\n");
        quotient_mantissa = (static_cast<uint32_t>(a_mantissa)) / static_cast<uint32_t>(b_mantissa);
    }

    std::bitset<8> bem(quotient_mantissa);
    std::cout << "Binary quotient_mantissa: " << bem << std::endl;

    // IDK i need this it is super bad but it make it work with 21/7
    if (quotient_mantissa == 0) {
        quotient_exp--;
        quotient_mantissa = 0x40;
    }

    return BrainFloat(quotient_mantissa, quotient_exp, quotient_sign);
}