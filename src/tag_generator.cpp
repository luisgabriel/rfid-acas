#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>
#include "../lib/random.h"

using namespace std;

char* generate_id(int num_bits){
    char * result = new char[num_bits+1];
    for(int i = 0; i < num_bits; i++){
        result[i] = random_gen(0, 1) == 0 ? '0' : '1';
    }
    result[num_bits] = 0;
    return result;
}

void process(int num_bits, int num_tags){
    map<string, bool> created;
    for(int i = 0; i < num_tags; i++){
        string tag_id = generate_id(num_bits);
        if(!created[tag_id]){
            created[tag_id] = true;
            cout << tag_id << endl;
        }
    }
    created.clear();
}

int main(int argc, char* argv[]){
    
    if(argc == 3) {
        process(atoi(argv[1]), atoi(argv[2]));
    }
    else {
        cout << "Numero de parametros incorreto." << endl;
        cout << "Uso correto: ./tag_generator <num_bits> <num_tags>" << endl;
    }    

    return 0;
}
