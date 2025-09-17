#include "iostream"
using namespace std;

int find_length(char str[]) {
    /**
     * To find length of string we look for delimiter
     */
    int i;
    for (i = 0; str[i] != '\0'; i++) {
    }
    return i;
}

void upper_address(char* y) {
    /**
     * Upper pass by address. Modifies original str
     */
    for (int i = 0; y[i] != '\0'; i++) {
        y[i] = y[i] - 32;
    }
}
void lower_ref(char(&y)[6]) { // pass by reference need specify size
    /**
     * Lower pass by reference. Modifies original str
     */
    for (int i = 0; y[i] != '\0'; i++) {
        y[i] = y[i] + 32;
    }
}

void flip(char* y) {
    /**
     * Flips upper and lower case
     */
    for (int i = 0; y[i] != '\0'; i++) {
        if (y[i] >= 65 && y[i] <= 90) // upper
        y[i] += 32;
        else { // lower
            y[i] -= 32;
        }
    }
}

int count_word(char* y) {
    int c = 0;
    for (int i = 0; y[i] != '\0'; i++) {
        if (y[i] == ' ' && y[i-1] != ' ') {
            c++;
        }
    }
    return c + 1;
}

int reverse(char* y) {
    int length = find_length(y);
    char* z = new char[length];
    for (int i = 0, j = length - 1; i < length; i++, j--) {
        z[i] = y[j];
    }
    for (int i = 0; i < length; i++) {
        y[i] = z[i];
    }
    y[length] = '\0';
}

bool equals(char* str1, char* str2){
    for (int i = 0, j = 0; str1[i] != '\0' && str2[j] != '\0'; i++, j++) {
        if (str1[i] != str2[j]) {
            return false;
        }
    }
    return true;
}


int main() {
    char temp = 65;
    cout << temp << endl; // prints A
    char temp2 = 'A';
    cout << temp2 << endl; // prints A

    char x[6] = {'H', 'E', 'L', 'L', 'O', '\0'};
    char y[] = {'w','O', 'r', 'L', 'd', '\0'}; // no size defined
    char z[] = {'h', 'i'}; // no delimter
    char a[3] = {'h', 'i'}; // no delimter
    cout << x << endl;
    cout << y << endl;
    cout << z << endl; // notice the bug if you do not put delimiter
    cout << a << endl; // bug fixed if size constant

    cout << "======= Length =====" << endl;
    cout << find_length(y); // 5

    cout << "======= Upper/Lower =====" << endl;
    char* s1 = new char[6]{'h', 'e', 'l', 'l', 'o', '\0'};
    cout << s1 << endl;
    upper_address(s1);
    cout << s1 << endl;
    lower_ref(x);
    cout << x << endl;
    flip(y);
    cout << y << endl;

    cout << "======= Count words =====" << endl;
    char sentence[] = {'h','i', ' ', 'h', 'i', ' ', ' ', 'h', 'i', '\0'}; // 3 words
    int word_count = count_word(sentence);
    cout << word_count << endl;

    cout << "======= Reverse =====" << endl;
    reverse(s1);
    cout << s1 << endl;

    cout << "======= Equals =====" << endl;
    cout << equals(s1, s1) << endl;
    cout << equals(x, s1) << endl;
    return 0;
}