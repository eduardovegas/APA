#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "data.h"
#include "utils.h"

int n;
int m;
int p;
int* b;
int** t;
int** c;

int* cur_capacities;

void print_solution(std::vector<std::vector<int>>& current_sol, int& current_cost)
{
    for(int i = 0; i < m+1; i++)
    {
        if(i == 0)
        {
            printf("N.A: ");
        }
        else
        {
            printf("S%d: ", i);
        }
        int jobs = current_sol[i].size();
        for(int j = 0; j < jobs; j++)
        {
            printf("%d ", current_sol[i][j]+1);
        }
        puts("");
    }
    printf("Cost: %d\n\n", current_cost);
}

void construction(std::vector<std::vector<int>>& current_sol, int& current_cost, float& alpha)
{
    std::vector<int> candidate_list(n);
    for(int j = 0; j < n; j++)
    {
        candidate_list[j] = j;
    }

    while(!candidate_list.empty())
    {
        int c_list_size = candidate_list.size();

        //unique_edge_ids = cantor_pair(job,server)
        std::vector<int> allocation_costs;
        std::vector<int> unique_edge_ids;
        //-----------------------------------------

        //all possible combination
        for(int c_idx = 0; c_idx < c_list_size; c_idx++)
        {
            int job = candidate_list[c_idx];

            //job allocated in each possible server
            for(int i = 1; i < m+1; i++)
            {
                if(cur_capacities[i-1] > t[i-1][job])
                {
                    allocation_costs.push_back(c[i-1][job]);
                    unique_edge_ids.push_back(pair(c_idx, i));
                }
            }
        }
        //------------------------

        std::vector<std::pair<int,int>> zipped;
        zip(allocation_costs, unique_edge_ids, zipped);

        //sorts in ascending order based on pair.first
        sort(zipped.begin(), zipped.end());

        unzip(allocation_costs, unique_edge_ids, zipped);

        //jobs not allocated (cost always worst)
        for(int c_idx = 0; c_idx < c_list_size; c_idx++)
        {
            allocation_costs.push_back(p);
            unique_edge_ids.push_back(pair(c_idx, 0));
        }
        //--------------------------------------

        int choosen = 0;
        int range = floor(alpha*allocation_costs.size());
        if(range != 0)
        {
            //choosen = rand([0,range))
            choosen = rand() % range;
        }

        int cost = allocation_costs[choosen];
        std::pair<int,int> inv_cantor = unpair(unique_edge_ids[choosen]);
        int c_idx = inv_cantor.first;
        int server = inv_cantor.second;
        int job = candidate_list[c_idx];

        current_sol[server].push_back(job);
        current_cost += cost;
        if(server != 0)
        {
            cur_capacities[server-1] -= t[server-1][job];
        }
        candidate_list.erase(candidate_list.begin()+c_idx);
    }

    return;
}

int main(int argc, char** argv)
{
    read_data(argc, argv, &n, &m, &p, &b, &t, &c, &cur_capacities);
    print_data(n, m, p, b, t, c);

    float alpha = strtof(argv[2], NULL);
    srand(time(NULL));

    std::vector<std::vector<int>> current_sol(m+1, std::vector<int>());
    int current_cost = 0;

    construction(current_sol, current_cost, alpha);
    print_solution(current_sol, current_cost);

    free(m, &b, &t, &c);

    return 0;
}