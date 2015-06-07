#ifndef __BN_UTILS__
#define __BN_UTILS__

#include <openssl/dh.h>
#include <openssl/bn.h>

BIGNUM* get_random_prime(int bits);
void print_bn(BIGNUM* bn);
BIGNUM* mod_exp_bin(BIGNUM* a, BIGNUM* b, BIGNUM* n);


#endif
