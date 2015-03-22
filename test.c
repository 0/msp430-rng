#include <msp430.h>

#include "rand.h"

#define LED_OUT P1OUT
#define LED_DIR P1DIR

#define LED_RED   BIT0
#define LED_GREEN BIT6

#define BLINK_DELAY 1200                      // 200 ms at 6 KHz

#define BITS_RAND  16
#define BITS_PRAND 8                          // Using only the MSB of the prand state

#define SUCCESS    0x0000
#define FAIL_RAND  0x0001
#define FAIL_PRAND 0x0002
#define RUNNING    0x8000

unsigned int status;
int sum_rand;
int sum_prand;

void __attribute__((interrupt(TIMERA1_VECTOR))) blink(void) {
	TACCTL1 &= ~CCIFG;                        // Unset interrupt flag

	if (status)                               // Toggle LEDs
		LED_OUT ^= LED_RED;
	else
		LED_OUT ^= LED_GREEN;
}

/******************************************************************************
 * Monobit
 *
 * SP 800-22 Rev. 1a
 * http://csrc.nist.gov/publications/nistpubs/800-22-rev1a/SP800-22rev1a.pdf
 ******************************************************************************/

/* Each n-bit number tested with monobit contributes n bits, so in the worst
 * case (all bits the same), the signed 16-bit bucket can store information
 * about this many numbers: */
#define MONOBIT_TIMES_RAND  2047              // (2^15-1) / BITS_RAND
#define MONOBIT_TIMES_PRAND 4095              // (2^15-1) / BITS_PRAND

/* The maximum absolute value of the sum bucket after a monobit test, where
 * 0.01 is the minimum P-value and inverfc is the inverse of the complementary
 * error function. */
#define MONOBIT_MAX_VAL 466                   // inverfc(0.01) * sqrt(2) * sqrt(2^15-1)

/**
 * Monobit test for rand().
 */
int monobit_rand(void) {
	int i, j;

	sum_rand = 0;

	for (i = 0; i < MONOBIT_TIMES_RAND; i++) {
		/* Ignore the least significant bits. */
		unsigned int r = rand() >> (16 - BITS_RAND);

		/* Add up all the bits, taking 0 to mean -1. */
		for (j = 0; j < BITS_RAND; j++) {
			sum_rand += r & 0x1 ? 1 : -1;
			r >>= 1;
		}
	}

	if (sum_rand < 0)
		sum_rand = 0 - sum_rand;              // Absolute value

	return sum_rand <= MONOBIT_MAX_VAL ? SUCCESS : FAIL_RAND;
}

/**
 * Monobit test for prand().
 */
int monobit_prand() {
	unsigned int state = rand();
	int i, j;

	sum_prand = 0;

	for (i = 0; i < MONOBIT_TIMES_PRAND; i++) {
		/* Ignore the least significant bits. */
		unsigned int r = state >> (16 - BITS_PRAND);

		/* Add up all the bits, taking 0 to mean -1. */
		for (j = 0; j < BITS_PRAND; j++) {
			sum_prand += r & 0x1 ? 1 : -1;
			r >>= 1;
		}

		state = prand(state);
	}

	if (sum_prand < 0)
		sum_prand = 0 - sum_prand;            // Absolute value

	return sum_prand <= MONOBIT_MAX_VAL ? SUCCESS : FAIL_PRAND;
}

/**
 * Run though all the tests.
 *
 * Both LEDs are lit up while testing, and one will blink once the tests are
 * done, depending on the outcome.
 */
void main(void) {
	WDTCTL = WDTPW | WDTHOLD;                 // Stop the dog

	LED_DIR |= LED_RED | LED_GREEN;
	LED_OUT |= LED_RED | LED_GREEN;

	status = RUNNING;
	status |= monobit_rand();
	status |= monobit_prand();
	status &= ~RUNNING;

	if (status)
		LED_OUT &= ~LED_GREEN;
	else
		LED_OUT &= ~LED_RED;

	BCSCTL3 |= LFXT1S_2;                      // Set LF to VLO = 12 KHz
	BCSCTL1 |= DIVA_1;                        // ACLK = LF / 2 = 6 KHz

	TACCR0 = BLINK_DELAY;                     // Set the timer
	TACTL = TASSEL_1 | MC_1;                  // TACLK = ACLK; up to CCR0
	TACCTL1 = CCIE | OUTMOD_3;                // TA1 interrupt enable; PWM set/reset

	__bis_SR_register(LPM3_bits | GIE);       // LPM3 w/ interrupt
}
