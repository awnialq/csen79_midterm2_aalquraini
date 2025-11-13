/*
 * Sin-Yaw Wang <swang24@scu.edu>
 * For CSEN79 Exercice
 */
#include <iostream>
#include "bignum.h"

namespace csen79 {

// trivial and lazy implementations.  consider enhancing these.
BigNum::~BigNum() { delete [] digits; }
BigNum::BigNum(const BigNum &rhs) {this->operator=(rhs);}

BigNum& BigNum::operator=(const BigNum &rhs) { this->deepCopy(rhs); return *this; }
BigNum& BigNum::operator=(BigNum &&rhs) {return this->operator=(rhs);}	// move operator

// implement these three
BigNum::BigNum(const long &n) {
    if(n < 0){
        sign = -1;
    }

    digits = new (std::nothrow) store_t[INCREMENT];
    if(digits == nullptr) { throw std::bad_alloc();}

    
}
BigNum& BigNum::operator+(const BigNum &op) { return *this; }
void BigNum::deepCopy(const BigNum &rhs) {}

// Formatted output
std::ostream& operator<<(std::ostream &os, const BigNum &n) {
    // if we want to be fancy
    if (os.flags() & os.dec) { /* print in decimal */}
    else if (os.flags() & os.hex) { /* print in hex */}
    else if (os.flags() & os.oct) { /* print in oct */}

    // since they did nothing
    return n.osdebug(os);
}


}