#ifndef __BN_UTILS__
#define __BN_UTILS__

#include <openssl/dh.h>
#include <openssl/bn.h>

int get_random_prime(int bits, BIGNUM* a);
void print_bn(BIGNUM* bn);
int mod_exp_bin(BIGNUM* result, BIGNUM* m, BIGNUM* e, BIGNUM* n);


#endif
