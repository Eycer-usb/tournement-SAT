#include <iostream>
#include <fstream>
#include <json/json.h>
#include <time.h>

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
    std::ifstream input_file(infile, std::ifstream::binary);
    Json::Value content;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    try {
        input_file >> content;
        // std::cout << content;
    }
    catch(Json::RuntimeError) {
        std::cout << "An error occurred reading input file." << "\n\n";
        exit(EXIT_FAILURE);
    }

    
    /*
        Creating Variables Abstraction
        The CNF variables are the index of the V vector from 1 to the lenght of V
    */

    // Storing time structures
    struct tm start_date{}, end_date{}, start_time{}, end_time{};

    const char* buf = Json::writeString(builder, content["start_date"]).c_str() + 1;
    strptime( buf ,"%Y-%m-%d",  &start_date);
    buf = Json::writeString(builder, content["end_date"]).c_str() + 1;
    strptime( buf ,"%Y-%m-%d",  &end_date);
    buf = Json::writeString(builder, content["start_time"]).c_str() + 1;
    strptime( buf ,"%H:%M:%S",  &start_time);
    buf = Json::writeString(builder, content["end_time"]).c_str() + 1;
    strptime( buf ,"%H:%M:%S",  &end_time);



   return 0;
}


void welcome() {
    printf("\nTournement SAT scheadule generator 1.0 -- Using Glucose SAT solver\n");
    printf("\nAuthors:\n\t- Eros Cedeño\n\t- Leonel Guerrero\n\n");
}

void print_help(char* pwd) {
    printf("\nUSAGE:\n\t%s <JSON-input-file> <output-file>\n", pwd);
}