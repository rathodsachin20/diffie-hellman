#ifndef __MONTGOMERY__
#define __MONTGOMERY__

#include <openssl/bn.h>
#include <stdio.h>

int egcd(BIGNUM* a, BIGNUM* b, BIGNUM* a_, BIGNUM* b_);
int egcd_mod(BIGNUM* r, BIGNUM* n, BIGNUM* rp, BIGNUM* np);
int get_r_from_n(BIGNUM* r, BIGNUM* n);
int monpro(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n);
int mod_mult_montgomery(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n);
int mod_exp_montgomery(BIGNUM* result, BIGNUM* m, BIGNUM* e, BIGNUM* n);

#endif
