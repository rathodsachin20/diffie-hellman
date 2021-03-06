#include "diffie_hellman.h"

int diffie_hellman(int random, int nbits){
    struct timespec tstart = {0,0}, tend = {0,0}, tdiff1={0,0}, tdiff2={0,0}, tavg1={0,0}, tavg2={0,0}, tavg3={0,0};
    BIGNUM *p, *g, *a, *b, *tmp, *key_alice, *key_bob;
    BN_CTX * ctx = BN_CTX_new();
    p = BN_new();
    g = BN_new();
    a = BN_new();
    b = BN_new();
    tmp = BN_new();
    key_alice = BN_new();
    key_bob = BN_new();
    printf("=========> Number of bits:%d <==========\n", nbits);
    if(random == 1){
        if(nbits < 8){
            printf("Try again with number of bits >= 8\n");
            exit(0);
        }
        printf("Generating random safe prime p.\n");
        get_random_prime(nbits, p, 1);
        //get_random_prime(nbits, g, 0);
        BN_hex2bn(&g, "5");
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
    printf("Generating random prime a.\n");
    fflush(stdout);
    get_random_prime(nbits, a, 0);
    printf("Generating random prime b.\n");
    fflush(stdout);
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

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    mod_exp_bin(tmp, g, a, p);  // g^a (mod p)
    mod_exp_bin(key_alice, tmp, b, p);  // g^ab (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff1 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff1);
    printf("Alice's key:\n");
    print_bn(key_alice);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    mod_exp_bin(tmp, g, b, p);  // g^a (mod p)
    mod_exp_bin(key_bob, tmp, a, p);  // g^a (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff2 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff2);
    printf("Bob's key:\n");
    print_bn(key_bob);

    tavg1 = avg(tdiff1, tdiff2);
    printf("======> Avg Time taken:");
    print_time(tavg1);
    if(!BN_cmp(key_alice, key_bob)){
        printf("Keys match! :)\n");
    }
    else {
        printf("Keys do not match! :(\n");
    } 
    //printf("\n=================================================================\n");

    // Montgomery Method of exponentiation
    printf("\n============= DIFFIE HELLMAN USING MONTOGOMERY EXPONENTIATION ========\n");
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    mod_exp_montgomery(tmp, g, a, p);  // g^a (mod p)
    mod_exp_montgomery(key_alice, tmp, b, p);  // g^a (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff1 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff1);

    printf("Alice's key:\n");
    print_bn(key_alice);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    mod_exp_montgomery(tmp, g, b, p);  // g^a (mod p)
    mod_exp_montgomery(key_bob, tmp, a, p);  // g^ab (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff2 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff2);
    printf("Bob's key:\n");
    print_bn(key_bob);

    tavg2 = avg(tdiff1, tdiff2);
    printf("======> Avg Time taken:");
    print_time(tavg2);
    if(!BN_cmp(key_alice, key_bob)){
        printf("Keys match! :)\n");
    }
    else {
        printf("Keys do not match! :(\n");
    } 
    //printf("\n=================================================================\n");

    // Openssl Method of exponentiation
    printf("\n============= DIFFIE HELLMAN USING OPENSSL EXPONENTIATION ========\n");
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    BN_mod_exp(tmp, g, a, p, ctx);  // g^a (mod p)
    BN_mod_exp(key_alice, tmp, b, p, ctx);  // g^a (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff1 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff1);

    printf("Alice's key:\n");
    print_bn(key_alice);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tstart);
    BN_mod_exp(tmp, g, b, p, ctx);  // g^a (mod p)
    BN_mod_exp(key_bob, tmp, a, p, ctx);  // g^ab (mod p)
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

    tdiff2 = diff(tstart, tend);
    printf("Time taken:");
    print_time(tdiff2);
    printf("Bob's key:\n");
    print_bn(key_bob);

    tavg3 = avg(tdiff1, tdiff2);
    printf("=======> Avg Time taken:");
    print_time(tavg3);
    if(!BN_cmp(key_alice, key_bob)){
        printf("Keys match! :)\n");
    }
    else {
        printf("Keys do not match! :(\n");
    } 
    printf("\n=================================================================\n");

    printf("======== Summary =========\n");
    printf("Keysize:%d\n", nbits);
    printf("Avg calculation times for Alice and Bob:\n");
    printf("1. Binary exponentiation:    \t");
    print_time(tavg1);
    printf("2. Montgomery exponentiation:\t");
    print_time(tavg2);
    printf("3. Openssl exponentiation:   \t");
    print_time(tavg3);

    BN_CTX_free(ctx);
    BN_clear_free(p);
    BN_clear_free(g);
    BN_clear_free(a);
    BN_clear_free(b);
    BN_clear_free(tmp);
    BN_clear_free(key_bob);
    BN_clear_free(key_alice);
   
    return 1;
}
