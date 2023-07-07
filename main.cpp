#include <iostream>
#include <fstream>
#include "json/single_include/nlohmann/json.hpp"
#include <time.h>

using json = nlohmann::json;

void welcome();
void print_help(char* pwd);

int main(int argc, char *argv[])
{
    welcome();
    if(argc < 3) {
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Getting Json
    char* infile = argv[1];
    char* outfile = argv[2];
    json content;
    try {
        std::ifstream f(infile);
        content = json::parse(f);
        // std::cout << content;
    }
    catch(json::parse_error) {
        std::cout << "An error occurred reading input file." << "\n\n";
        exit(EXIT_FAILURE);
    }

    
    /*
        Creating Variables Abstraction
        The CNF variables are the index of the V vector from 1 to the lenght of V
    */

    // Storing time structures
    struct tm start_date{}, end_date{}, start_time{}, end_time{};

    strptime( content["start_date"].dump().c_str()+1 ,"%Y-%m-%d",  &start_date);
    strptime( content["end_date"].dump().c_str()+1 ,"%Y-%m-%d",  &end_date);
    strptime( content["start_time"].dump().c_str()+1 ,"%H:%M:%S",  &start_time);
    strptime( content["end_time"].dump().c_str()+1 ,"%H:%M:%S",  &start_time);

    

   return 0;
}


void welcome() {
    printf("\nTournement SAT scheadule generator 1.0 -- Using Glucose SAT solver\n");
    printf("\nAuthors:\n\t- Eros Cedeño\n\t- Leonel Guerrero\n\n");
}

void print_help(char* pwd) {
    printf("\nUSAGE:\n\t%s <JSON-input-file> <output-file>\n", pwd);
}