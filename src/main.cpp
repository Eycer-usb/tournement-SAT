/*
CI 5437 Artificial Inteligence I April-July 2023

Tournement SAT scheadule generator 1.0 -- Using Glucose SAT solver

Authors:
    - Eros Cedeño
    - Leonel Guerrero

*/

#include <iostream>
#include <fstream>
#include "../json/single_include/nlohmann/json.hpp"
#include <time.h>
#include <vector>
#include <sstream>
#include "utils.hpp"

using json = nlohmann::json;

void welcome();
void print_help(char* pwd);
void create_cnf_file( std::vector<variable> variables, std::vector<time_t> times,
                    std::string filename, int num_teams, int num_blocks_per_day );

void create_ics_file ( std::vector<variable> variables, std::string tournement_name, json::array_t teams, std::vector<time_t> times, std::string outfile );


int main(int argc, char *argv[])
{
    // Printing Welcome Message on start
    welcome();
    if(argc < 3) { // Asserting correct input arguments
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

    /* Unifying time */
    int num_blocks = (mktime(&end_time) - mktime(&start_time))/(2*60*60);
    start_time.tm_year = start_date.tm_year;
    start_time.tm_mon = start_date.tm_mon;
    start_time.tm_mday = start_date.tm_mday;
    end_time.tm_year = end_date.tm_year;
    end_time.tm_mon = end_date.tm_mon;
    end_time.tm_mday = end_date.tm_mday;

    /* Storing possibly times into the scheadule given*/
    std::vector<time_t> times;
    get_times_vector(times, &start_time, &end_time, num_blocks);

    // Getting teams or participans
    json::array_t teams = content["participants"];

    /*
    Define structure for variables and create a array of variables
    */
    std::vector<variable> variables;
    get_variables(variables, teams, times);

    // // Testing accessing to variables from vector
    // test_variables_accessing(variables);

    // Creating .cnf file and filling with restrictions
    create_cnf_file( variables, times, outfile, teams.size(), num_blocks );
    
    // Executing SAT Solver and storing output in file
    system(("./bin/glucose -model " + std::string(outfile) + ".cnf " + std::string(outfile) + ".out" + ">>/dev/null").c_str() );

    // Parsing result in ics file
    std::string tournement_name = content["tournament_name"].dump();
    create_ics_file( variables, tournement_name, teams, times, outfile );

    // Exit Message
    std::cout << "Done!\n\n";

   return 0;
}

/*
Welcome function print in stdin the version and credits
*/
void welcome() {
    printf("\nTournement SAT scheadule generator 1.0 -- Using Glucose SAT solver\n");
    printf("\nAuthors:\n\t- Eros Cedeño\n\t- Leonel Guerrero\n\n");
}

/*
Usage Specification
*/
void print_help(char* pwd) {
    printf("\nUSAGE:\n\t%s <JSON-input-file> <output-file>\n", pwd);
}

/*
Create the CNF file for the problem abstracted into the variables vector
Args:
    - variables:    A variables's vector abstraction
    - times:        Posible times of games
    - filename:     name (without extension) for the output cnf file
    - num_teams:    Number of participants
    - num_blocks_per_day:   What do you think ;) ?
*/
void create_cnf_file( std::vector<variable> variables, std::vector<time_t> times,
                    std::string filename, int num_teams, int num_blocks_per_day ) {
    // Showing starting function status message
    std::cout << "Creating CNF file: " << filename << ".cnf " <<  '\n';
    std::string f = ""; // Content to write
    int counter = 0;    // Number of clausules writen
    
    // Basic Constraints
    // At least 2 games per team
    int i = 0;
    int m = times.size();
    while ( i < variables.size())
    {
        f += std::to_string(i+1) + " ";
        // std::cout << (i + 1) % m << '\n';

        if ((i+1) % m == 0)
        {
            f += "0\n";
            counter++;
        }
        i++;        
    }

    // Exactly two match per pair
    int num = variables.size() / times.size();
    int num_times = times.size();
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num_times; j++)
        {
            for (int k = j+1; k < num_times; k++)
            {
                f += std::to_string(- (j+1 + i*num_times) ) + ' ' + std::to_string( -(k+1 + i*num_times) ) + " 0\n";
                counter++;
            }
        }
    }

    // No simultaneus games
    for (int i = 0; i < num_times; i++)
    {
        for (int j = 0; j < num - 1 ; j++)
        {
           for (int k = j+1; k < num; k++)
           {
            f += std::to_string(-(j*num_times + i + 1)) + ' ' + std::to_string( -(k*num_times + i + 1) ) + " 0\n";
            counter++;            
           }
           
        }
    }

    // Only one match of a team per day
    for (int i = 0; i < num_teams; i++)
    {
        int block_size = num_times*(num_teams - 1);
        int block_start = i*block_size;
        
        // Contrains Local vs Local
        for (int rowj = 0; rowj < num_teams - 2; rowj++)
        {
            for (int rowk = rowj + 1; rowk < num_teams - 1 ; rowk++)
            {
                for (int day = 0; day < num_times/num_blocks_per_day; day++)
                {
                    int ej = block_start + rowj*num_times + day*num_blocks_per_day;
                    int ek = block_start + rowk*num_times + day*num_blocks_per_day;
                    for (int u = 0; u < num_blocks_per_day; u++)
                    {
                        for (int v = 0; v < num_blocks_per_day; v++)
                        {
                            f += std::to_string( -(ej + 1 + u)) + ' ' + std::to_string( -(ek + 1 + v) ) + " 0\n";
                            counter++;
                        }
                        
                    }
                }
            }
        }

        // Contrains Local vs Visiting
        for (int j = block_start; j < block_start + block_size ; j++)
        {
            int day = (j%num_times)/num_blocks_per_day;
            for (int k = 0; k < num_teams; k++)
            {
                int visiting_block_start = k*block_size;
                int visiting_match;
                if (k < i)
                {
                    visiting_match = visiting_block_start + (i-1)*num_times + num_blocks_per_day*day;
                }
                if( k > i)
                {
                    visiting_match = visiting_block_start + i*num_times + num_blocks_per_day*day;
                }
                if( k != i )
                {
                    for (int s = visiting_match; s < visiting_match + num_blocks_per_day; s++)
                    {
                        f += std::to_string(-( j + 1 )) + ' ' + std::to_string( -(s + 1) ) + " 0\n";
                        counter++;
                    }
                }
                                
            }
        }
        
        // Contrains Visiting vs Visiting
        // Selecting 2 Blocks and one day
        for (int j = 0; j < num_teams - 1; j++)
        {
            if( i != j )  {
                for (int k = j+1; k < num_teams; k++)
                {
                    if ( i != k)
                    {
                        for (int day = 0; day < num_times/num_blocks_per_day; day++)
                        {                           
                            // Getting rows positions in each block
                            int rowj;
                            int rowk;
                            if (j < i)
                            {
                                rowj = j*block_size + (i-1)*num_times + num_blocks_per_day*day;
                            }
                            if( j > i)
                            {
                                rowj = j*block_size + i*num_times + num_blocks_per_day*day;
                            }
                            if (k < i)
                            {
                                rowk = k*block_size + (i-1)*num_times + num_blocks_per_day*day;
                            }
                            if( k > i)
                            {
                                rowk = k*block_size + i*num_times + num_blocks_per_day*day;
                            }
                            for (int u = 0; u < num_blocks_per_day; u++)
                            {
                                for (int v = 0; v < num_blocks_per_day; v++)
                                {
                                    f += std::to_string(- (rowj + u + 1)) + ' ' + std::to_string(-(rowk + v + 1)) + " 0\n";
                                    counter++;
                                }
                            }
                        }
                    }
                }
                
            }
        }
    }

    // No local or visiting games of same team in consecutive days
    for (int i = 0; i < num_teams; i++)
    {
        int block_size = num_times*(num_teams - 1);
        int block_start = i*block_size;
        
        // Contrains Local vs Local
        for (int rowj = 0; rowj < num_teams - 2; rowj++)
        {
            for (int rowk = rowj + 1; rowk < num_teams - 1 ; rowk++)
            {
                for (int day = 0; day < num_times/num_blocks_per_day - 1; day++)
                {
                    int ej = block_start + rowj*num_times + day*num_blocks_per_day;
                    int ek = block_start + rowk*num_times + (day+1)*num_blocks_per_day;
                    for (int u = 0; u < num_blocks_per_day; u++)
                    {
                        for (int v = 0; v < num_blocks_per_day; v++)
                        {
                            f += std::to_string(-(ej + 1 + u)) + ' ' + std::to_string(-(ek + 1 + v)) + " 0\n";
                            counter++;

                        }
                        
                    }
                }
            }
        }
        
        // Contrains Visiting and Visiting
        // Selecting 2 Blocks and one day
        for (int j = 0; j < num_teams - 1; j++)
        {
            if( i != j )  {
                for (int k = j+1; k < num_teams; k++)
                {
                    if ( i != k)
                    {
                        for (int day = 0; day < num_times/num_blocks_per_day-1; day++)
                        {                           
                            // Getting rows positions in each block
                            int rowj;
                            int rowk;
                            if (j < i)
                            {
                                rowj = j*block_size + (i-1)*num_times + num_blocks_per_day*day;
                            }
                            if( j > i)
                            {
                                rowj = j*block_size + i*num_times + num_blocks_per_day*day;
                            }
                            if (k < i)
                            {
                                rowk = k*block_size + (i-1)*num_times + num_blocks_per_day*(day+1);
                            }
                            if( k > i)
                            {
                                rowk = k*block_size + i*num_times + num_blocks_per_day*(day+1);
                            }
                            for (int u = 0; u < num_blocks_per_day; u++)
                            {
                                for (int v = 0; v < num_blocks_per_day; v++)
                                {
                                    f += std::to_string(- (rowj + u + 1)) + ' ' + std::to_string(-(rowk + v + 1)) + " 0\n";
                                    counter++;
                                }
                            }
                        }
                    }
                }
                
            }
        }
    }

    // Concatenating the file' header
    f = "p cnf " + std::to_string(variables.size()) + ' ' + std::to_string(counter) +" \n" + f;
    f = "c\n" + f;
    f = "c\tGenerated CNF file for " + filename + ".ics " +  " specification\n" + f;
    f = "c\n" + f;

    // Creating and open outfile as a stream
    std::ofstream file_for_out( filename + ".cnf"); // Open the file for writing.
    if( !file_for_out.is_open() ){
        std::cout << "Error creating outputfile\n";
        exit(EXIT_FAILURE);
    }
    file_for_out << f; // Write the new file content into the file.
    file_for_out.close();
}

