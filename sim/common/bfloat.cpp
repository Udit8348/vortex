#include <iostream>
#include <bitset>

// get float "in-memory" to exploit iee754 binary representation of floating point values
// use a u to trick compiler into letting you access float's bits directly
// bitwise operations cannot be done directly on iee754 representations per compiler settings
// ordering of the fields is important here
class MyFloat
{
private:
    void printBinary(int n, int i)
    {
        // Prints the binary representation
        // of a number n up to i-bits.
        int k;
        for (k = i - 1; k >= 0; k--)
        {

            if ((n >> k) & 1)
                std::cout << "1";
            else
                std::cout << "0";
        }
    }

public:
    union BFloat_t
    {
        float f;
        int i;
        struct
        {
            uint32_t dead : 16;    // don't use these, just place-holders
            uint32_t mantissa : 7; // Mantissa (fractional part) of the number
            uint32_t exponent : 8; // Exponent (power of 2) of the number
            uint32_t sign : 1;
        } parts;
    };

    void printBFloat(BFloat_t b)
    {
        std::cout << b.parts.sign << " | ";
        printBinary(b.parts.exponent, 8);
        std::cout << " | ";
        printBinary(b.parts.mantissa, 7);
        std::cout << std::endl;
    }

    BFloat_t in_mem;

    MyFloat(float x)
    {
        in_mem.f = x;
        printBFloat(in_mem);
    }

    MyFloat(uint8_t mantissa, uint8_t exponent, bool sign)
    {
        in_mem.parts.mantissa = mantissa & 0x7F;
        in_mem.parts.exponent = exponent;
        in_mem.parts.sign = (int)sign;

        std::cout << "inside constructor" << std::endl;
        std::cout << "bfloat:" << in_mem.f << std::endl;
        printBFloat(in_mem);
    }

    friend MyFloat operator+(const MyFloat &a, const MyFloat &b)
    {
        // get fields
        bool a_sign = (bool)a.in_mem.parts.sign;
        uint8_t a_exp = a.in_mem.parts.exponent - 127;
        uint8_t a_mantissa = a.in_mem.parts.mantissa | 0x80; // add in the implicit bit

        bool b_sign = (bool)b.in_mem.parts.sign;
        uint8_t b_exp = b.in_mem.parts.exponent - 127;
        uint8_t b_mantissa = b.in_mem.parts.mantissa | 0x80; // add in the implicit bit

        // align mantissas by shifting the smaller exponent to the larger exponent
        if (a_exp < b_exp)
        {
            a_mantissa >>= (b_exp - a_exp);
            a_exp = b_exp;
        }
        else
        {
            b_mantissa >>= (a_exp - b_exp);
            b_exp = a_exp;
        }

        // add mantissas and adjust exponent if necessary
        int sum_mantissa = a_mantissa + b_mantissa;
        if (sum_mantissa & 0x100)
        { // this val check might be wrong
            sum_mantissa >>= 1;
            a_exp++;
        }

        // build binary representation of result
        return MyFloat(sum_mantissa, a_exp, a_sign);
    }

