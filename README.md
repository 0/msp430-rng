# msp430-rng

Random and pseudorandom number generation for the MSP430, implemented in C.


## Usage

1. `make rand`
1. `#include "msp430-rng/rand.h"`
1. `msp430-elf-gcc -o foo.elf foo.o rand.o`


## Generators

### Random (`rand()`)

Truly random number generator.
Uses the time difference between the VLO and DCO, as outlined in [SLAA338](http://www.ti.com/sc/docs/psheets/abstract/apps/slaa338.htm).
Rather slow, and is only intended to be used to generate seeds for a PRNG.

### Pseudorandom (`prand()`)

A very simple and fast PRNG implemented by a linear congruential generator with the following values:

* **modulus**: 65536 (2^16)
* **multiplier**: 49381
* **increment**: 8643


## Supported compilers

Currently only tested using the gcc `msp430-elf` [cross-compiler](https://gcc.gnu.org/onlinedocs/gcc/MSP430-Options.html) (not [mspgcc](http://sourceforge.net/projects/mspgcc/)).
Patches with modifications for other environments are welcome.


## Tests

To run the tests, `make test` and run `test.elf` on your MSP430.
They should take less than a minute to finish.

The test mechanism assumes that you are using a Launchpad, with LEDs on pins 0 and 6.
While the tests are running, both LEDs are on; when the tests finish, one of the LEDs blinks (green: pass, red: fail).

Using [mspdebug](http://mspdebug.sourceforge.net/), it is easy to see which test is failing and possibly why:
```
prog test.elf
run
^C
md status 2
md sum_rand 2
md sum_prand 2
```

### Monobit

An implementation of the Monobit algorithm outlined in section 2.1 of [SP 800-22 Rev. 1a](http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf).
Checks that the numbers of 0 and 1 bits in the produced values are equal to within a certain tolerance.


## License

Provided under the terms of the MIT license.
See LICENSE.txt for more information.
