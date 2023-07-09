#if !defined(UTILS_H)
#define UTILS_H

#include "../json/single_include/nlohmann/json.hpp"
#include <vector>
#include <time.h>
#include <iostream>


typedef struct variable
{
    int local_team;
    int visiting_team;
    int time_index;
} variable;

void get_times_vector( std::vector<time_t>& times, struct tm* start_time, struct tm* end_time, int num_blocks);
void get_variables(std::vector<variable>& variables, nlohmann::json::array_t teams, std::vector<time_t> times );
void test_variables_accessing( std::vector<variable> variables);


#endif // UTILS_H
