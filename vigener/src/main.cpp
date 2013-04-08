/**
 * 1. projekt do predmetu KRY - Vigenerova sifra
 * autor: xwozni00, Jan Wozniak
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

//Konstanty
#define MIN_GRAM 3
#define MAX_GRAM 5
#define ALPH_SIZE 26 
#define K_P 0.0654967
#define K_R 0.03846

using namespace std;

map<string, int> freq;      //frekvence vyskytu konkretniho stringu
map<int, int> letter_count; //pocty vyskytu distances mezi shodnymi stringy
int letter_freq[ALPH_SIZE]; //frekvence pismen
size_t len;                 //delka zpracovavaneho textu
int total_found=0;          //TODO zjisti z githubu, kde jsem jeden vyskyt smazal

//pravdepodobnost vyskytu pismen anglicke abecedy v anglickem textu
float letter_table[ALPH_SIZE] = {
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 
    0.02015, 0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 
    0.02406, 0.06749, 0.07507, 0.01929, 0.00095, 0.05987, 
    0.06327, 0.09056, 0.02758, 0.00978, 0.02360, 0.00150, 
    0.01974, 0.00074 
};

/*
 * Smaze prebytecne znaky a konvertuje na lower-case
 *
 * @param src - zdrojovy text
 * @param dst - upraveny retezec
 * @param len - delka
 * @return velikost upraveneho textu
 */
size_t remove_unnecessary(const char* src, char* dst, size_t len2){
    size_t length = 0;
    int ii = 0;
    for(int i = 0; i<len2; i++){
        if(src[i] >= 'a' && src[i] <= 'z'){
            dst[ii] = src[i];
            ii++;
        }
        else if(src[i] >= 'A' && src[i] <= 'Z'){
            dst[ii] = src[i] - 'A' + 'a';
            ii++;
        }
    }
    dst[ii] = '\0';
    return ii;
}

/* 
 * Porovnani podretezcu na shodnost
 *
 * @param s1 - referencni podretezec
 * @param s2 - kontrolovany podretezec
 * @param len - delka podretezce
 * @return true pokud jsou si rovny, jinak false
 */
bool equal(char* s1, char* s2, int len){
    for(int i=0; i<len; i++){
        if(s1[i] != s2[i]){
            return false;
        }
    }
    return true;
}

/*
 * Vyhleda souhlasne podretezce s "text" a jejich vzdalenosti vyskutu ulozi do freq
 *
 * @param text - referencni podretezec
 * @param posun - v sifrovanem textu
 * @param d - delka podretezce
 * @param str - zbytek sifrovaneho textu
 */
void find_distances(char* text, int offset, int d, char* str){
    freq[str] = 1;
    int max_index = len - d, diff;
    char* substr;

    for(int pos = d+offset; pos <= max_index; pos++){
        substr = text+pos;
        if(equal(str,substr,d)){
            freq[str]++;
            diff = pos-offset;
            letter_count[diff]++;
            total_found++;
        }
    }
}

/*
 * Nastavi frekvence vyskytu substringu
 *
 * @param text - sifrovany text
 */
void find_frequencies(char* text){
    int pos = 0;
    int max_index = len - 2*MAX_GRAM;
    char *substr_start;
    int letter_index;

    memset(letter_freq, 0, ALPH_SIZE*sizeof(int));

    //ulozi frekvence vyskytu pismen
    for(int i = 0; i<len; i++){
        letter_index = text[i]-'a';
        letter_freq[letter_index]++; 
    }

    //vyskyty substringu
    for(int pos = 0; pos <= max_index; pos++){
        substr_start = text+pos;
        for(int d = MIN_GRAM; d<=MAX_GRAM; d++){
            if(freq[string(substr_start,d)] == 0){
                find_distances(text, pos, d, substr_start);
            }
            else break;
        }
    }
}

/*
 * Hleda rozumneho nejvetsiho spolecneho delitele pro vyskyty shodnych substringu
 *
 * @return - spolecneho nejvetsiho delitele
 */
