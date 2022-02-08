#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

typedef struct{
    int valid_bit;
    int tag_bits;
    int block_bits;
    int time;
} line;

typedef struct{
    line** lines;
    int num_lines;
} set;

typedef struct{
    set** sets;
    int num_sets;
} full_cache;

void print_line(line* l) {
    printf("valid bit is: %d, tag bit is: %d, block bits is %d, time is: %d\n", l->valid_bit, l->tag_bits, l->block_bits, l->time);
}

void print_set(set* s) {
    for (int i = 0; i < s->num_lines; i++) {
        //printf("i = %d\n", i);
        print_line(s->lines[i]);
    }
}

void print_cache(full_cache* c) {
    for (int i=0; i<c->num_sets; i++) {
        print_set(c->sets[i]);
    }
}

line *create_line(){
    line *eline = (line*)malloc(sizeof(line));
    eline->valid_bit = 0;
    eline->tag_bits = 0;
    eline->block_bits = 0; // ??
    eline->time = 0;
    return eline;
}

set *create_set(int E) {
    set* eset = (set*)malloc(sizeof(set));
    eset->num_lines = E;
    line** ls = (line**)malloc(sizeof(line**));
    for(int i = 0; i < E; i++) {
        ls[i] = create_line();
        //printf("line valid bit: %d, line tag bit: %d\n", ls[i]->valid_bit, ls[i]->tag_bits);
    }
    eset->lines = ls;
    return eset;
}

full_cache *create_cache(int E, int s){
    int S = pow(2, s);
    printf("S (number of sets) = %d\n", S);
    full_cache* cache = (full_cache*)malloc(sizeof(full_cache));
    cache->num_sets = S;
    set** ss = (set**)malloc(sizeof(set*) * S);
    //ss.num_lines = E;
    for(int i = 0; i < S; i++){
        //printf("i = %d\n", i); // problem here. 
        ss[i] = create_set(E);
    }
    cache->sets = ss;
    return cache;
}

void free_line(line* l){
    free(l);
}

void free_set(set* s, int E){
    for(int i = 0; i < E; i++){
        free_line(s->lines[i]);
    }
}

void free_cache(full_cache* c, int S, int E){
    for(int i = 0; i < S; i++){
        free_set(c->sets[i], E);
    }
}

void test() {
    printf("line test:\n");
    line* l1 = create_line(); 
    print_line(l1); 
    printf("set test, E = 2:\n");
    set* s1 = create_set(2);
    print_set(s1);
    printf("cache test, E = 2, s = 2, S = 4:\n");
    full_cache* c1 = create_cache(2, 2);
    print_cache(c1);
    printf("test over\n");
}

int getBlockBits(long address, int b)
{
    long mask = 0;
    for(int i =0; i < b; i++)
    {
        mask = mask<<1;
        mask = mask+1;

    }
    return (mask)&(address);
}

int getSetBits(long address, int b, int s)
{
    long mask = 0;
    for(int i =0; i < s; i++)
    {
        mask = mask<<1;
        mask = mask+1;

    }
    long midmask = mask << b;
    long addressbits = (midmask)&(address);
    return mask&(addressbits>>b);
}

int getTagBits(long address, int b, int s, int t)
{
    long mask = 0;
    for(int i =0; i < t; i++)
    {
        mask = mask<<1;
        mask = mask+1;

    }
    long tagmask = mask << (b+s);
    long tagbits = tagmask&address;
    return mask&(tagbits>>(b+s));
}

int hitCount = 0;
int missCount = 0;
int evictCount = 0;

/*
int set_hit(set* s, long address, int b, int s, int t) { // returns the position of the cache line with the hit
    t = getTagBits(address, b, s, t);
    s = getSetBits(address, b, s);
    for(int i = 0; i < set->num_lines; i++) { 
        if ((t == set->lines[i]) && (set->lines[i].valid_bit == 1)) {
            return i+1;
        } else {
            return 0;
        }
    }
}

int set_not_full(set*, address) { //should return the first line that is not full (now + 1);
    for(int i = 0; i < set->num_lines; i++) {
        if (set->lines[i].valid_bit == 0) {
            return i+1;
        } else {
            return 0;
        }
    }
}

int LRU(set*, address) { // returns # of LRU.
    int res, highest_time = 0;
    for (int i = 0; i < set->num_lines; i++) {
        int current_time = set->lines[i].time;
        if (current_time > highest_time) {
            highest_time = current_time;
            res = i;
        }
    }
    return res;
}

void increment_time(set*) {
    for(int i = 0; i < set->num_lines; i++) {
        set->lines[i].time++;
    }
}

void cache_sim(full_cache* c, long address, int b, int current_set, int t) {
    if (set_hit(cache->sets[s], address, b, current_set, t)) { //test for hit 
        hitCount++;
        increment_time(cache->sets[current_set]);
        set* current_pointer = cache->sets[current_set];
        current_pointer->lines[set_hit(cache->sets[s], address, b, current_set, t) - 1] = 0; // set LRU of the hit line to 0; 
        return;
    } else if (set_not_full(c->sets[current_set], address)) {
        missCount++;
        line empty_line = cache->sets[current_set].lines[set_not_full - 1];
        empty_line.valid_bit = 1; //helper function? 
        empty_line.tag_bits = t;
        empty_line.block_bits = b;
        empty_line.time = 0;
        return;
    } else {
        evictCount++;
        missCount++;
        int line_to_evict = cache->sets[current_set].lines[LRU(cache->sets[current_set], address)];
        line_to_evict.valid_bit = 1;
        line_to_evict.tag_bits = t;
        line_to_evict.block_bits = b;
        line_to_evict.time = 0;
        return;
    }
}
*/

