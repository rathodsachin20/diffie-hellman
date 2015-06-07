#include "bn_utils.h"


BIGNUM* get_random_prime(int bits){
    BIGNUM *a;
    a = BN_new();

    BN_generate_prime(a, bits, 0, NULL, NULL, NULL, NULL);
    return a;
}

void print_bn(BIGNUM* bn){
    char* bn_char = BN_bn2dec(bn);
    printf("%s\n", bn_char);
}



BIGNUM* mod_exp_bin(BIGNUM* a, BIGNUM* b, BIGNUM* n){
    BIGNUM *result;
    result = BN_new();
    int len = BN_num_bytes(b);
    char* to = (char*) malloc(len*sizeof(char));
    int bin_len = BN_bn2bin(b, to);
    //printf("bin:%s\n", to);
    int i, j;
    char c;
    //int c_int;
    int bit = 0;

    //TODO:intialize: get to first bit '1', set result value, make next for loop start from next bit
    for(i=0; i<bin_len; i++){
        c = to[i];
        //c_int = (int)c;
        for (j=7; j>=0; j--){
            bit = 0;
            bit = ((c & (1<<j))!=0);
            printf("%d",bit);
            //TODO:square
            if(c&(1<<j)){ //TODO:bit is 1, multiply
            }

        }
    }

    return result;
}
