#ifndef __RAND_H
#define __RAND_H

/* LCG constants */
#define M 49381                               // Multiplier
#define I 8643                                // Increment

unsigned int rand(void);
unsigned int prand(unsigned int state);

#endif /* __RAND_H */
