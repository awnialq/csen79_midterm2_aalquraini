/*
 * Sin-Yaw Wang <swang24@scu.edu>
 * For CSEN79 Exercice
 */
#include <exception>
#include <iostream>
#include <vector>
#include <cstring>
#include "bignum.h"

namespace csen79 {

// trivial and lazy implementations.  consider enhancing these.
BigNum::~BigNum() { delete [] digits; }
BigNum::BigNum(const BigNum &rhs) : digits(nullptr), high(0), sign(1), capacity(0) {this->operator=(rhs);}

BigNum& BigNum::operator=(const BigNum &rhs) { this->deepCopy(rhs); return *this; }
BigNum& BigNum::operator=(BigNum &&rhs) {return this->operator=(rhs);}	// move operator

// implement these three
BigNum::BigNum(const long &n): digits(nullptr), high(0), sign(1), capacity(0) {
    // Handle zero case first
    if (n == 0) {
        try {
            checkCapacity(0);
        } catch(std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }
        digits[0] = 0;
        high = 1;
        return;
    }

    // Determine sign and convert to unsigned magnitude
    unsigned long mag;
    if (n < 0) {
        sign = -1;
        mag = static_cast<unsigned long>(-(n + 1)) + 1;
    } else {
        sign = 1;
        mag = static_cast<unsigned long>(n);
    }

    int index = 0;

    // Extract digits in base 256
    while (mag > 0) {
        try {
            checkCapacity(index);
        } catch(std::exception &e) {
            std::cerr << e.what() << std::endl;
            throw;
        }

        digits[index] = static_cast<store_t>(mag % StoreCap);
        if (high <= static_cast<unsigned int>(index))
            high = index + 1;
        mag = mag / StoreCap;
        index++;
    }
}

BigNum& BigNum::operator+(const BigNum &op) {
    // Handle self-addition: make a copy
    if (this == &op) {
        BigNum temp(op);
        return this->operator+(temp);
    }
    
    // compare magnitudes: return 1 if a>b, 0 if equal, -1 if a<b
    auto cmpMag = [](const BigNum &a, const BigNum &b)->int {
        if (a.high != b.high) return (a.high > b.high) ? 1 : -1;
        for (int i = static_cast<int>(a.high) - 1; i >= 0; --i) {
            if (a.digits[i] != b.digits[i]) return (a.digits[i] > b.digits[i]) ? 1 : -1;
        }
        return 0;
    };

    // same sign: magnitude add into *this
    if (this->sign == op.sign) {
        int i = 0;
        buffer_t carry = 0;
        for (; i < static_cast<int>(this->high) || i < static_cast<int>(op.high) || carry; ++i) {
            try { checkCapacity(i); } catch(std::exception &e) { std::cerr << e.what() << std::endl; }
            BigNum::buffer_t av = (i < static_cast<int>(this->high)) ? this->digits[i] : 0;
            BigNum::buffer_t bv = (i < static_cast<int>(op.high)) ? op.digits[i] : 0;
            BigNum::buffer_t s = av + bv + carry;
            this->digits[i] = static_cast<BigNum::store_t>(s % StoreCap);
            carry = static_cast<BigNum::buffer_t>(s / StoreCap);
            if (this->high <= static_cast<unsigned int>(i))
                this->high = i+1;
        }
        // ensure at least one digit
        if (this->high == 0) this->high = 1;
        return *this;
    }

    // different signs: perform magnitude subtraction
    int cmp = cmpMag(*this, op);
    if (cmp == 0) {
        // magnitudes equal -> result is zero
        if (this->capacity == 0) {
            try { checkCapacity(0); } catch(std::exception &e) { std::cerr << e.what() << std::endl; }
        }
        this->digits[0] = 0;
        this->high = 1;
        this->sign = 1;
        return *this;
    }

    if (cmp > 0) {
        // |this| > |op|, compute this = this - op
        BigNum::buffer_t borrow = 0;
        for (unsigned int i = 0; i < this->high; ++i) {
            BigNum::buffer_t av = this->digits[i];
            BigNum::buffer_t bv = (i < op.high) ? op.digits[i] : 0;
            if (av < bv + borrow) {
                this->digits[i] = static_cast<BigNum::store_t>(av + StoreCap - bv - borrow);
                borrow = 1;
            } else {
                this->digits[i] = static_cast<BigNum::store_t>(av - bv - borrow);
                borrow = 0;
            }
        }
        // trim leading zeros
        while (this->high > 1 && this->digits[this->high - 1] == 0)
            --this->high;
        // sign remains as original this->sign
        return *this;
    } else {
        // |op| > |this|, compute result = op - this and store in *this
        unsigned int newHigh = op.high;
        size_t newCap = newHigh + INCREMENT;
        BigNum::store_t *res = new (std::nothrow) BigNum::store_t[newCap];
        if (res == nullptr) throw std::bad_alloc();
        memset(res, 0, sizeof(BigNum::store_t) * newCap);
        // perform subtraction op - this
        BigNum::buffer_t borrow = 0;
        for (unsigned int i = 0; i < newHigh; ++i) {
            BigNum::buffer_t av = (i < op.high) ? op.digits[i] : 0;
            BigNum::buffer_t bv = (i < this->high) ? this->digits[i] : 0;
            if (av < bv + borrow) {
                res[i] = static_cast<BigNum::store_t>(av + StoreCap - bv - borrow);
                borrow = 1;
            } else {
                res[i] = static_cast<BigNum::store_t>(av - bv - borrow);
                borrow = 0;
            }
        }
        // determine actual high
        while (newHigh > 1 && res[newHigh - 1] == 0) --newHigh;

        // replace digits buffer
        delete [] this->digits;
        this->digits = res;
        this->capacity = newCap;
        this->high = newHigh;
        this->sign = op.sign;
        return *this;
    }
}

void BigNum::deepCopy(const BigNum &rhs) {
    store_t *temp = new store_t[rhs.capacity];
    delete [] digits;
    digits = temp;
    capacity = rhs.capacity;
    high = rhs.high;
    sign = rhs.sign;

    for(int i = 0; i < high; ++i){
        digits[i] = rhs.digits[i];
    }
}

// Formatted output
std::ostream& operator<<(std::ostream &os, const BigNum &n) {
    // Convert from base-256 to decimal string
    if (n.high == 0 || (n.high == 1 && n.digits[0] == 0)) {
        os << "0";
        return os;
    }
    
    // Use a vector to store decimal digits
    std::vector<unsigned char> decimal;
    decimal.push_back(0);
    
    // Process each base-256 digit from high to low
    for (int i = n.high - 1; i >= 0; --i) {
        // Multiply current decimal result by 256
        unsigned int carry = 0;
        for (size_t j = 0; j < decimal.size(); ++j) {
            unsigned int temp = decimal[j] * 256 + carry;
            decimal[j] = temp % 10;
            carry = temp / 10;
        }
        while (carry > 0) {
            decimal.push_back(carry % 10);
            carry /= 10;
        }
        
        // Add the current base-256 digit
        carry = n.digits[i];
        for (size_t j = 0; j < decimal.size() && carry > 0; ++j) {
            unsigned int temp = decimal[j] + carry;
            decimal[j] = temp % 10;
            carry = temp / 10;
        }
        while (carry > 0) {
            decimal.push_back(carry % 10);
            carry /= 10;
        }
    }
    
    // Output sign
    if (n.sign < 0) {
        os << "-";
    }
    
    // Output decimal digits (stored in reverse order)
    for (int i = decimal.size() - 1; i >= 0; --i) {
        os << static_cast<char>('0' + decimal[i]);
    }
    
    return os;
}


}