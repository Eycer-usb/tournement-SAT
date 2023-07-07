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
    struct tm start_date{}, end_date{};

    const char* buf = Json::writeString(builder, content["start_date"]).c_str() + 1;
    printf("%s\n", buf);
    strptime( buf ,"%Y-%m-%d",  &start_date);
    printf( "%d-%d-%d\n",  start_date.tm_year, start_date.tm_mon, start_date.tm_mday);

    // buf = (char*) Json::writeString(builder, content["end_date"]).c_str();
    // strptime( buf,"%Y-%m-%d",  &end_date);
    // strftime(buf, 11, "%Y/%m/%d", &end_date);
    // printf( "%s\n",  buf);
   return 0;
}


void welcome() {
    printf("\nTournement SAT scheadule generator 1.0 -- Using Glucose SAT solver\n");
    printf("\nAuthors:\n\t- Eros Cedeño\n\t- Leonel Guerrero\n\n");
}

void print_help(char* pwd) {
    printf("\nUSAGE:\n\t%s <JSON-input-file> <output-file>\n", pwd);
}