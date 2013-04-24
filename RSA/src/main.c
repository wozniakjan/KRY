/**
 * 2. projekt do predmetu KRY - Implementace a prolomeni RSA
 * autor: xwozni00, Jan Wozniak
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>  
#include <msieve.h>

//seed length for random generator
#define SEED_SIZE 1024

//#define NEXTPRIME mpz_nextprime
//#define INVERT mpz_invert
//#define POWM mpz_powm
#define NEXTPRIME nextprime
#define INVERT invert
#define POWM powm

//global variables
gmp_randstate_t random_state; //state of GMP random generator

//RSA key structure
typedef struct {
    mpz_t p;
    mpz_t q; 
    mpz_t n; 
    mpz_t e; 
    mpz_t d;
} Key;

//Prints usage of the program
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

//Montgomery ladder
void powm(mpz_t dst, mpz_t b, mpz_t e, mpz_t m){
	unsigned long int k = mpz_sizeinbase (e, 2); 
	mpz_t x1, x2;
	mpz_init(x1); mpz_init(x2);
	mpz_set(x1,b); mpz_pow_ui(x2,b,2);
	int ni;

	for(unsigned long int i=k-2; i>0; i--){
		ni = mpz_tstbit(e,i);
		if(ni == 0){
			mpz_mul(x2,x1,x2);
			mpz_mul(x1,x1,x1);
		} else {
			mpz_mul(x1, x1, x2);
			mpz_mul(x2, x2, x2);
		}
		mpz_mod(x1,x1,m);
		mpz_mod(x2,x2,m);
	}
	//and for i == 0 last loop
	ni = mpz_tstbit(e,0);
	if(ni == 0){
		mpz_mul(x1,x1,x1);
	} else {
		mpz_mul(x1, x1, x2);
	}
	mpz_mod(dst,x1,m);
}

//Miller Rabin test for probable primality
int millerrabin (mpz_t n, int k) {
	unsigned long int s = 0;
	mpz_t d, a, r_max, x, n1;
	mpz_init(d); mpz_init(a); mpz_init(r_max); mpz_init(x); mpz_init(n1); 
	mpz_sub_ui(r_max,n,4);

	mpz_sub_ui(n1,n,1);	
	mpz_set(d,n1); 

	//n-1 = 2^s * d
	do{	
		mpz_tdiv_q_2exp(d, d, 1);
		s++;
	} while(mpz_even_p(d));

	int loop;
	for(int i = 0; i<k; i++){
		loop = 0;
		//a in <2; n-2>
		mpz_urandomm(a, random_state, r_max);
		mpz_add_ui(a,a,2);

		POWM(x, a, d, n);	
		if(mpz_cmp_ui(x,1) == 0 || mpz_cmp(x,n1) == 0)
			continue;

		for(unsigned long int r = 1; r<s-1; r++){
			mpz_mul(x, x, x);
			mpz_mod(x, x, n);
			if(mpz_cmp_ui(x,1) == 0){
				return 0;
			}	
			if(mpz_cmp(x,n1) == 0){
				loop = 1;
				break;
			}
		}
		if(loop == 0)
			return 0;
	}
	return 1;
}

//gets next prime number from certain number
void nextprime (mpz_t dst, mpz_t src){
	mpz_add_ui(dst, src, 2);
	while(!mpz_millerrabin(dst,20)){
		mpz_add_ui(dst,dst,2);
	}
}

//initialize random generators
void init_random(){
	gmp_randinit_default(random_state);
	unsigned char buff[SEED_SIZE];
    srand(time(NULL));
    for(int i=0; i<SEED_SIZE; i++){
        buff[i] = rand() % 0xFF;
    }

	mpz_t seed; mpz_init(seed);
	mpz_import(seed, SEED_SIZE, 1, sizeof(buff[0]), 0, 0, buff);

	gmp_randseed(random_state, seed);
}

//sets random value
void set_random(mpz_t val, int byte_count){
    unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char)*byte_count);
    for(int i=0; i<byte_count; i++){
        buff[i] = rand() % 0xFF;//0x2B;//
    }

    //heuristics for easier prime generation
    buff[0] |= 0xC0;
    buff[byte_count-1] |= 0x01;

    //set the buffer as integer
    mpz_import(val, byte_count, 1, sizeof(buff[0]), 0, 0, buff);
	free(buff);
}

//soulution for g = ax + by -- modular multiplicative inverse 
void extend_gcd(mpz_t x, mpz_t a, mpz_t b){
	mpz_t y, last_x, last_y, q, tmp_a, tmp_b, tmp;
	mpz_init(y); mpz_init(last_x); mpz_init(last_y); mpz_init(q); mpz_init(tmp);
	mpz_init(tmp_a); mpz_set(tmp_a, a); mpz_init(tmp_b); mpz_set(tmp_b, b);
	
	mpz_set_ui(x,0); mpz_set_ui(y,1); mpz_set_ui(last_x, 1); mpz_set_ui(last_y, 0);
	while(mpz_cmp_ui(tmp_b,0) != 0){
		//q = a div b
		mpz_div(q, tmp_a, tmp_b);
		
		//(a,b) = (b, a mod b)
		mpz_set(tmp, tmp_a);
		mpz_set(tmp_a, tmp_b);
		mpz_mod(tmp_b, tmp, tmp_b);

		//(x, lastx) = (lastx - q*x, x)
		mpz_mul(tmp, q, x);
		mpz_sub(tmp, last_x, tmp);
		mpz_set(last_x, x);
		mpz_set(x, tmp);

		//(y, lasty) = (lasty - q*y, y)
		mpz_mul(tmp, q, y);
		mpz_sub(tmp, last_y, tmp);
		mpz_set(last_y, y);
		mpz_set(y, tmp);
	}	

	mpz_set(x, last_x);
	mpz_set(y, last_y);
}

//sets the multiplicative inverse
void invert(mpz_t dst, mpz_t x, mpz_t n){
	mpz_t gcd, tmp;
	mpz_init(gcd); mpz_init(tmp); 

	extend_gcd(tmp, x, n);

	if(tmp->_mp_size >= 0){
		mpz_set(dst,tmp);
	}else{
		if(n->_mp_size >= 0){
			mpz_add(dst, tmp, n);
		}else{
			mpz_sub(dst, tmp, n);
		}
	}
}

//generate random primes p, q into Key
void gen_primes(Key* k, int bit_length) {
	init_random();

    mpz_t phi, tmp1, tmp2;
    mpz_init(phi); mpz_init(tmp1); mpz_init(tmp2);
    int bytes_len_prime = bit_length/8;

	//generate p
	set_random(tmp1, bytes_len_prime);
    NEXTPRIME(k->p, tmp1);

    mpz_mod(tmp2, k->p, k->e);
    while(!mpz_cmp_ui(tmp2, 1)){
        NEXTPRIME(k->p, k->p);
        mpz_mod(tmp2, k->p, k->e);
    }

    //generate q
	set_random(tmp1, bytes_len_prime); 
    NEXTPRIME(k->q, tmp1);
    mpz_mod(tmp2, k->q, k->e);
    while(!mpz_cmp_ui(tmp2, 1)){
        NEXTPRIME(k->q, k->q);
        mpz_mod(tmp2, k->q, k->e);
    }

    // n = p * q
    mpz_mul(k->n, k->p, k->q);

    // phi(n) = (p-1)*(q-1)
    mpz_sub_ui(tmp1, k->p, 1);
    mpz_sub_ui(tmp2, k->q, 1);
    mpz_mul(phi, tmp1, tmp2);

    // d = multiplicative_inverse(e mod phi)
    INVERT(k->d, k->e, phi);
	
    //cleaning
    mpz_clear(phi); mpz_clear(tmp1); mpz_clear(tmp2);
}

//returns string in specified format for generation
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
        //gmp_sprintf(return_values,"%Zd %Zd %Zd %Zd %Zd\n",k.p, k.q, k.n, k.e, k.d);
        gmp_sprintf(return_values,"0x%Zx 0x%Zx 0x%Zx 0x%Zx 0x%Zx\n",k.p, k.q, k.n, k.e, k.d);
    }

    //cleaning
    mpz_clear(k.p); mpz_clear(k.q); mpz_clear(k.n); mpz_clear(k.e); mpz_clear(k.d);
    
    return return_values;
}

//returns string in specified format for cipher/decipher
const char* cipher(const char* E, const char* N, const char* M){
    Key k;
    mpz_init(k.p); mpz_init(k.q); mpz_init(k.n); mpz_init(k.e); mpz_init(k.d);
    mpz_init_set_str(k.n, N+2, 16); mpz_init_set_str(k.e, E+2, 16);
    mpz_t m; mpz_init(m); mpz_init_set_str(m, M+2, 16);
    mpz_t c; mpz_init(c);

    POWM(c, m, k.e, k.n);

    int len = strlen(E);
    char *buff = (char*)malloc(sizeof(char)*len); 
    gmp_sprintf(buff,"0x%Zx\n",c);

    mpz_clear(k.p); mpz_clear(k.q); mpz_clear(k.n); mpz_clear(k.e); mpz_clear(k.d);
    mpz_clear(m); mpz_clear(c);
    return buff;
}

//sets random seeds
void get_random_seeds(uint32 *seed1, uint32 *seed2) {
	uint32 tmp_seed1, tmp_seed2;
    uint64 high_res_time = read_clock();
    tmp_seed1 = ((uint32)(high_res_time >> 32) ^
             (uint32)time(NULL)) * 
             (uint32)getpid();
    tmp_seed2 = (uint32)high_res_time;

	(*seed1) = tmp_seed1 * ((uint32)40499 * 65543);
	(*seed2) = tmp_seed2 * ((uint32)40499 * 65543);
}

void factor_integer(mpz_t p, mpz_t q, char* n){
    uint32 seed1, seed2;
    get_random_seeds(&seed1, &seed2);
	uint32 flags;
	uint32 max_relations = 0;
	enum cpu_type cpu;
	uint32 cache_size1, cache_size2;
	uint32 num_threads = 0;
	uint32 which_gpu;
	const char *nfs_args = NULL;
	
	msieve_obj* factorization = msieve_obj_new(n, flags,
            NULL, NULL, NULL,
            seed1, seed2, max_relations,
            cpu, cache_size1, cache_size2,
            num_threads, which_gpu,
            nfs_args);

    if(factorization == NULL) {
        return;
    }

	msieve_run(factorization);

	if (!(factorization->flags & MSIEVE_FLAG_FACTORIZATION_DONE)) {
        return;
    }

    msieve_factor *factor = factorization->factors; 
    mpz_set_str(p, factor->number, 10);
    factor = factor->next;
    mpz_set_str(q, factor->number, 10); 
}

const char* crack(char* E, char* N, char* C){
    Key k;
    mpz_init(k.p); mpz_init(k.q); mpz_init(k.d);
    mpz_init_set_str(k.n, N+2, 16); mpz_init_set_str(k.e, E+2, 16);
    mpz_t c; mpz_init_set_str(c, C+2, 16);
    mpz_t m; mpz_init(m);
    mpz_t phi, tmp1, tmp2;
    mpz_init(phi); mpz_init(tmp1); mpz_init(tmp2);

    int n_len = strlen(N);
    char *N_str = (char*)malloc(n_len*2*sizeof(char));
    gmp_sprintf(N_str, "%Zd", k.n);

    factor_integer(k.p, k.q, N_str);
    
    // phi(n) = (p-1)*(q-1)
    mpz_sub_ui(tmp1, k.p, 1);
    mpz_sub_ui(tmp2, k.q, 1);
    mpz_mul(phi, tmp1, tmp2);
    
    // d = multiplicative_inverse(e mod phi)
    INVERT(k.d, k.e, phi);

    // decipher
    POWM(m, c, k.d, k.n);

    int len = n_len * 2 + strlen(C)*2;
    char *buff = (char*)malloc(sizeof(char)*len);
    gmp_sprintf(buff,"0x%Zx 0x%Zx 0x%Zx\n", k.p, k.q, m);

    mpz_clear(k.p); mpz_clear(k.q); mpz_clear(k.n); mpz_clear(k.e); mpz_clear(k.d);
    mpz_clear(phi); mpz_clear(tmp1); mpz_clear(tmp2); mpz_clear(c);
    if(N_str != NULL) free(N_str);

    return buff;
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
        const char* str = NULL;
        if(strcmp(argv[1],"-g")==0 && argc == 3){
            str = generate(argv[2]);
            printf("%s",str);
        }
        else if(strcmp(argv[1],"-e")==0 && argc == 5){
            str = cipher(argv[2], argv[3], argv[4]);
            printf("%s",str); 
        }
        else if(strcmp(argv[1],"-d")==0 && argc == 5){
            str = cipher(argv[2], argv[3], argv[4]);
            printf("%s",str); 
        }
        else if(strcmp(argv[1],"-b")==0 && argc == 5){
            str = crack(argv[2], argv[3], argv[4]);
            printf("%s",str); 
        }
        else{
            print_help();
        }
        //if(str != NULL) free(str);
    }

    return 0;
}
