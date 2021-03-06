#include "montgomery.h"

int egcd(BIGNUM* a, BIGNUM* b, BIGNUM* a_, BIGNUM* b_){
    BIGNUM *g0, *g1, *u0, *u1, *v0, *v1, *q, *tmp, *mul;
    BN_CTX * ctx = BN_CTX_new();
    g0 = BN_new();
    g1 = BN_new();
    u0 = BN_new();
    u1 = BN_new();
    v0 = BN_new();
    v1 = BN_new();
    q = BN_new();
    tmp = BN_new();
    mul = BN_new();
    BN_copy(g0, a);
    BN_copy(g1, b);
    BN_one(u0);
    BN_zero(u1);
    BN_zero(v0);
    BN_one(v1);

    while (!BN_is_zero(g1)){
        BN_div(q, NULL, g0, g1, ctx);
        BN_copy(tmp, g0);
        BN_copy(g0, g1);
        BN_mul(mul, g1, q, ctx);
        BN_sub(g1, tmp, mul);

        BN_copy(tmp, u0);
        BN_copy(u0, u1);
        BN_mul(mul, u1, q, ctx);
        BN_sub(u1, tmp, mul);

        BN_copy(tmp, v0);
        BN_copy(v0, v1);
        BN_mul(mul, v1, q, ctx);
        BN_sub(v1, tmp, mul);
    }
    BN_copy(a_, u0);
    BN_copy(b_, v0);
    BN_CTX_free(ctx);
    BN_free(g0);
    BN_free(g1);
    BN_free(u0);
    BN_free(u1);
    BN_free(v0);
    BN_free(v1);
    BN_free(tmp);
    BN_free(mul);
    return 1;
}


int egcd_mod(BIGNUM* r, BIGNUM* n, BIGNUM* rp, BIGNUM* np){
    egcd(r, n, rp, np);
    if(rp->neg){
        BN_add(rp, rp, n);
        BN_sub(np, np, r);
    }
    BN_set_negative(np, 0);
    return 1;
}

int egcd_mod2(BIGNUM* r, BIGNUM* n, BIGNUM* rp, BIGNUM* np, BN_CTX *ctx){
    BN_mod_inverse(rp, r, n, ctx);  // rp = r_inv (mod n)
    BN_mul(np, r, rp, ctx);  // r*r_inv
    BN_sub_word(np, 1);  // r*r_inv - 1
    BN_div(np, NULL, np, n, ctx);
}

int get_r_from_n(BIGNUM* r, BIGNUM* n){
    BN_one(r);
    int len = BN_num_bits(n);
    BN_lshift(r, r, len);
    return 1;
}

int monpro(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n, BIGNUM* r, BIGNUM* n_, BIGNUM* r_, BN_CTX * ctx){
    BIGNUM *t;
    t = BN_new();
    int len = BN_num_bits(n);

    BN_mul(t, a, b, ctx);
    BN_mul(result, t, n_, ctx);
    BN_mod(result, result, r, ctx);

    BN_mul(result, result, n, ctx);  // u = m*n, m=t*n' (mod r)
    BN_add(result, result, t);       // u = m*n +t
    BN_rshift(result, result, len); // just shift for division, as r is power of 2  // u = (mn+t) / r

    if(BN_cmp(result, n) >= 0){   // u>n
        BN_sub(result, result, n);
    }

    BN_free(t);
    return 1;
}

int mod_mult_montgomery(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n, BIGNUM* r, BIGNUM* n_, BIGNUM* r_, BN_CTX *ctx){
    BIGNUM *a_, *one;
    a_ = BN_new();
    one = BN_new();
    BN_one(one);

    BN_mod_mul(a_, a, r, n, ctx);
    BN_mod_mul(result, b, r, n, ctx);
    monpro(result, a_, result, n, r, n_, r_, ctx);
    monpro(result, result, one, n, r, n_, r_, ctx);
    
    BN_free(a_);
}

int mod_exp_montgomery(BIGNUM* result, BIGNUM* m, BIGNUM* e, BIGNUM* n){
    BIGNUM *r, *m_, *c_, *one, *r_, *n_;
    r = BN_new();
    r_ = BN_new();
    n_ = BN_new();
    m_ = BN_new();
    c_ = BN_new();
    one = BN_new();
    BN_one(one);
    BN_CTX * ctx = BN_CTX_new();

    // Get r from n
    get_r_from_n(r, n);
    // Get n'
    //egcd_mod(r, n, r_, n_);
    egcd_mod2(r, n, r_, n_, ctx);
    //mod_mult_montgomery(m_, m, r, n, r, n_, r_, ctx);
    BN_mod_mul(m_, m, r, n, ctx);  // m_ = m*r (mod n)
    BN_mod(c_, r, n, ctx);   // c_ = 1*r (mod n)

    int len = BN_num_bytes(e);
    char* to = (char*) malloc(len*sizeof(char));
    int bin_len = BN_bn2bin(e, to);
    char c;
    int i, j;

    i = 0;
    // Skip 0 bytes in the beginning, if any
    while(to[i]=='0'){
        i++;
        if(i==bin_len)
            break;
    }

    // Go to first non-zero bit in first non-zero byte
    c = to[i];
    for(j=7; j>=0; j--){
        if(c && (1<<j)){
            break;
        }
    }

    //printf("to[i]:%d, i:%d, j:%d\n", (int)to[i], i, j);
    for(; i<bin_len; i++){
        c = to[i];
        for(; j>=0; j--){
            monpro(c_, c_, c_, n, r, n_, r_, ctx);
            if(c & (1<<j)){
                monpro(c_, m_, c_, n, r, n_, r_, ctx);
            }
        }
        j = 7;
    }
    monpro(result, c_, one, n, r, n_, r_, ctx);

    BN_CTX_free(ctx);
    BN_free(r);
    BN_free(r_);
    BN_free(n_);
    BN_free(m_);
    BN_free(c_);
    BN_free(one);
    return 1;
}

