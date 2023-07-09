#include "utils.hpp"


void get_times_vector( std::vector<time_t>& times, struct tm* start_time, struct tm* end_time, int num_blocks) {
    time_t start = mktime(start_time);
    time_t end = mktime(end_time);
    int gap = 0;
    int index_block = 0;
    int day = 0;
    while ( start + gap + 2*60*60 <= end ) // start + gap is the scheadule block
    {
        if ( index_block == num_blocks )
        {
            day ++;
            start =  mktime(start_time) + day*60*60*24;
            index_block = 0;
            gap = 0;
        }
        times.push_back(start + gap);
        gap += 2*60*60;
        index_block++;
    }
}

void get_variables(std::vector<variable>& variables, nlohmann::json::array_t teams, std::vector<time_t> times ) {
    for (int i = 0; i < teams.size(); ++i )
    {
        for (int j = 0; j < teams.size(); ++j )
        {
            if (teams[i] != teams[j])
            {                
                for (int t = 0; t < times.size(); t++)
                {
                    variable v;
                    v.local_team = i;
                    v.visiting_team = j;
                    v.time_index = t;
                    variables.push_back(v);
                }
            }
        }
    }
}

void test_variables_accessing( std::vector<variable> variables) {
    for (int i = 0; i < variables.size(); i++)
    {
        std::cout << "Local: " << variables[i].local_team << '\n';
        std::cout << "Visiting: " << variables[i].visiting_team << '\n';
        std::cout << "time: " << variables[i].time_index << '\n';
    }
}