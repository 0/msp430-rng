#include <io.h>

/**
 * Random number generator.
 *
 * NOTE: This affects Timer A.
 *
 * Algorithm from TI SLAA338:
 * http://www.ti.com/sc/docs/psheets/abstract/apps/slaa338.htm
 *
 * @return 16 random bits generated from a hardware source.
 */
unsigned int rand() {
	int i, j;
	unsigned int result = 0;

	TACCTL0 = CAP | CM_1 | CCIS_1;            // Capture mode, positive edge
	TACTL = TASSEL_2 | MC_2;                  // SMCLK, continuous up

	for (i = 0; i < 16; i++) {
		unsigned int ones = 0;

		for (j = 0; j < 5; j++) {
			while (!(CCIFG & TACCTL0));       // Wait for interrupt

			TACCTL0 &= ~CCIFG;                // Clear interrupt

			if (1 & TACCR0)                   // If LSB set, count it
				ones++;
		}

		result >>= 1;                         // Save previous bits

		if (ones >= 3)                        // Best out of 5
			result |= 0x8000;                 // Set MSB
	}

	return result;
}

#define M 49381                               // Multiplier
#define I 8643                                // Increment

/**
 * Pseudo-random number generator.
 *
 * 16-bit linear congruential generator.
 * NOTE: Only treat the upper byte of the return value as random.
 *
 * @param state Previous state of the generator.
 * @return Next state of the generator.
 */
unsigned int prand(unsigned int state) {
	return (M * state + I);                   // Generate the next state of the LCG
}

/*
 * Test:
 */

#define TIMES 32

/**
 * Red light on   => Waiting for rand() to return
 * Green light on => Done
 */
void main() {
	volatile unsigned int result[TIMES];
	unsigned int s;
	int i;

	P1DIR |= BIT0 | BIT6;
	P1OUT |= BIT0;                            // R
	P1OUT &= ~BIT6;                           // !G

	s = rand();
	result[0] = s;

	P1OUT &= ~BIT0;                           // !R

	for (i = 1; i < TIMES; i++) {
		s = prand(s);
		result[i] = s;
	}

	P1OUT |= BIT6;                            // G

	asm("mov %0, r8" : : "r" (&result));      // Manually check TIMES words starting at address in r8
}
