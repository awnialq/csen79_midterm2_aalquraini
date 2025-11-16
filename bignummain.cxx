#include <string>
#include <iostream>
#include <vector>
#include "bignum.h"

using namespace std;
using namespace csen79;

/*
 * Simple interactive program for BigNum.
 * For comprehensive testing, use the Python test script: test_bignum.py
 */
int main(void) {

	std::string str;
	while (true) {
		cin >> str;
		cout << endl;
		if (str[0] == 'q' || str[0] == 'Q')
			break;
		try {
			BigNum bn(str);	// constructed with string
			// this output is meant for Python to execute
			cout << "orig=" << str << endl;
			cout << "bn=" << bn << endl;
			try{
				long longNum = std::stol(str);
				BigNum bnLong(longNum);
				cout << "bnLong=" << bnLong << endl;
			} catch(exception &e){
				cout << "Value larger than type long (no long constructor)" << endl;
			}
			cin >> str;
			cout << endl;
			BigNum bn2(str);
			cout << "orig=" << str << endl;
			cout << "bn2=" << bn2 << endl;
			cout << "bn1+bn2=" << bn + bn2 << endl;
			cout << "print('yes' if orig==bn else 'no')" << endl;
			cout << endl;
		} catch (std::bad_alloc const &e) {
			cerr << "failed to allocate memory for " << str << endl;
		}
	}

	return EXIT_SUCCESS;
}
