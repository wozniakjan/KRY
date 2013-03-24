#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

using namespace std;

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

void find_distances(const char* text, int offset, int d, size_t len, map<string, int> &freq, string str, map<int, int> &count){
    freq[str] = 1;
    string substr;
    for(int i=0; i<d; i++){
        substr.append("0");
    }
    int max_index = len - d, diff;

    for(int pos = d+offset; pos <= max_index; pos++){
        for(int i = 0; i<d; i++){
            substr[i] = text[i+pos];
        }
        //cout << "comparing:[" << str << ":" << substr << "]";
        if(str.compare(substr) == 0){
            freq[str]++;
            diff = pos-offset;
            count[diff]++;
            //cout << " distance: " << diff;
        }
        //cout << "\n";
    }
}

void find_frequencies(const char* text, int d, size_t len, map<string, int> &freq, map<int, int> &count){
    int pos = 0;
    int max_index = len - 2*d;
    string substr;
    for(int i=0; i<d; i++){
        substr.append("0");
    }

    //cout << "\nlength:" << len << " max_index:" << max_index << "\n";

    for(int pos = 0; pos <= max_index; pos++){
        for(int i = 0; i<d; i++){
            substr[i] = text[i+pos];
        }
        if(freq[substr] == 0){
            //cout << "find_distance[" << substr << "]\n";
            find_distances(text, pos, d, len, freq, substr, count);
        }
    }
}

size_t kaisisky_test(const char* text, size_t len){
    map<string, int> freq;
    map<int, int> count;
    for(int i = 3; i < 6; i++){
        find_frequencies(text, i, len, freq, count);
    }

    cout << "\n\ncounts";
    for(map<int, int>::iterator i = count.begin(); i != count.end(); ++i){
        cout << i->first << " " << i->second << "\n";
    }
}

int main(int argc, char* argv[]) {
    size_t length = strlen(argv[1]);
    char* temp = (char*)malloc(sizeof(char)*length);
    
    length = remove_unnecessary(argv[1], temp, length);

    kaisisky_test(temp, length);

    free(temp);
    return 0;
}
