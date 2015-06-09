//#include "bn_utils.h"
//#include "montgomery.h"
#include "diffie_hellman.h"


int main(int argc, char** argv){
    /*
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
    
    BN_clear_free(m);
    BN_clear_free(e);
    BN_clear_free(n);
    */
    if(argc <= 1){
        diffie_hellman(0, 1024);
    }
    else if (argc == 2){
        int nbits;
        nbits = atoi(argv[1]);
        diffie_hellman(0, nbits);
    }
    else if (argc == 3){
        int nbits;
        nbits = atoi(argv[2]);
        diffie_hellman(1, nbits);
    }
    else{
        printf("\nUsage: %s \n or %s <num_bits> : 1024/2048 bits\n or %s 1 <numbits> : Given number of bits.", argv[0], argv[0], argv[0]);
    }

    return 0;
}
