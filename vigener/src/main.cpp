#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

using namespace std;

map<string, int> freq;
size_t len;


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

void find_distances(const char* text, int d){
    int pos = 0;
    int max_index = len - 2*d;
    string substr;
    for(int i=0; i<d; i++){
        substr.append("0");
    }

    cout << "length:" << len << " max_index:" << max_index << "\n";

    for(int pos = 0; pos < max_index; pos++){
        for(int i = 0; i<d; i++){
            substr[i] = text[i+pos];
        }
        cout << "comparing[" << substr  << "]\n";
        if(freq[substr] == 0){
            freq[substr] = 1;
        }
        else{
            cout << "been here[" << substr << "]\n";
        }
    }
}

size_t kaisisky_test(const char* text){
    map<string, int>freq;
    for(int i = 3; i < 4; i++){
        find_distances(text, i);
    }
}

int main(int argc, char* argv[]) {
    size_t length = strlen(argv[1]);
    char* temp = (char*)malloc(sizeof(char)*length);
    
    len = remove_unnecessary(argv[1], temp, length);

    kaisisky_test(temp);

    free(temp);
    return 0;
}
