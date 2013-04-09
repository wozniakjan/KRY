/**
 * 2. projekt do predmetu KRY - Implementace a prolomeni RSA
 * autor: xwozni00, Jan Wozniak
 */

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <stdio.h>
#include <gmp.h>

using namespace std;

void print_help(){
    cout << "Implementace a prolomeni RSA\n"
         << "Generovani klicu\n"
         << "  vstup: ./kry -g B\n"
         << "  vystup: P Q N E D\n\n"
         << "Sifrovani\n"
         << "  vstup: ./kry -e E N M\n"
         << "  vystup: C\n\n"
         << "Desifrovani\n"
         << "  vstup: ./kry -d D N C\n"
         << "  vystup: M\n\n"
         << "Prolomeni RSA\n"
         << "  vstup: ./kry -b E N C\n"
         << "  vystup: P Q M\n\n"
         << "B ... pozadovana velikost verejneho modulu v bitech (napr. 1024)\n"
         << "P ... prvocislo (pri generovani nahodne)\n"
         << "Q ... jine prvocislo (pri generovani nahodne)\n"
         << "N ... verejny modulus\n"
         << "E ... verejny exponent (vetsinou 3)\n"
         << "D ... soukromy exponent\n"
         << "M ... otevrena zpráva (cislo, nikoli text)\n"
         << "C ... zasifrovana zprava (cislo, nikoli text)\n"
         << "vsechna cisla na vstupu i vystupu (krome B) jsou hexadecimalni, zacinaji '0x'\n";
}

const char* generate(const char* B){
    int b = atoi(B);
    char *return_values;
    mpz_t p, q, n, e, d;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);
    
    if(b >= 4){
        int len = 2*b+1+b/4+10+6;
        return_values = (char*)malloc(sizeof(char)*(len)); 
        return_values[len-1] = '\0';
        return_values[len-2] = '\n';

        //gmp_printf ("%Zd\n", r);
    }

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);
    
    return return_values;
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
            cout << generate(argv[2]); 
        }
        else if(strcmp(argv[1],"-e")==0 && argc == 5){
            cout << "cipher\n"; 
        }
        else if(strcmp(argv[1],"-d")==0 && argc == 5){
            cout << "decipher\n"; 
        }
        else if(strcmp(argv[1],"-b")==0 && argc == 5){
            cout << "crack\n"; 
        }
        else{
            print_help();
        }
    }

    return 0;
}
