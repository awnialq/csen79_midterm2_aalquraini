#!/usr/bin/env python3
"""
Test script for BigNum class implementation.
Tests the two constructors (string and long) and addition operator.
"""

import subprocess
import sys
from typing import Tuple, Optional

class BigNumTester:
    def __init__(self, executable='./bignum'):
        self.executable = executable
        self.tests_passed = 0
        self.tests_failed = 0
        
    def compile_program(self) -> bool:
        """Compile the BigNum program using make."""
        print("Compiling BigNum program...")
        try:
            result = subprocess.run(['make', 'clean'], 
                                  capture_output=True, 
                                  text=True,
                                  timeout=10)
            result = subprocess.run(['make'], 
                                  capture_output=True, 
                                  text=True,
                                  timeout=10)
            if result.returncode != 0:
                print(f"Compilation failed:\n{result.stderr}")
                return False
            print("✓ Compilation successful\n")
            return True
        except Exception as e:
            print(f"Error during compilation: {e}")
            return False
    
    def run_bignum_test(self, num1_str: str, num2_str: str) -> Optional[dict]:
        """
        Run the bignum program with two numbers and parse the output.
        Returns dict with results or None if failed.
        """
        try:
            input_data = f"{num1_str}\n{num2_str}\nq\n"
            result = subprocess.run([self.executable],
                                  input=input_data,
                                  capture_output=True,
                                  text=True,
                                  timeout=5)
            
            if result.returncode != 0:
                print(f"Program exited with error code {result.returncode}")
                print(f"stderr: {result.stderr}")
                return None
            
            # Parse output
            output = result.stdout
            parsed = {}
            
            for line in output.split('\n'):
                line = line.strip()
                if line.startswith('orig='):
                    if 'orig1' not in parsed:
                        parsed['orig1'] = line.split('=', 1)[1]
                    else:
                        parsed['orig2'] = line.split('=', 1)[1]
                elif line.startswith('bn='):
                    parsed['bn'] = line.split('=', 1)[1]
                elif line.startswith('bnLong='):
                    parsed['bnLong'] = line.split('=', 1)[1]
                elif line.startswith('bn2='):
                    parsed['bn2'] = line.split('=', 1)[1]
                elif line.startswith('bn1+bn2='):
                    parsed['sum'] = line.split('=', 1)[1]
                elif 'no long constructor' in line.lower():
                    parsed['no_long'] = True
            
            return parsed
        except subprocess.TimeoutExpired:
            print("Program timed out")
            return None
        except Exception as e:
            print(f"Error running test: {e}")
            return None
    
    def verify_string_constructor(self, orig: str, bn: str) -> bool:
        """Verify string constructor produces correct output."""
        # Remove leading zeros for comparison, but preserve "0"
        orig_normalized = orig.lstrip('0') or '0'
        bn_normalized = bn.lstrip('0') or '0'
        
        # Handle negative numbers
        if orig.startswith('-'):
            orig_normalized = '-' + (orig[1:].lstrip('0') or '0')
            bn_normalized = '-' + (bn[1:].lstrip('0') or '0') if bn.startswith('-') else bn
        
        return orig_normalized == bn_normalized
    
    def verify_long_constructor(self, orig: str, bn_long: str) -> bool:
        """Verify long constructor produces correct output."""
        # Same normalization as string constructor
        return self.verify_string_constructor(orig, bn_long)
    
    def verify_addition(self, num1: str, num2: str, result: str) -> bool:
        """Verify addition operator produces correct result."""
        try:
            # Convert to Python integers for verification
            n1 = int(num1)
            n2 = int(num2)
            expected = n1 + n2
            
            # Normalize the result
            result_normalized = result.lstrip('0') or '0'
            if result.startswith('-'):
                result_normalized = '-' + (result[1:].lstrip('0') or '0')
            
            expected_str = str(expected)
            
            return expected_str == result_normalized
        except ValueError:
            print(f"Could not convert to int for verification: {num1}, {num2}")
            return False
    
    def test_case(self, num1: str, num2: str, test_name: str):
        """Run a complete test case."""
        print(f"Test: {test_name}")
        print(f"  Input: {num1} and {num2}")
        
        result = self.run_bignum_test(num1, num2)
        if result is None:
            print("  ✗ Test failed - could not run program\n")
            self.tests_failed += 1
            return
        
        # Test string constructor for first number
        if 'bn' in result:
            if self.verify_string_constructor(num1, result['bn']):
                print(f"  ✓ String constructor for {num1}: {result['bn']}")
            else:
                print(f"  ✗ String constructor failed: expected {num1}, got {result['bn']}")
                self.tests_failed += 1
                return
        else:
            print("  ✗ No output for string constructor")
            self.tests_failed += 1
            return
        
        # Test long constructor (if applicable)
        try:
            long_val = int(num1)
            # Check if it's within typical long range (-2^63 to 2^63-1)
            if -9223372036854775808 <= long_val <= 9223372036854775807:
                if 'bnLong' in result:
                    if self.verify_long_constructor(num1, result['bnLong']):
                        print(f"  ✓ Long constructor for {num1}: {result['bnLong']}")
                    else:
                        print(f"  ✗ Long constructor failed: expected {num1}, got {result['bnLong']}")
                        self.tests_failed += 1
                        return
                elif 'no_long' in result:
                    print(f"  ⚠ Long constructor skipped (value too large)")
                else:
                    print("  ✗ No output for long constructor")
                    self.tests_failed += 1
                    return
            else:
                if 'no_long' in result:
                    print(f"  ✓ Long constructor correctly skipped (value too large)")
                else:
                    print(f"  ⚠ Value outside long range")
        except ValueError:
            print(f"  ⚠ Could not test long constructor for non-numeric input")
        
        # Test string constructor for second number
        if 'bn2' in result:
            if self.verify_string_constructor(num2, result['bn2']):
                print(f"  ✓ String constructor for {num2}: {result['bn2']}")
            else:
                print(f"  ✗ String constructor failed: expected {num2}, got {result['bn2']}")
                self.tests_failed += 1
                return
        else:
            print("  ✗ No output for second string constructor")
            self.tests_failed += 1
            return
        
        # Test addition operator
        if 'sum' in result:
            if self.verify_addition(num1, num2, result['sum']):
                expected_sum = int(num1) + int(num2)
                print(f"  ✓ Addition: {num1} + {num2} = {result['sum']} (expected {expected_sum})")
            else:
                expected_sum = int(num1) + int(num2)
                print(f"  ✗ Addition failed: {num1} + {num2} = {result['sum']} (expected {expected_sum})")
                self.tests_failed += 1
                return
        else:
            print("  ✗ No output for addition")
            self.tests_failed += 1
            return
        
        self.tests_passed += 1
        print(f"  ✓ All checks passed\n")
    
    def run_all_tests(self):
        """Run comprehensive test suite."""
        print("="*60)
        print("BigNum Class Test Suite")
        print("Testing: String Constructor, Long Constructor, Addition")
        print("="*60 + "\n")
        
        # Test cases: (num1, num2, description)
        test_cases = [
            ("0", "0", "Zero values"),
            ("1", "1", "Simple single digits"),
            ("5", "7", "Single digit addition"),
            ("123", "456", "Three digit numbers"),
            ("999", "1", "Carry propagation"),
            ("12345", "67890", "Five digit numbers"),
            ("999999", "1", "Multiple carries"),
            ("1234567890", "9876543210", "Large numbers"),
            ("100", "200", "Round numbers"),
            ("1000000", "2000000", "Millions"),
        ]
        
        for num1, num2, description in test_cases:
            self.test_case(num1, num2, description)
        
        # Summary
        print("="*60)
        print("Test Summary")
        print("="*60)
        print(f"Tests Passed: {self.tests_passed}")
        print(f"Tests Failed: {self.tests_failed}")
        total = self.tests_passed + self.tests_failed
        if total > 0:
            print(f"Success Rate: {self.tests_passed/total*100:.1f}%")
        print("="*60)
        
        return self.tests_failed == 0

def main():
    tester = BigNumTester()
    
    # Compile the program
    if not tester.compile_program():
        print("Failed to compile. Exiting.")
        sys.exit(1)
    
    # Run all tests
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
