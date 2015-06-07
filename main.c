#include "bn_utils.h"


int main(){
    BIGNUM* bn;
    bn = get_random_prime(512);
    print_bn(bn);
    mod_exp_bin(NULL, bn, NULL);

    return 0;
}
