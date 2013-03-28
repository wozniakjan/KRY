#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

#define MIN_GRAM 3
#define MAX_GRAM 5
#define ALPH_SIZE 26 
#define K_P 0.067
#define K_R 0.0385

using namespace std;

map<string, int> freq;      //frekvence vyskytu konkretniho stringu
map<int, int> letter_count;		//pocty vyskytu distances mezi shodnymi stringy
int letter_freq[ALPH_SIZE]; //frekvence pismen
size_t len;
int total_found;


size_t remove_unnecessary(const char* src, char* dst, size_t len){
    size_t length = 0;
    int ii = 0;
    for(int i = 0; i<len; i++){
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

bool equal(char* s1, char* s2, int len){
	for(int i=0; i<len; i++){
		if(s1[i] != s2[i]){
			return false;
		}
	}
	return true;
}

void find_distances(char* text, int offset, int d, char* str){
    freq[str] = 1;
    int max_index = len - d, diff;
	char* substr;

    for(int pos = d+offset; pos <= max_index; pos++){
        substr = text+pos;
        //cout << "comparing:[" << str << ":" << substr << "]";
        if(equal(str,substr,d)){
            freq[str]++;
            diff = pos-offset;
            letter_count[diff]++;
			total_found++;
            //cout << " distance: " << diff;
        }
        //cout << "\n";
    }
}

void find_frequencies(char* text){
    int pos = 0;
    int max_index = len - 2*MAX_GRAM;
	char *substr_start;
	int letter_index;
    //cout << "\nlength:" << len << " max_index:" << max_index << "\n";

	//init
	for(int i = 0; i<ALPH_SIZE; i++){
		letter_freq[i] = 0;
	}

    for(int pos = 0; pos <= max_index; pos++){
		letter_index = text[pos]-'a';
		letter_freq[letter_index]++; 
		substr_start = text+pos;
		for(int d = MIN_GRAM; d<=MAX_GRAM; d++){
			if(freq[string(substr_start,d)] == 0){
				//cout << "find_distance[" << substr << "]\n";
				find_distances(text, pos, d, substr_start);
			}
			else break;
		}
    }

	for(int i=max_index; i<len; i++){
		letter_index = text[pos]-'a';
		letter_freq[letter_index]++; 
	}
}

int get_gcd(){
	map<int, int> divisors;
	int dist, num; 

	for(map<int, int>::iterator distance = letter_count.begin(); distance != letter_count.end(); distance++){
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
			//cout << "\ndivisor:" << i->first << " count:" << i->second;
			max = i->first;
		}
	}
	return max;
}

double friedman_test(){
	long sum = 0;
	double N = len*(len-1);
	for(int i = 0; i<ALPH_SIZE; i++){
		sum += letter_freq[i]*(letter_freq[i]-1); 	
	}
	//cout << "\n\nsum:"<< sum << "\n\n";
	double K_0 = sum/N;
	return (K_P-K_R)/(K_0-K_R);		
}

int kaisisky_test(char* text){
    total_found = 0;

	find_frequencies(text);
	return get_gcd();		
    //cout << "\n\ngcd:" << gcd << " tf:" << total_found <<"\n";
    //for(map<int, int>::iterator i = count.begin(); i != count.end(); ++i){
    //    cout << "distance:" << i->first << " count:" << i->second << "\n";
    //}
}

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

    cout << friedman << ";" << kaisisky << "\n"; 

    free(temp);
    return 0;
}
