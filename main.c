#include "bn_utils.h"
#include "montgomery.h"


int main(){
    BIGNUM* m = BN_new();
    BIGNUM* e = BN_new();
    BIGNUM* n = BN_new();
    get_random_prime(21, m);
    get_random_prime(23, e);
    get_random_prime(22, n);
    BIGNUM* result = BN_new();
    printf("m,e,n, binary result:\n");
    mod_exp_bin(result, m, e, n);
    print_bn(m);
    print_bn(e);
    print_bn(n);
    print_bn(result);

    printf("m,e,n, montgomery result:\n");
    mod_exp_montgomery(result, m, e, n);
    print_bn(m);
    print_bn(e);
    print_bn(n);
    print_bn(result);
    
    /*monpro(result, m, e, n);
    printf("m,e,n, result:\n");
    print_bn(m);
    print_bn(e);
    print_bn(n);
    print_bn(result);*/
    BN_clear_free(m);
    BN_clear_free(e);
    BN_clear_free(n);

    return 0;
}
