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
    unsigned long mag;    //magnitude of the long
    if(n < 0){
        sign = -1;
    }

    capacity = INCREMENT;

    digits = new (std::nothrow) store_t[capacity];
    if(digits == nullptr) { throw std::bad_alloc();}

    if(n == 0){
        digits[0] = 0;
        high = 1;
        return;
    }

    mag = static_cast<unsigned long>(std::abs(n));
    int index = 0;
    
    while(mag > 0){
        try{
            checkCapacity(index);
        } catch(std::exception &e){ std::cerr << e.what() << std::endl;}

        digits[index++] = mag % StoreCap;

        mag = mag / StoreCap;
    }

    high = index;

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