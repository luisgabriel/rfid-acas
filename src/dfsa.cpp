#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <list>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "../lib/random.h"

#define LOWER_BOUND 4
#define SCHOUTE 5
#define EOM_LEE 6

using namespace std;

struct Tag {
	int id;

	Tag(int id) : id(id) {};
};

int frames = 0, slots = 0, collision_slots = 0, iddle_slots = 0;
int estimation_method = LOWER_BOUND, init_frame_size;

void read(list<Tag *> &tag_set, int num_tags, const char* estimation, int init_frame){
    for(int i = 0; i < num_tags; i++)
        tag_set.push_back(new Tag(i));
        
    if(strcmp(estimation, "lower-bound") == 0)
        estimation_method = LOWER_BOUND;
    else if(strcmp(estimation, "schoute") == 0)
        estimation_method = SCHOUTE;
    else if(strcmp(estimation, "eom-lee") == 0)
        estimation_method = EOM_LEE;
        
    init_frame_size = init_frame;
}

int eom_lee_estimation(int previous_frame_size, int collisions, int success){
    double beta = numeric_limits<double>::max();
    double gama0, gama1 = 2.0, threshold = 0.001, temp_exp;
    //int counter = 0;
    
    /*cout << "pfz: " << previous_frame_size << endl;
    cout << "col: " << collisions << endl;
    cout << "suc: " << success << endl;*/
    
    do {
        //counter++;
        gama0 = gama1;
        //cout << "- gama0: " << gama0 << endl;
        beta = previous_frame_size / (gama0*collisions + success);
        //cout << "- beta: " << beta << endl;
        temp_exp = exp(-1.0/beta);
        //cout << "- exp: " << temp_exp << endl;
        gama1 = (1.0-temp_exp) / (beta*(1.0 - ((1.0 + (1.0/beta)) * temp_exp)));   
        //cout << "- gama1: " << gama1 << endl;
        //cout << "--" << endl;
    } while(abs(gama0 - gama1) >= threshold);
    
    //cout << "frame: " << gama1*collisions << endl;

    return (int) round(gama1*collisions);
}

int estimate_frame_size(int previous_frame_size, int collisions, int iddles, int success){
    int frame_size = 0;
    
    if(estimation_method == LOWER_BOUND)
        frame_size = 2*collisions;
    else if(estimation_method == SCHOUTE)
        frame_size = (int) round(2.39*collisions);
    else if(estimation_method == EOM_LEE)
        frame_size = eom_lee_estimation(previous_frame_size, collisions, success);
    
    if(frame_size <= 0)
        frame_size = init_frame_size;

    return frame_size;
}

void run_fsa(int num_tags, list<Tag *> &tag_set){
    int num_identified = 0;
    int frame_size = init_frame_size;
    int collisions = 0, iddles = 0, success = 0;
    
    vector< vector< list<Tag *>::iterator > > frame;
    frame.resize(frame_size);
    
    while(num_identified < num_tags){
        
        int slot;
        /* sorteia o slot que cada tag vai transmitir */
        list<Tag *>::iterator current;
	    for(current = tag_set.begin(); current != tag_set.end(); ++current){
            slot = random_gen(0, frame_size-1);
            frame[slot].push_back(current);     		    
	    }
        
        collisions = 0; 
        iddles = 0;
        success = 0;
        /* processa as transmissoes em cada slot do frame */
        for(int i = 0; i < frame_size; i++){
            if(frame[i].size() == 0)
                iddles++;
            else if(frame[i].size() == 1){
                success++;
                Tag * identified = *frame[i][0];
		        tag_set.erase(frame[i][0]); // muting
		        delete identified;
            }
            else{
                collisions++;  
            } 
            slots++;   
        }
        /* atualiza os contadores globais */
        collision_slots += collisions;
        iddle_slots += iddles;
        num_identified += success;
        frames++;
        
        /* zera os frames */
        for(int i = 0; i < frame_size; i++)
            frame[i].clear();
        
        /* estima o tamanho do proximo frame */
        frame_size = estimate_frame_size(frame_size, collisions, iddles, success);
        frame.resize(frame_size);
    }
}

void process(int num_tags, list<Tag *> &tag_set){
	run_fsa(num_tags, tag_set);
        
	printf("%10d  %10d  %10d  %10d  %10d\n", num_tags, frames, slots, iddle_slots, collision_slots);
}

int main(int argc, char * argv[]){
    
    if(argc == 4){
	   	list<Tag *> tag_set;
	   	int num_tags = atoi(argv[2]);
    	read(tag_set, num_tags, argv[1], atoi(argv[3]));
        process(num_tags, tag_set);
    	return 0;
	}
	else {
        cout << "Numero de parametros incorreto." << endl;
        cout << "Uso correto: ./dfsa <estimation_method> <num_tags> <init_frame_size>" << endl;
        return 1;
    }    
}
