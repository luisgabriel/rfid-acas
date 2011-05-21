#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <list>

#define FILE_DOES_NOT_EXIST 44
#define NOT_ENOUGH_IDS 46
#define OK 100

using namespace std;

/*
                TODO
                
  1. Clean the code (delete unnecessary comments)
*/


struct Tag {
	string id;

	Tag(string id) : id(id) {};

	bool match(const string &msg){
		bool result = true;
		for(int i = 0; i < msg.size() && result; i++){
			if(this->id[i] != msg[i])
				result = false;
		}
		return result;
	}
};

int slots = 0, collision_slots = 0, iddle_slots = 0;

int read(list<Tag *> &tag_set, int num_tags, const char* input_file_path){
    ifstream input(input_file_path, ios::in);
    if(input){
        int i;
        string tag_id;
        for(i = 0; i < num_tags && !input.eof(); i++){
            input >> tag_id;
            tag_set.push_back(new Tag(tag_id));
        }
        input.close();
        
        return i < num_tags ? NOT_ENOUGH_IDS : OK; 
    }
    else 
        return FILE_DOES_NOT_EXIST;
}

void run_qt(list<Tag *> &tag_set, const string &query){
	//cout << "> Query: " << query << endl;
	//cout << "Responses: ";

	int responses = 0;
	list<Tag *>::iterator it, last;
	for(it = tag_set.begin(); it != tag_set.end(); ++it){
		if((*it)->match(query)){
			responses++;
			last = it;
			//cout << (*it)->id << "  ";
		}
	}

	if(responses > 1){
		//cout << endl;
		//cout << "Result: Collision." << endl;
        collision_slots++;
		run_qt(tag_set, query + "0");
		run_qt(tag_set, query + "1");
	}
	else if(responses == 1){
		//cout << endl;
		//cout << "Result: Tag identified." << endl;
		Tag * identified = *last;
		tag_set.erase(last);
		delete identified;
	}
	else {
        //cout << endl;
		//cout << "Result: No response." << endl;
		iddle_slots++;
	}
	
	slots++;
}

void process(int num_tags, list<Tag *> &tag_set, const char* output_file_path){
	run_qt(tag_set, "0");
	run_qt(tag_set, "1");
	
	ofstream output(output_file_path, fstream::app);
	char line[50];
	sprintf(line, "%10d  %10d  %10d  %10d", num_tags, slots, iddle_slots, collision_slots);
	output << line << endl;
	output.close();
}

int main(int argc, char * argv[]){
    
    if(argc == 4){
	   	list<Tag *> tag_set;
	   	int num_tags = atoi(argv[1]);
    	int status = read(tag_set, num_tags, argv[2]);
        if(status == OK)
            process(num_tags, tag_set, argv[3]);
        else if(status == NOT_ENOUGH_IDS)
            cout << "Arquivo de entrada nao contem " << num_tags << " IDs." << endl;
        else
            cout << "Arquivo de entrada inexistente." << endl;    
	}
	else {
        cout << "Numero de parametros incorreto." << endl;
        cout << "Uso correto: ./qt <num_tags> <input_file_path> <output_file_path>" << endl;
    }    
	
	return 0;
}
