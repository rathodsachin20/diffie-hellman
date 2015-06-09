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
    BN_clear_free(g0);
    BN_clear_free(g1);
    BN_clear_free(u0);
    BN_clear_free(u1);
    BN_clear_free(v0);
    BN_clear_free(v1);
    BN_clear_free(tmp);
    BN_clear_free(mul);
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

int get_r_from_n(BIGNUM* r, BIGNUM* n){
    BN_one(r);
    int len = BN_num_bits(n);
    BN_lshift(r, r, len);
    return 1;
}

int monpro(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n, BIGNUM* r, BIGNUM* n_, BIGNUM* r_){
    BIGNUM *t, *u, *m, *tmp, *tmp1;
    BN_CTX * ctx = BN_CTX_new();
    t = BN_new();
    u = BN_new();
    m = BN_new();
    tmp = BN_new();
    tmp1 = BN_new();
    int len = BN_num_bits(n);

    //BN_mul(n_, n_, -1, ctx);
    //printf("r_, n_:\n");
    //print_bn(r_);
    //print_bn(n_);

    BN_mul(t, a, b, ctx);
    BN_mul(tmp, t, n_, ctx);
    BN_mod(m, tmp, r, ctx);

    BN_mul(tmp, m, n, ctx);
    BN_add(tmp, tmp, t);
    //BN_div(tmp, NULL, tmp, r, ctx);
    //print_bn(tmp);
    BN_rshift(tmp, tmp, len); // just shift for division, as r is power of 2
    //print_bn(tmp);

    BN_copy(tmp1, tmp);
    BN_sub(tmp1, tmp1, n);
    if(!(tmp1->neg)){
        BN_copy(tmp, tmp1);
    }

    BN_copy(result, tmp);

    BN_CTX_free(ctx);
    BN_clear_free(t);
    BN_clear_free(u);
    BN_clear_free(m);
    BN_clear_free(tmp);
    BN_clear_free(tmp1);
    return 1;
}

int mod_mult_montgomery(BIGNUM* result, BIGNUM* a, BIGNUM* b, BIGNUM* n, BIGNUM* r, BIGNUM* n_, BIGNUM* r_){
    BIGNUM *a_, *b_, *c_, *one;
    BN_CTX * ctx = BN_CTX_new();
    a_ = BN_new();
    b_ = BN_new();
    c_ = BN_new();
    one = BN_new();
    BN_one(one);

    BN_mod_mul(a_, a, r, n, ctx);
    BN_mod_mul(b_, b, r, n, ctx);
    monpro(c_, a_, b_, n, r, n_, r_);
    monpro(result, c_, one, n, r, n_, r_);
    
    BN_CTX_free(ctx);
    BN_clear_free(a_);
    BN_clear_free(b_);
    BN_clear_free(c_);
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
    egcd_mod(r, n, r_, n_);
    mod_mult_montgomery(m_, m, r, n, r, n_, r_);
    //BN_mod_mul(m_, m, r, n, ctx);
    //printf("m_:");
    //print_bn(m_);
    BN_mod(c_, r, n, ctx);

    int len = BN_num_bytes(e);
    char* to = (char*) malloc(len*sizeof(char));
    int bin_len = BN_bn2bin(e, to);
    char c;
    int i, j;

    i = 0;
    while(to[i]=='0'){
        i++;
        if(i==bin_len)
            break;
    }

    c = to[i];
    for(j=7; j>=0; j--){
        if(c && (1<<j)){
            break;
        }
    }

//printf("to[i]:%d, i:%d, j:%d\n", (int)to[i], i, j);
    for(i; i<bin_len; i++){
        c = to[i];
        for(j; j>=0; j--){
            monpro(c_, c_, c_, n, r, n_, r_);
            //printf("c_:");
            //print_bn(c_);
            if(c & (1<<j)){
                monpro(c_, m_, c_, n, r, n_, r_);
            }
        }
        j = 7;
    }
    monpro(result, c_, one, n, r, n_, r_);

    BN_CTX_free(ctx);
    BN_clear_free(r);
    BN_clear_free(r_);
    BN_clear_free(n_);
    BN_clear_free(m_);
    BN_clear_free(c_);
    BN_clear_free(one);
    return 1;
}