void cache_sim(int set_number, int tbits, int E, full_cache* c){
    //printf("inside cache sim\n");
    int has_hit = 0;
    printf("after print cache, E=%d\n", E);


    set* s = c->sets[set_number];
    //printf("after setting s\n");

    print_cache(c);

    //printf("before for loop\n");

    for(int i = 0; i < E; i++){
        line* l = s->lines[i];

        //printf("valid bit for l is: %d, tag bit is: %d\n", l->valid_bit, l->tag_bits);
        //printf("after line declaration\n");
        printf("valid bit = %d\n", l->valid_bit);
        if(has_hit == 0){
            //printf("before first nested if\n");
            if((l->valid_bit) == 1){
                //printf("inside first nested if\n");
                if(l->tag_bits == tbits){
                    hitCount++;
                    l->time = 0;
                    has_hit = 1;
                } else {
                    l->time = l->time + 1;
                    printf("occupied\n");
                    // maybe a miss?
                }
                //printf("after first overall if statement\n");
            } else if(l->valid_bit == 0){
                    //printf("inside valid bit = 0 case\n");
                    missCount++;
                    l->valid_bit = 1;
                    l->tag_bits = tbits;
                    l->time = 0;
                    has_hit = 1;
                    printf("exiting validbit = 0 case\n");

            } 
            else {
                printf("\n\nINVALID VALID BIT\n\n");
            }
    } else {
        l->time = l->time + 1;
        //printf("incrementing time \n");
        }
    }

    //printf("exiting for loop\n");
    if(has_hit == 0){
        missCount++;
        int max_time = 0;
        int index_max = -1;
        for(int j = 0; j < E; j++){
            line* l = (s->lines)[j];
            if(l->time > max_time){
                max_time = l->time;
                index_max = j;
            }
        }
        printf("setting eviction line = %d \n", index_max);
        line* eviction_line = (s->lines)[index_max];
        evictCount++;
        eviction_line->valid_bit = 1;
        eviction_line->tag_bits = tbits;
        eviction_line->time = 0;
    }
           
        
}

int main(int argc, char **argv)
{
    //test();
    //stores command line arguements
    int s, E, b;
    char* tfilename;

    test();
    //stores getopt output
    int getopt_output = 0;

    // reads the command line, see below links:
    // https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    //printf("before getopt");

    while((getopt_output = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch(getopt_output){
            case 's':
                s = atoi(optarg);
                //printf("s is %d", s);
                break;
            case 'E':
                E = atoi(optarg);
                //printf("E is %d", s);
                break;
            case 'b':
                b = atoi(optarg);
                //printf("b is %d", b);
                break;
            case 't':
                tfilename = optarg;
                //printf("tfilename is %s\n", tfilename);
                break;
            default:
                fprintf(stderr, "error: parsing failed \n");
                exit(1);
        }
    }

    //opens file
    // link to learn more: https://www.programiz.com/c-programming/c-file-input-output

    //printf("FILE NAME IS: %s\n", tfilename);

    FILE *open_file = fopen(tfilename, "r");
    
    //printf("afterfile\n");

    full_cache *c = create_cache(E, s);
    
    print_cache(c);

    //printf("creating cache done\n");

    // we can potentially put some of this in a separate function

    // some info on reading file line by line: 
    // https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
    // https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
    
    //random int to tell fgets how much of a line of open_file to read
    int line_len = 1000;
    // stores the current line of open_file
    char store_line[1000];

    //int hitCount = 0;
    //int missCount = 0;
    //int evictCount = 0;
    
    //printf("before while \n");
    int count = 0;
    while(fgets(store_line, line_len, open_file)){
        // printf("in while \n");
        // parsing line using scanf, link below for where the idea came from:
        // https://stackoverflow.com/questions/49908464/reading-int-from-file-store-first-line-in-variables-in-c
        char access_type;
        long int address;
        int size;
        //printf("about to enter store line\n");

        // need if statement because format for I vs M, L, and S in file is different
        //printf("Enter store line\n");

        if(store_line[0] != 'I'){
            sscanf(store_line, " %c %ld,%d", &access_type, &address, &size);
            //printf("after scan\n");
            int set_number = getSetBits(address, b, s);
            //printf("set number is %d:\n", set_number);
            int t = 64 - (b + s);
            int tbits = getTagBits(address, b, s, t);


           //printf("tag bits is %d\n", tbits);
            
            //printf("Enter cache sim\n");

            if(access_type == 'L' || access_type == 'S'){
                cache_sim(set_number, tbits, E, c);
                //printf("after 1st cache sim\n");
            } else if(access_type == 'M'){
                cache_sim(set_number, tbits, E, c);
                cache_sim(set_number, tbits, E, c);
            }
            count++;
            //printf("%d iterations so far", count);
        }
    }

    //printf("after while loop\n");
    fclose(open_file);
    printSummary(hitCount, missCount, evictCount);
    return 0;
}
