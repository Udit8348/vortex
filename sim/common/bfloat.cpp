#include <iostream>
#include <bitset>
#include <cmath>

class BrainFloat {
private:
    void printBinary(int n, int i)
    {
        // Prints the binary representation
        // of a number n up to i-bits.
        int k;
        for (k = i - 1; k >= 0; k--) {
            if ((n >> k) & 1)
                std::cout << "1";
            else
                std::cout << "0";
        }
    }
    
    static void swap (uint32_t* L, uint32_t* R) {
        uint32_t temp = *L;
        *L = *R;
        *R = temp;
    }

public:
    union bfloat16 {
    float f;
    struct {
        uint16_t dead       : 16;   // unused in bfloat16
        uint32_t mantissa   : 7;    // Mantissa (fractional part) of the number
        uint32_t exponent   : 8;    // Exponent (power of 2) of the number
        bool sign           : 1;    // Sign bit (bool is a 1-bit datatype)
    } parts;
    };
    
    bfloat16 in_mem;
    
    void print(bfloat16 b) {
        std::cout << b.parts.sign << " | ";
        printBinary(b.parts.exponent, 8);
        std::cout << " | ";
        printBinary(b.parts.mantissa, 7);
        std::cout << std::endl;
    }
    
    // follow ieee-754 to interpret bit value of BrainFloat
    float to_float() {
        uint8_t sign = in_mem.parts.sign;
        double exp = in_mem.parts.exponent - 127;
        uint8_t mantissa = in_mem.parts.mantissa;
        return std::pow(-1, sign) * std::pow(2, exp) * (1+(mantissa / std::pow(2,7)));
    }
    
    // constructor for float -> BrainFloat
    BrainFloat(float x) {
        in_mem.f = x;
        print(in_mem);
    }
    
    // construct individual components of BrainFloat
    BrainFloat(uint8_t mantissa, uint8_t exponent, bool sign) {
        in_mem.parts.mantissa = mantissa & 0x7F;
        in_mem.parts.exponent = exponent;
        in_mem.parts.sign = (int) sign;
        print(in_mem);
    }
    
    friend BrainFloat operator-(const BrainFloat& a, const BrainFloat& b) {
        // get fields for operands
        bool a_sign = a.in_mem.parts.sign;
        uint32_t a_exp = (uint32_t) a.in_mem.parts.exponent - 127;
        uint32_t a_mantissa =  (uint32_t) a.in_mem.parts.mantissa | 0x80; // add in the implicit bit
            
        bool b_sign = b.in_mem.parts.sign;
        uint32_t b_exp =  (uint32_t) b.in_mem.parts.exponent - 127;
        uint32_t b_mantissa = (uint32_t) b.in_mem.parts.mantissa | 0x80; // add in the implicit bit
            
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
        return BrainFloat((diff_mantissa & 0x7f), a_exp + 127, a_sign);
    }
    
    friend BrainFloat operator-(const BrainFloat& a) {
        // get fields for operands
        bool a_sign = a.in_mem.parts.sign ^ 1;
        uint32_t a_exp = (uint32_t) a.in_mem.parts.exponent;
        uint32_t a_mantissa =  (uint32_t) a.in_mem.parts.mantissa;
            
        // build binary representation of result
        return BrainFloat(a_mantissa, a_exp, a_sign);
    }
    