/*
Set the opening tags of a ics file into the output string
*/
void init_ics( std::string& output )
{
    output += "BEGIN:VCALENDAR\r\n";
    output += "PRODID:CI5437 AbrilJulio2023\r\n";
    output += "METHOD:PUBLISH\r\n";
    output += "VERSION:2.0\r\n";
}

/*
Set the closing tags of a ics file into the output string
*/
void end_ics( std::string& output )
{
    output += "END:VCALENDAR\r\n";
}

/*
Insert a new event given the certain parameters
*/
void write_ics_event( std::string& output, std::string tournement_name,
                    std::string local, std::string visiting, time_t time ) {

    char start_time[20];
    char end_time[20];
    std::strftime( start_time, 20, "%Y%m%dT%H%M%SZ", std::localtime(&time));
    time = time + 2*60*60;
    std::strftime( end_time, 20, "%Y%m%dT%H%M%SZ", std::localtime(&time));
    output += "BEGIN:VEVENT\r\n";
    output += "DTSTAMP:20230710T200500Z\r\n";
    output += "DTSTART:" + std::string(start_time) + "\r\n";
    output += "DTEND:" + std::string(end_time) + "\r\n";
    output += "SUMMARY: " + tournement_name + " - " + local + " vs " + visiting + "\r\n";
    output += "DESCRIPTION: Tournement Match - " + local + " vs " + visiting + "\r\n";
    output += "LOCATION: Local of " + local + "\r\n";
    output += "UID:" + std::string(start_time) + "\r\n";
    output += "END:VEVENT\r\n";
}

