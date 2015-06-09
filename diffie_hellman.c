#include "diffie_hellman.h"

int diffie_hellman(int random, int nbits){
    BIGNUM *p, *g, *a, *b, *tmp, *key_alice, *key_bob;
    p = BN_new();
    g = BN_new();
    a = BN_new();
    b = BN_new();
    tmp = BN_new();
    key_alice = BN_new();
    key_bob = BN_new();
    if(random == 1){
        if(nbits < 8){
            printf("Try again with number of bits >= 8\n");
            exit(0);
        }
        printf("Generating random primes.\n");
        get_random_prime(nbits, p, 1);
        get_random_prime(nbits, g, 0);
    }
    else if(nbits <= 1024){
        nbits = 1024;
        BN_hex2bn(&p, df_p_hex1024_str); 
        BN_hex2bn(&g, df_g_hex1024_str);
    }
    else {
        nbits = 2048;
        BN_hex2bn(&p, df_p_hex1024_str); 
        BN_hex2bn(&g, df_g_hex1024_str);
    }
    printf("=========> Number of bits:%d <==========\n", nbits);
    get_random_prime(nbits, a, 0);
    get_random_prime(nbits, b, 0);

    printf("p: ");
    print_bn(p);
    printf("g: ");
    print_bn(g);
    printf("a: ");
    print_bn(a);
    printf("b: ");
    print_bn(b);
    fflush(stdout);
    // Binary Method of exponentiation
    printf("\n============= DIFFIE HELLMAN USING BINARY EXPONENTIATION =========\n");
    mod_exp_bin(tmp, g, a, p);  // g^a (mod p)
    mod_exp_bin(key_alice, tmp, b, p);  // g^a (mod p)
    printf("Alice's key:\n");
    print_bn(key_alice);

    mod_exp_bin(tmp, g, b, p);  // g^a (mod p)
    mod_exp_bin(key_bob, tmp, a, p);  // g^a (mod p)
    printf("bob's key:\n");
    print_bn(key_bob);

    if(!BN_cmp(key_alice, key_bob)){
        printf("Keys match! :)\n");
    }
    else {
        printf("Keys do not match! :(\n");
    } 
    printf("\n=================================================================\n");

    // Montgomery Method of exponentiation
    printf("\n============= DIFFIE HELLMAN USING MONTOGOMERY EXPONENTIATION ========\n");
    mod_exp_montgomery(tmp, g, a, p);  // g^a (mod p)
    mod_exp_montgomery(key_alice, tmp, b, p);  // g^a (mod p)
    printf("Alice's key:\n");
    print_bn(key_alice);

    mod_exp_montgomery(tmp, g, b, p);  // g^a (mod p)
    mod_exp_montgomery(key_bob, tmp, a, p);  // g^a (mod p)
    printf("bob's key:\n");
    print_bn(key_bob);

    if(!BN_cmp(key_alice, key_bob)){
        printf("Keys match! :)\n");
    }
    else {
        printf("Keys do not match! :(\n");
    } 
    printf("\n=================================================================\n");


    BN_clear_free(p);
    BN_clear_free(g);
    BN_clear_free(a);
    BN_clear_free(b);
    BN_clear_free(tmp);
    BN_clear_free(key_bob);
    BN_clear_free(key_alice);
   
    return 1;
}
