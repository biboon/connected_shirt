#include "parity.h"

int checkParity(unsigned char data) {
    unsigned int b = 1, res = 0, i;
    for (i = 0; i < 8; i++)
    if (data & (b << i)) res++;
    return (count % 2) ? ;
}
