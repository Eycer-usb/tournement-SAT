#include <iostream>
#include <fstream>
#include "../json/single_include/nlohmann/json.hpp"
#include <time.h>
#include <vector>
#include "utils.hpp"

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
        The CNF variables are the index of the Variables vector from 1 to the lenght of Variables
    */

    // Storing time structures
    struct tm start_date{}, end_date{}, start_time{}, end_time{};

    strptime( content["start_date"].dump().c_str()+1 ,"%Y-%m-%d",  &start_date);
    strptime( content["end_date"].dump().c_str()+1 ,"%Y-%m-%d",  &end_date);
    strptime( content["start_time"].dump().c_str()+1 ,"%H:%M:%S",  &start_time);
    strptime( content["end_time"].dump().c_str()+1 ,"%H:%M:%S",  &end_time);


    /* O'clock hours in start time */
    if (start_time.tm_min != 0) {
        start_time.tm_min = 0;
        start_time.tm_hour ++;
    }

    // std::cout << start_time.tm_hour << " " << end_time.tm_hour << "\n";


    /* Unifying time */
    int num_blocks = (mktime(&end_time) - mktime(&start_time))/(2*60*60);
    // std::cout << "Num Blocks " << num_blocks << "\n";
    start_time.tm_year = start_date.tm_year;
    start_time.tm_mon = start_date.tm_mon;
    start_time.tm_mday = start_date.tm_mday;
    end_time.tm_year = end_date.tm_year;
    end_time.tm_mon = end_date.tm_mon;
    end_time.tm_mday = end_date.tm_mday;

    /* Storing possibly times into the scheadule given*/
    std::vector<time_t> times;
    get_times_vector(times, &start_time, &end_time, num_blocks);

    // std::cout << "Times Size: " << times.size() << '\n';

    // Getting teams or participans
    json::array_t teams = content["participants"];
    // std::cout << "Number of teams: " << teams.size() << "\n";
    // std::cout << "First Participant: " << teams[0] << "\n";

    /*
    Define structure for variables and create a array of variables
    */
    
    std::vector<variable> variables;
    get_variables(variables, teams, times);

    // std::cout << "Number of variables: " << variables.size() << '\n';

    // // Testing accessing to variables from vector
    test_variables_accessing(variables);

    // Creating .cnf file and filling with restrictions
    

   return 0;
}


void welcome() {
    printf("\nTournement SAT scheadule generator 1.0 -- Using Glucose SAT solver\n");
    printf("\nAuthors:\n\t- Eros Cedeño\n\t- Leonel Guerrero\n\n");
}

void print_help(char* pwd) {
    printf("\nUSAGE:\n\t%s <JSON-input-file> <output-file>\n", pwd);
}