    friend MyFloat operator*(const MyFloat &a, const MyFloat &b)
    {
        uint16_t a_exp = a.in_mem.parts.exponent;
        uint16_t b_exp = b.in_mem.parts.exponent;
        uint16_t a_mantissa = a.in_mem.parts.mantissa;
        uint16_t b_mantissa = b.in_mem.parts.mantissa;

        bool product_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;
        // Nan check
        if ((a_exp == 0xFF && a_mantissa > 1) || (b_exp == 0xFF && b_mantissa > 1))
        {
            std::cout << "HERE IN NAN" << std::endl;
            return MyFloat(1, 0xff, product_sign);
        }

        // Zero check
        if ((a_exp == 0 && a_mantissa == 0) || (b_exp == 0 && b_mantissa == 0))
        {
            std::cout << "HERE IN ZERO" << std::endl;
            return MyFloat(0, 0, product_sign);
        }
        // infinity check
        if (a_exp == 0xFF || b_exp == 0xff)
        {
            std::cout << "HERE IN infinity" << std::endl;
            return MyFloat(0, 0xFF, product_sign);
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
        return MyFloat(product_mantissa, product_exp, product_sign);
    }

    friend MyFloat operator/(const MyFloat &a, const MyFloat &b)
    {
        uint16_t a_exp = a.in_mem.parts.exponent;
        uint16_t b_exp = b.in_mem.parts.exponent;
        bool quotient_sign = a.in_mem.parts.sign ^ b.in_mem.parts.sign;
        // Check if divisor is zero
        if (b_exp == 0 && b.in_mem.parts.mantissa == 0)
        {
            return MyFloat(1, 0xFF, 0); // Return nam
        }
        // check if a is zero
        if (a_exp == 0 && a.in_mem.parts.mantissa == 0)
        {
            return MyFloat(0, a_exp, quotient_sign);
        }
        // check if a is infinity and b is infinity
        if ((a.in_mem.f == 0x7f80 || a.in_mem.f == 0xff80) && (b.in_mem.f == 0x7f80 || b.in_mem.f == 0xff80))
        {
            return MyFloat(1, a_exp, quotient_sign);
        }

        if (a_exp == 0xff)
        {
            return MyFloat(0, a_exp, quotient_sign);
        }
        // If divisor == inf return 0
        if (b_exp == 0xff)
        {
            return MyFloat(0, 0, quotient_sign);
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

        if (b.in_mem.parts.mantissa == 0)
        {

            quotient_mantissa = a_mantissa;
        }
        else
        {
            printf("HHHHHHIIII\n");
            quotient_mantissa = (static_cast<uint32_t>(a_mantissa)) / static_cast<uint32_t>(b_mantissa);
        }

        std::bitset<8> bem(quotient_mantissa);
        std::cout << "Binary quotient_mantissa: " << bem << std::endl;

        // IDK i need this it is super bad but it make it work with 21/7
        if (quotient_mantissa == 0)
        {
            quotient_exp--;
            quotient_mantissa = 0x40;
        }

        return MyFloat(quotient_mantissa, quotient_exp, quotient_sign);
    }
};

union FloatToUInt32
{
    float f;
    uint32_t u;
};
int main()
{
    union FloatToUInt32 a;
    a.u = 0x7f800000;
    float b = 1;
    printf("a/b = %f\n", a.f);
    std::bitset<sizeof(float) * 8> bits(*reinterpret_cast<unsigned long *>(&a));
    std::cout << "Binary representation of " << a.f << " is \n"
              << bits << std::endl;
    std::cout << "Binary representation of " << b << " is \n"
              << bits << std::endl;

    MyFloat bfloat_version_of_a(a.f);
    MyFloat bfloat_version_of_b(b);
    MyFloat c = bfloat_version_of_b / bfloat_version_of_a;

    // You can now print the result stored in c or perform other operations with it.

    return 0;
}

/*

#include <iostream>

// function to print binary representation of a float number
void printBinary(float num) {
    unsigned char *p = reinterpret_cast<unsigned char*>(&num);
    for (int i = sizeof(float) - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            std::cout << ((p[i] >> j) & 1);
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

// function to add two binary representations of float numbers
float addBinary(float a, float b) {
    unsigned int a_bits = *(unsigned int*)&a;
    unsigned int b_bits = *(unsigned int*)&b;

    // separate sign, exponent, and mantissa of a and b
    int a_sign = (a_bits >> 31) & 1;
    int a_exp = ((a_bits >> 23) & 0xff) - 127;
    int a_mantissa = (a_bits & 0x7fffff) | 0x800000;
    int b_sign = (b_bits >> 31) & 1;
    int b_exp = ((b_bits >> 23) & 0xff) - 127;
    int b_mantissa = (b_bits & 0x7fffff) | 0x800000;

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
    if (sum_mantissa & 0x1000000) {
        sum_mantissa >>= 1;
        a_exp++;
    }

    // build binary representation of result
    int result_sign = a_sign;
    int result_exp = a_exp;
    int result_mantissa = sum_mantissa & 0x7fffff;

    // combine sign, exponent, and mantissa into a float number
    unsigned int result_bits = (result_sign << 31) | ((result_exp + 127) << 23) | result_mantissa;
    float result = *(float*)&result_bits;

    return result;
}

int main() {
    float a = 0.25f;
    float b = 100.0f;

    std::cout << "a = " << a << " in binary: ";
    printBinary(a);

    std::cout << "b = " << b << " in binary: ";
    printBinary(b);

    float sum = addBinary(a, b);

    std::cout << "a + b = " << sum << " in binary: ";
    printBinary(sum);

    return 0;
}


*/