    friend BrainFloat operator+(const BrainFloat& a, const BrainFloat& b) {
        // get fields for operands
        bool a_sign = a.in_mem.parts.sign;
        uint32_t a_exp = (uint32_t) a.in_mem.parts.exponent - 127;
        uint32_t a_mantissa =  (uint32_t) a.in_mem.parts.mantissa & 0x7F; // remove the sign bit
        
        bool b_sign = b.in_mem.parts.sign;
        uint32_t b_exp =  (uint32_t) b.in_mem.parts.exponent - 127;
        uint32_t b_mantissa = (uint32_t) b.in_mem.parts.mantissa & 0x7F; // remove the sign bit
        
        // check if operands are negative
        bool negative = false;
        if (a_sign && b_sign) {
            negative = true;
        } else if (a_sign) {
            return b - (-a);
        } else if (b_sign) {
            return a - (-b);
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
        if (sum_mantissa & 0x80) {
            sum_mantissa >>= 1;
            a_exp++;
        }
        
        // build binary representation of result
        uint8_t sign = negative ? 1 : 0;
        uint8_t exponent = a_exp + 127;
        uint8_t mantissa = sum_mantissa & 0x7F;
        if (negative) {
            mantissa |= 0x80;
        }
        return BrainFloat(mantissa, exponent, sign);
    }

    friend BrainFloat operator*(const BrainFloat &a, const BrainFloat &b)
    {
        uint16_t a_exp = a.in_mem.parts.exponent;
        uint16_t b_exp = b.in_mem.parts.exponent;
        uint16_t a_mantissa = a.in_mem.parts.mantissa | 0x0080; // Add implicit bit
        uint16_t b_mantissa = b.in_mem.parts.mantissa | 0x0080; // Add implicit bi

        std::bitset<8> bits(a_exp);
        std::cout << "Binary a exp: " << bits << std::endl;

        bool product_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;

        if (a_exp == 0xFF || b_exp == 0xff)
        {
            return BrainFloat(0, 0xFF, product_sign);
        }
        // Multiply mantissas
        uint32_t product_mantissa = static_cast<uint32_t>(a_mantissa) * static_cast<uint32_t>(b_mantissa);

        // Add exponents
        int product_exp = a_exp + b_exp - 127;

        product_mantissa = (product_mantissa + 0x40) >> 7;

        // Round to nearest even (round half to even)
        if ((product_mantissa & 0x7F) == 0x40 && (product_mantissa & 0x1) != 0)
        {
            product_mantissa++;
        }
        if (product_mantissa & 0x0100)
        { // Check if the implicit bit shifted to the left
            product_mantissa >>= 1;
            product_exp++;
        }
        else
        {
            product_mantissa &= 0x7F; // Remove the implicit bit
        }
        return BrainFloat(product_mantissa, product_exp, product_sign);
    }

    friend BrainFloat operator/(const BrainFloat &a, const BrainFloat &b)
    {
        uint16_t a_exp = a.in_mem.parts.exponent;
        uint16_t b_exp = b.in_mem.parts.exponent;
        std::bitset<8> bits(b_exp);
        std::cout << "Binary b exp: " << bits << std::endl;
        uint16_t a_mantissa = a.in_mem.parts.mantissa | 0x0080; // Add implicit bit
        uint16_t b_mantissa = b.in_mem.parts.mantissa | 0x0080; // Add implicit bit

        bool quotient_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;

        // Check if divisor is zero
        if (b_exp == 0 && b_mantissa == 0)
        {
            std::cout << "HERE" << std::endl;
            return BrainFloat(0, 0xFF, quotient_sign); // Return infinity with the appropriate sign
        }

        // Check for infinity or zero in dividend
        if (a_exp == 0xFF || a_exp == 0)
        {
            return BrainFloat(0, a_exp, quotient_sign);
        }

        // Subtract exponents
        int quotient_exp = a_exp - b_exp + 127;

        // Divide mantissas
        uint32_t quotient_mantissa = (static_cast<uint32_t>(a_mantissa) << 8) / static_cast<uint32_t>(b_mantissa);

        quotient_mantissa = (quotient_mantissa + 0x40) >> 8;

        // Round to nearest even (round half to even)
        if ((quotient_mantissa & 0x1) != 0 && (quotient_mantissa & 0x7F) == 0x40)
        {
            quotient_mantissa--;
        }
        else if ((quotient_mantissa & 0x7F) == 0x40)
        {
            quotient_mantissa++;
        }

        if (quotient_mantissa & 0x0100)
        { // Check if the implicit bit shifted to the left
            quotient_mantissa >>= 1;
            quotient_exp++;
        }
        else
        {
            quotient_mantissa &= 0x7F; // Remove the implicit bit
        }
        return BrainFloat(quotient_mantissa, quotient_exp, quotient_sign);
    }
};

//cant compile with 2 mains need to add tests

// int main()
// {
//     float a = 8;
//     float b = 0;
//     std::cout << a << std::endl;

//     std::bitset<sizeof(float) * 8> bits(*reinterpret_cast<unsigned long *>(&a));
//     std::cout << "Binary representation of " << a << " is \n"
//               << bits << std::endl;
//     std::cout << "Binary representation of " << b << " is \n"
//               << bits << std::endl;

//     BrainFloat bfloat_version_of_a(a);
//     BrainFloat bfloat_version_of_b(b);
//     BrainFloat c = bfloat_version_of_a / bfloat_version_of_b;

//     // You can now print the result stored in c or perform other operations with it.

//     return 0;
// }