int get_gcd(){
    map<int, int> divisors;
    int dist, num; 
    map<int, int>::iterator distance;
    for(distance = letter_count.begin(); distance != letter_count.end(); distance++){
        dist = distance->first;
        num = distance->second;
        for(int i = 2; i<=dist; i++){
            if(dist%i == 0) divisors[i]+=num;
        }		
    }

    int max = 1;
    int limit = total_found*2/3;
    for(map<int,int>::iterator i = divisors.begin(); i != divisors.end(); i++){
        if(i->second >= limit && max < i->first){
            max = i->first;
        }
    }
    return max;
}

/*
 * Friedmanuv test
 */
double friedman_test(){
    long sum = 0;
    double N = len*(len-1);
    for(int i = 0; i<ALPH_SIZE; i++){
        sum += letter_freq[i]*(letter_freq[i]-1); 	
    }
    double K_0 = sum/N;
    return (K_P-K_R)/(K_0-K_R);		
}

/*
 * Kaisiskeho test
 */
int kaisisky_test(char* text){
    find_frequencies(text);
    return get_gcd();		
}

/*
 * Posun v abecede s modulem
 *
 * @param char_index - pismeno
 * @param g - hodnota shift
 * @return posun
 */
int mod(int char_index, int g){
    int x = (char_index+g) % ALPH_SIZE;
    return x;
}

/*
 * Vrati rozdil mezi indexem koincidence a M_g
 *
 * @param tested - hodnota "indexu koincidence" pro shiftnutou abecedu textu
 * @return - o kolik se lisi "index koincidence" shiftnute abecedy od te anglicke
 */
double get_diff(double tested){
    double diff = tested - K_P;
    if(diff < 0)
        diff = -diff;
    return diff;
}

/*
 * Najde heslo k textu
 *
 * @param pwd - heslo, ktere bude zde nastaveno
 * @param pwd_len - delka hesla
 * @param cipher_text - sifrovany text
 */
void set_password(char* pwd, int pwd_len, char* cipher_text){
    pwd[pwd_len] = '\0';
    int cipher_letter_freq[ALPH_SIZE];
    int submsg_len;
    int letter_index;

    //zpracovavej pro kazdy podretezec textu definovane klicem
    for(int pwd_index = 0; pwd_index < pwd_len; pwd_index++){
        memset(cipher_letter_freq, 0, ALPH_SIZE*sizeof(int));
        submsg_len = 0;
        
        //najde frekvence pismen
        for(int i=pwd_index; i<len; i+=pwd_len){
            submsg_len++;
            letter_index = cipher_text[i]-'a';
            cipher_letter_freq[letter_index]++;
        }
       
        int g_index = 0;
        double mg, g_best = 100;
        //spocti nejlepsi M_g
        for(int g=0; g<ALPH_SIZE; g++){
            mg = 0;
            for(int i=0; i<ALPH_SIZE; i++){
                int l_freq = cipher_letter_freq[mod(i,g)];
                mg += (letter_table[i]*l_freq)/submsg_len;
            }

            //cim blize je posunute M_g - K_P, tim spis se jedna o anglictinu
            double diff = get_diff(mg);
            if(diff < g_best){
                g_best = diff;
                g_index = g;
            }
        }
        pwd[pwd_index] = g_index + 'a';
    }
}






/////////////////////////////////////////
//               MAIN                  //
/////////////////////////////////////////
int main(int argc, char* argv[]) {
    string line, input;
    while(getline(cin, line)){
        input += line;
    }
    size_t length = input.size();
    char* temp = (char*)malloc(sizeof(char)*length);

    len = remove_unnecessary(input.c_str(), temp, length);

    int kaisisky = kaisisky_test(temp);
    double friedman = friedman_test();

    char *password=(char*)malloc((kaisisky+1)*sizeof(char));
    set_password(password,kaisisky,temp);

    double sum = 0, ic;
    for(int i = 0; i < ALPH_SIZE; i++){
        sum += letter_table[i];
        ic += letter_table[i]*letter_table[i];
    }

    cout << friedman << ";" << kaisisky << ";" << kaisisky << ";" << password << "\n"; 

    free(password);
    free(temp);

    return 0;
}
