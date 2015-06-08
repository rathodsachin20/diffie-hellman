#include "bn_utils.h"


int main(){
    BIGNUM* m = BN_new();
    BIGNUM* e = BN_new();
    BIGNUM* n = BN_new();
    get_random_prime(32, m);
    get_random_prime(32, e);
    get_random_prime(32, n);
    BIGNUM* result = BN_new();
    mod_exp_bin(result, m, e, n);
    printf("m,e,n, result:\n");
    print_bn(m);
    print_bn(e);
    print_bn(n);
    print_bn(result);

    return 0;
}
