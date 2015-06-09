#include "bn_utils.h"


int get_random_prime(int bits, BIGNUM* a, int safe){
    //BIGNUM *a;
    //a = BN_new();
    if(safe)
        safe = 1;
    BN_generate_prime(a, bits, safe, NULL, NULL, NULL, NULL); // create 'safe' prime
    return 1;
}

void print_bn(BIGNUM* bn){
    char* bn_char = BN_bn2dec(bn);
    printf("%s\n", bn_char);
}



int mod_exp_bin(BIGNUM* result, BIGNUM* m, BIGNUM* e, BIGNUM* n){
    //BIGNUM *result;
    BN_CTX * ctx = BN_CTX_new();
    //result = BN_new();
    int len = BN_num_bytes(e);
    char* to = (char*) malloc(len*sizeof(char));
    int bin_len = BN_bn2bin(e, to);
    //printf("bin:%s\n", to);
    int i, j;
    char c;
    //int c_int;
    int bit = 0;

    //TODO:intialize: get to first bit '1', set result value, make next for loop start from next bit
    i = 0;
    while(to[i]=='0'){
        i++;
        if(i==bin_len)
            break;
    }

    // Handle first byte
    if(to[i] & 128){  // First bit is 1
        BN_copy(result, m);
    }
    else{
        BN_one(result);
    }

    c = to[i];
    for (j=6; j>=0; j--){
        BN_sqr(result, result, ctx);
        BN_mod(result, result, n, ctx);
        if(c&(1<<j)){
            BN_mul(result, result, m, ctx);
            BN_mod(result, result, n, ctx);
        }
    }
    i++;

    // Handle remaining bytes
    for(i; i<bin_len; i++){
        c = to[i];
        for (j=7; j>=0; j--){
            
            BN_sqr(result, result, ctx);
            BN_mod(result, result, n, ctx);
            if(c&(1<<j)){
                BN_mul(result, result, m, ctx);
                BN_mod(result, result, n, ctx);
            }
        }
    }

    BN_CTX_free(ctx);
    return 1;
}
