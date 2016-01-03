## Diffie-Hellman protocol implementation on Udoo board

[Diffie-Hellman](https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange) is a security protocol for key exchange. 
This implementation compares different exponentiation methods for DH protocol on [Udoo](www.udoo.org) development board running Ubuntu 12.04.

Three methods of exponentiation are used:  
1. Binary Exponeniation method  
2. Montgomery Exponentiation method  
3. OpenSSL implementation of exponentiation  
We used BigNum (bn.h) provided by OpenSSL library for handling large integers.


### Usage:
```
make clean
make
./main
```
