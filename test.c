#include <msp430.h>

#include "rand.h"

#define LED_OUT P1OUT
#define LED_DIR P1DIR

#define LED_RED   BIT0
#define LED_GREEN BIT6

#define BLINK_DELAY 1200                      // 200 ms at 6 KHz

#define BITS_RAND  16
#define BITS_PRAND 8                          // Using only the MSB of the prand state

int failure = 0;

/**
 * Set up the timers and blink!
 */
void prepare_to_blink(void) {
	BCSCTL3 |= LFXT1S_2;                      // Set LF to VLO = 12 KHz
	BCSCTL1 |= DIVA_1;                        // ACLK = LF / 2 = 6 KHz

	TACCR0 = BLINK_DELAY;                     // Set the timer
	TACTL = TASSEL_1 | MC_1;                  // TACLK = ACLK; up to CCR0
	TACCTL1 = CCIE | OUTMOD_3;                // TA1 interrupt enable; PWM set/reset

	__bis_SR_register(LPM3_bits | GIE);       // LPM3 w/ interrupt
}

void __attribute__((interrupt(TIMERA1_VECTOR))) blink_LED(void) {
	TACCTL1 &= ~CCIFG;                        // Unset interrupt flag

	if (failure)                              // Toggle LEDs
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

/* The hardware RNG is slow, so limit test to 800 bits. */
#define MONOBIT_TIMES_RAND 50                 // 800 / BITS_RAND

/* Each 8-bit number tested with monobit contributes 8 bits, so in the worst
 * case, the signed 16-bit bucket can store information about this many
 * numbers: */
#define MONOBIT_TIMES_PRAND 4095              // (2^15-1) / BITS_PRAND

/* The maximum absolute value of the sum bucket after a monobit test, where
 * 0.01 is the minimum P-value and inverfc is the inverse of the complementary
 * error function. */
#define MONOBIT_MAX_VAL_RAND  72              // inverfc(0.01) * sqrt(2) * sqrt(800)
#define MONOBIT_MAX_VAL_PRAND 466             // inverfc(0.01) * sqrt(2) * sqrt(2^15-1)

/**
 * Monobit test for rand().
 *
 * Returns 0 on success; otherwise otherwise.
 */
int monobit_rand(void) {
	int sum = 0;

	int i, j;

	for (i = 0; i < MONOBIT_TIMES_RAND; i++) {
		/* Ignore the least significant bits. */
		unsigned int r = rand() >> (16 - BITS_RAND);

		/* Add up all the bits, taking 0 to mean -1. */
		for (j = 0; j < BITS_RAND; j++) {
			sum += r & 0x1 ? 1 : -1;
			r >>= 1;
		}
	}

	if (sum < 0)
		sum = 0 - sum;                        // Absolute value

	return sum > MONOBIT_MAX_VAL_RAND;
}

/**
 * Monobit test for prand().
 *
 * Returns 0 on success; otherwise otherwise.
 */
int monobit_prand() {
	unsigned int state = rand();
	int sum = 0;

	int i, j;

	for (i = 0; i < MONOBIT_TIMES_PRAND; i++) {
		/* Ignore the least significant bits. */
		unsigned int r = state >> (16 - BITS_PRAND);

		/* Add up all the bits, taking 0 to mean -1. */
		for (j = 0; j < BITS_PRAND; j++) {
			sum += r & 0x1 ? 1 : -1;
			r >>= 1;
		}

		state = prand(state);
	}

	if (sum < 0)
		sum = 0 - sum;                        // Absolute value

	return sum > MONOBIT_MAX_VAL_PRAND;
}

/**
 * Store the failure code on the top of the stack and alternate flashing the
 * LEDs to signify failure.
 *
 * Never returns!
 */
void fail(unsigned int code) {
	__asm__ __volatile__("push %0" : : "r" (code));

	failure = 1;

	LED_OUT &= ~LED_GREEN;
	prepare_to_blink();
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

	if (monobit_rand())
		fail(0xdead);

	if (monobit_prand())
		fail(0xbeef);

	LED_OUT &= ~LED_RED;
	prepare_to_blink();
}
