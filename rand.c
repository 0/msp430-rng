#include <io.h>

#include "rand.h"

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
unsigned int rand(void) {
	int i, j;
	unsigned int result = 0;

	/* Save state */
	unsigned int TACCTL0_old = TACCTL0;
	unsigned int TACTL_old = TACTL;

	/* Set up timer */
	TACCTL0 = CAP | CM_1 | CCIS_1;            // Capture mode, positive edge
	TACTL = TASSEL_2 | MC_2;                  // SMCLK, continuous up

	/* Generate bits */
	for (i = 0; i < 16; i++) {
		unsigned int ones = 0;

		for (j = 0; j < 5; j++) {
			while (!(CCIFG & TACCTL0));       // Wait for interrupt

			TACCTL0 &= ~CCIFG;                // Clear interrupt

			if (1 & TACCR0)                   // If LSb set, count it
				ones++;
		}

		result >>= 1;                         // Save previous bits

		if (ones >= 3)                        // Best out of 5
			result |= 0x8000;                 // Set MSb
	}

	/* Restore state */
	TACCTL0 = TACCTL0_old;
	TACTL = TACTL_old;

	return result;
}

/**
 * Pseudo-random number generator.
 *
 * Implemented by a 16-bit linear congruential generator.
 * NOTE: Only treat the MSB of the return value as random.
 *
 * @param state Previous state of the generator.
 * @return Next state of the generator.
 */
unsigned int prand(unsigned int state) {
	return M * state + I;                     // Generate the next state of the LCG
}