/*
Create the final ics file
Args:
    - variables:    A variables's vector abstraction
    - teams:        The json array of participants
    - times:        Posible times of games
    - outputfile:   Name for the output file (no extension)
*/
void create_ics_file ( std::vector<variable> variables, std::string tournement_name, json::array_t teams,
                        std::vector<time_t> times, std::string outfile ) {

    // Getting the .out file containing the result state or UNSAT
    std::string line;
    try
    {
        std::ifstream in( outfile + ".out" );
        getline( in, line );
        if (line == "UNSAT")
        {
            std::cout << "\nUnsatisfiable\n";
            std::cout << "Skipping ICS file creation\n";
            exit(EXIT_FAILURE);
        }
    }
    catch(const std::exception& e)
    {
        std::cout << "Error Getting " + outfile + ".out\n";
        exit(EXIT_FAILURE);

    }
    // Printing Start message 
    std::cout << "Creating ICS file "<< outfile << ".ics " <<  '\n';

    
    // Reading the vector of variables that satisfies the constrains
    std::istringstream is( line );
    std::vector<int> v( ( std::istream_iterator<int>( is ) ), ( std::istream_iterator<int>() ) );
    std::string f = "";

    // Initializing ics file
    init_ics(f);

    // Creating events
    for( std::vector<int>::iterator i = v.begin(); i != v.end(); ++i )
    {
        if( *i > 0 ){
            std::string local = teams[variables[*i-1].local_team].dump();
            std::string visiting = teams[variables[*i-1].visiting_team].dump();
            time_t t = times[variables[*i-1].time_index];
            write_ics_event( f, tournement_name, local, visiting, t );
        }
    }

    // Closing sintaxis tags
    end_ics(f);

    // Writing out the output file
    std::ofstream file_for_out( outfile + ".ics"); // Open the file for writing.
    if( !file_for_out.is_open() ){
        std::cout << "Error creating ics file\n";
        exit(EXIT_FAILURE);
    }
    file_for_out << f; // Write the new file content into the file.
    file_for_out.close();
}