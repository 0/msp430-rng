# msp430-rng

Random and pseudorandom number generation for the MSP430, implemented in C.

## Usage

1. `make rand`
1. `#include "msp430-rng/rand.h"`
1. `msp430-gcc -o foo.elf foo.o rand.o`

## Generators

### Random

Truly random number generator. Uses the time difference between the VLO and DCO, as outlined in [SLAA338](http://www.ti.com/sc/docs/psheets/abstract/apps/slaa338.htm).

Rather slow, and is only intended to be used to generate seeds for a PRNG.

### Pseudorandom

A very simple and fast PRNG implemented by a linear congruential generator with the following values:

* **modulus**: 65536 (= 2 ^ 16)
* **multiplier**: 49381
* **increment**: 8643

## Tests

To run the tests, `make test` and run `test.elf` on your MSP430.

### Monobit

An implementation of the Monobit algorithm outlined in section 2.1 of [SP 800-22 Rev. 1a](http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf).

Checks that the numbers of 0 and 1 bits in the produced values are equal to within a certain tolerance.
