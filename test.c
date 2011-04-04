#include <io.h>

#include "rand.h"

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
