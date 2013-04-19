/**
 * 2. projekt do predmetu KRY - Implementace a prolomeni RSA
 * autor: xwozni00, Jan Wozniak
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>

typedef struct {
    mpz_t p;
    mpz_t q; 
    mpz_t n; 
    mpz_t e; 
    mpz_t d;
} Key;

void print_help(){
    printf("Implementace a prolomeni RSA\n");
    printf("Generovani klicu\n");
    printf( "  vstup: ./kry -g B\n");
    printf( "  vystup: P Q N E D\n\n");
    printf( "Sifrovani\n");
    printf( "  vstup: ./kry -e E N M\n");
    printf( "  vystup: C\n\n");
    printf( "Desifrovani\n");
    printf( "  vstup: ./kry -d D N C\n");
    printf( "  vystup: M\n\n");
    printf( "Prolomeni RSA\n");
    printf( "  vstup: ./kry -b E N C\n");
    printf( "  vystup: P Q M\n\n");
    printf( "B ... pozadovana velikost verejneho modulu v bitech (napr. 1024)\n");
    printf( "P ... prvocislo (pri generovani nahodne)\n");
    printf( "Q ... jine prvocislo (pri generovani nahodne)\n");
    printf( "N ... verejny modulus\n");
    printf( "E ... verejny exponent (vetsinou 3)\n");
    printf( "D ... soukromy exponent\n");
    printf( "M ... otevrena zpráva (cislo, nikoli text)\n");
    printf( "C ... zasifrovana zprava (cislo, nikoli text)\n");
    printf( "vsechna cisla na vstupu i vystupu (krome B) jsou hexadecimalni, zacinaji '0x'\n");
}

void gen_primes(Key* k, int bit_length) {
    srand(time(NULL));
    mpz_t phi, tmp1, tmp2;
    mpz_init(phi); mpz_init(tmp1); mpz_init(tmp2);
    int bytes_len_prime = bit_length/8;

    //buffer of random numbers
    unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char)*bytes_len_prime);
    for(int i=0; i<bytes_len_prime; i++){
        buff[i] = rand() % 0xFF;
    }

    //heuristics for easier prime generation
    buff[0] |= 0xC0;
    buff[bytes_len_prime-1] |= 0x01;

    //set the buffer as integer
    mpz_import(tmp1, bytes_len_prime, 1, sizeof(buff[0]), 0, 0, buff);
    //pick the next prime from that number
    mpz_nextprime(k->p, tmp1);

    mpz_mod(tmp2, k->p, k->e);
    while(!mpz_cmp_ui(tmp2, 1)){
        mpz_nextprime(k->p, k->p);
        mpz_mod(tmp2, k->p, k->e);
    }

    //generate q
    for(int i = 0; i < bytes_len_prime; i++)
        buff[i] = rand() % 0xFF; 
    buff[0] |= 0xC0;
    buff[bytes_len_prime - 1] |= 0x01;
    mpz_import(tmp1, bytes_len_prime, 1, sizeof(buff[0]), 0, 0, buff);
    
    // Pick the next prime starting from that random number
    mpz_nextprime(k->q, tmp1);
    mpz_mod(tmp2, k->q, k->e);
    while(!mpz_cmp_ui(tmp2, 1)){
        mpz_nextprime(k->q, k->q);
        mpz_mod(tmp2, k->q, k->e);
    }

    // n = p * q
    mpz_mul(k->n, k->p, k->q);

    // phi(n) = (p-1)*(q-1)
    mpz_sub_ui(tmp1, k->p, 1);
    mpz_sub_ui(tmp2, k->q, 1);
    mpz_mul(phi, tmp1, tmp2);

    // d = multiplicative_inverse(e mod phi)
    if(mpz_invert(k->d, k->e, phi) == 0){
        mpz_gcd(tmp1, k->e, phi);
        printf("invert failed");
    }

    //cleaning
    free(buff);
    mpz_clear(phi); mpz_clear(tmp1); mpz_clear(tmp2);
}

const char* generate(const char* B){
    //number of bites of bites for modulus
    int b = atoi(B); 
   
    //string output
    char *return_values;
    
    //random prime numbers p,q; modulus n; pub exponent e; priv exp d
    Key k;
    mpz_init(k.p); mpz_init(k.q); mpz_init(k.n); mpz_init(k.e); mpz_init(k.d);
    mpz_set_ui(k.e, 3);

    gen_primes(&k, b);
   
    //transforming generated values to output string
    if(b >= 4){
        int len = b*5;
        return_values = (char*)malloc(sizeof(char)*(len)); 
        //gmp_sprintf(return_values,"0x%Zd 0x%Zd 0x%Zd 0x%Zd 0x%Zd\n",k.p, k.q, k.n, k.e, k.d);
        gmp_sprintf(return_values,"0x%Zx 0x%Zx 0x%Zx 0x%Zx 0x%Zx\n",k.p, k.q, k.n, k.e, k.d);
    }

    //cleaning
    mpz_clear(k.p); mpz_clear(k.q); mpz_clear(k.n); mpz_clear(k.e); mpz_clear(k.d);
    
    return return_values;
}

const char* cipher(const char* E, const char* N, const char* M){
    //convert E, N, M
    //mpz_powm(C, M, kp.e, kp.n);
    return "0x0\n";
}

const char* decipher(const char* D, const char* N, const char* C){
    return "0x0\n";
}

/////////////////////////////////////////
//               MAIN                  //
/////////////////////////////////////////
int main(int argc, char* argv[]) {
    //parse command line
    if(argc<=1){
        print_help();
    }
    else {
        if(strcmp(argv[1],"-g")==0 && argc == 3){
            printf("%s",generate(argv[2])); 
        }
        else if(strcmp(argv[1],"-e")==0 && argc == 5){
            printf("%s",cipher(argv[2], argv[3], argv[4])); 
        }
        else if(strcmp(argv[1],"-d")==0 && argc == 5){
            printf("%s",decipher(argv[2], argv[3], argv[4])); 
        }
        else if(strcmp(argv[1],"-b")==0 && argc == 5){
            printf("crack\n"); 
        }
        else{
            print_help();
        }
    }

    return 0;
}
