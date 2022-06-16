#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "data.h"
#include "utils.h"

#define M 99999

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
            printf("N.A: ");
        else
            printf("S%d: ", i);

        int jobs = current_sol[i].size();
        for(int j = 0; j < jobs; j++)
        {
            printf("%d ", current_sol[i][j]+1);
        }
        if(i != 0)
            printf("Cap: %d", cur_capacities[i-1]);
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

bool swap(std::vector<std::vector<int>>& current_sol, int& current_cost)
{
    int server_pos_1 = -1;
    int server_pos_2 = -1;
    int best_job_1 = -1;
    int best_job_2 = -1;
    int job1_pos = -1;
    int job2_pos = -1;
    int best_delta = 0;
    int cap_1_left = 0;
    int cap_2_left = 0;
    bool improved = false;

    for (int i_1 = 0; i_1 < m; i_1++)
    {
        int server_1_size = current_sol[i_1+1].size();

        for(int j_1 = 0; j_1 < server_1_size; j_1++)
        {
            int job_1 = current_sol[i_1+1][j_1];
            int cap_1_aux = cur_capacities[i_1] + t[i_1][job_1]; //job_1 leaving server_1

            for(int i_2 = i_1+1; i_2 < m; i_2++)
            {
                int server_2_size = current_sol[i_2+1].size();
                int delta_aux = c[i_2][job_1] - c[i_1][job_1]; //job_1 leaving server_1 and entering server_2
                int cap_2_aux = cur_capacities[i_2] - t[i_2][job_1]; //job_1 entering server_2

                for(int j_2 = 0; j_2 < server_2_size; j_2++)
                {
                    int job_2 = current_sol[i_2+1][j_2];

                    //checks if servers have enough capacity for the movement
                    if(cap_1_aux > t[i_1][job_2] && cap_2_aux + t[i_2][job_2] > 0)
                    {
                        //checks if the movement cost is better than best delta
                        int delta = c[i_1][job_2] + delta_aux - c[i_2][job_2];
                        if(delta < best_delta)
                        {
                            best_delta = delta;
                            server_pos_1 = i_1;
                            server_pos_2 = i_2;
                            job1_pos = j_1;
                            job2_pos = j_2;
                            best_job_1 = job_1;
                            best_job_2 = job_2;
                            cap_1_left = cap_1_aux;
                            cap_2_left = cap_2_aux;
                            improved = true;
                        }
                    }
                }
            }
        }
    }

    if(improved)
    {
        current_cost += best_delta; //Updating current cost

        //Swap
        current_sol[server_pos_2+1][job2_pos] = best_job_1;
        current_sol[server_pos_1+1][job1_pos] = best_job_2;
        cur_capacities[server_pos_1] = cap_1_left - t[server_pos_1][best_job_2];
        cur_capacities[server_pos_2] = cap_2_left + t[server_pos_2][best_job_2];
    }
    
    return improved;
}

bool disturbance(std::vector<std::vector<int>>& current_sol, int& current_cost)
{
    int server_pos_1 = -1;
    int server_pos_2 = -1;
    int job1_pos = -1;
    int job2_pos = -1;
    int cap_1_left = 0;
    int cap_2_left = 0;

    for (int i_1 = 0; i_1 < m; i_1++)
    {
        int server_1_size = current_sol[i_1+1].size();

        for(int j_1 = 0; j_1 < server_1_size; j_1++)
        {
            int job_1 = current_sol[i_1+1][j_1];
            int cap_1_aux = cur_capacities[i_1] + t[i_1][job_1]; //job_1 leaving server_1

            for(int i_2 = i_1+1; i_2 < m; i_2++)
            {
                int server_2_size = current_sol[i_2+1].size();
                int delta_aux = c[i_2][job_1] - c[i_1][job_1]; //job_1 leaving server_1 and entering server_2
                int cap_2_aux = cur_capacities[i_2] - t[i_2][job_1]; //job_1 entering server_2

                for(int j_2 = 0; j_2 < server_2_size; j_2++)
                {
                    int job_2 = current_sol[i_2+1][j_2];

                    //checks if servers have enough capacity for the movement
                    if(cap_1_aux > t[i_1][job_2] && cap_2_aux + t[i_2][job_2] > 0)
                    {
                        //checks if the movement cost is better than best delta
                        int delta = c[i_1][job_2] + delta_aux - c[i_2][job_2];
                        if(delta < best_delta)
                        {
                            best_delta = delta;
                            server_pos_1 = i_1;
                            server_pos_2 = i_2;
                            job1_pos = j_1;
                            job2_pos = j_2;
                            best_job_1 = job_1;
                            best_job_2 = job_2;
                            cap_1_left = cap_1_aux;
                            cap_2_left = cap_2_aux;
                            improved = true;
                        }
                    }
                }
            }
        }
    }

    if(improved)
    {
        current_cost += best_delta; //Updating current cost

        //Swap
        current_sol[server_pos_2+1][job2_pos] = best_job_1;
        current_sol[server_pos_1+1][job1_pos] = best_job_2;
        cur_capacities[server_pos_1] = cap_1_left - t[server_pos_1][best_job_2];
        cur_capacities[server_pos_2] = cap_2_left + t[server_pos_2][best_job_2];
    }
    
    return improved;
}

bool reinsertion_allocated(std::vector<std::vector<int>>& current_sol, int& current_cost)
{
    int server_pos_1 = -1;
    int server_pos_2 = -1;
    int best_job = -1;
    int job_pos = -1;
    int best_delta = 0;
    bool improved = false;

    for(int i_1 = 0; i_1 < m; i_1++)
    {
        int server_1_size = current_sol[i_1+1].size();

        for(int j = 0; j < server_1_size; j++)
        {
            int job = current_sol[i_1+1][j];

            for(int i_2 = 0; i_2 < m; i_2++)
            {
                if(i_2 != i_1)
                {
                    //checks if server_2 has enough capacity for the insertion
                    if(cur_capacities[i_2] > t[i_2][job])
                    {
                        int delta = c[i_2][job] - c[i_1][job]; //job leaving server_1 and entering server_2

                        //checks if the movement cost is better than best delta
                        if(delta < best_delta)
                        {
                            best_delta = delta;
                            server_pos_1 = i_1;
                            server_pos_2 = i_2;
                            best_job = job;
                            job_pos = j;
                            improved = true;
                        }
                    }
                }
            }
        }
    }

    if(improved)
    {
        current_cost += best_delta; //Updating current cost

        //Reinsertion
        cur_capacities[server_pos_1] += t[server_pos_1][best_job];
        cur_capacities[server_pos_2] -= t[server_pos_2][best_job];
        current_sol[server_pos_2+1].push_back(best_job);
        current_sol[server_pos_1+1].erase(current_sol[server_pos_1+1].begin()+job_pos);
    }

    return improved;
}

bool reinsertion_not_allocated(std::vector<std::vector<int>>& current_sol, int& current_cost)
{
    int server_pos = -1;
    int best_job = -1;
    int job_pos = -1;
    int best_delta = M;
    bool improved = false;

    int not_allocated_size = current_sol[0].size();
    for(int j = 0; j < not_allocated_size; j++)
    {
        int job = current_sol[0][j];

        for(int i = 0; i < m; i++)
        {
            //checks if server_i has enough capacity for the insertion
            if(cur_capacities[i] > t[i][job])
            {
                int delta = t[i][job];

                //checks if the movement processing time is better than best delta
                if(delta < best_delta)
                {
                    best_delta = delta;
                    server_pos = i;
                    best_job = job;
                    job_pos = j;
                    improved = true;
                }
            }
        }
    }

    if(improved)
    {
        current_cost += c[server_pos][best_job] - p; //Updating current cost

        //Reinsertion
        cur_capacities[server_pos] -= best_delta;
        current_sol[server_pos+1].push_back(best_job);
        current_sol[0].erase(current_sol[0].begin()+job_pos);
    }

    return improved;
}

void rvnd(std::vector<std::vector<int>> &current_sol, int& current_cost){
    std::vector<int> neighbourhood = {1, 2, 3}; //1 is for swap, 2 for reinsertion allocated and 3 for reinsertion not allocated
    bool empty_list = false;
    int movement;
    bool improved;
    srand(time(0));

    while(neighbourhood.empty() == false){
        improved = true;
        movement = rand()%neighbourhood.size(); //Choses neighbourhood at random
        
        if(neighbourhood[movement] == 1){

            while(improved == true){
        
                improved = swap(current_sol, current_cost);
                
            }
            //Once solution is no longer improving (best neighbourhood found), value  is erased from vector
            neighbourhood.erase(neighbourhood.begin() + movement);

        }else if(neighbourhood[movement] == 2){
                
            while (improved == true)
            {
                improved = reinsertion_allocated(current_sol, current_cost);
                
            }

            neighbourhood.erase(neighbourhood.begin() + movement);
    
        }else if(neighbourhood[movement] == 3){

            while (improved == true)
            {
                improved = reinsertion_not_allocated(current_sol, current_cost);
                
            }

            neighbourhood.erase(neighbourhood.begin() + movement);

        }

    }

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

    swap(current_sol, current_cost);
    print_solution(current_sol, current_cost);

    reinsertion_allocated(current_sol, current_cost);
    print_solution(current_sol, current_cost);

    reinsertion_not_allocated(current_sol, current_cost);
    print_solution(current_sol, current_cost);

    rvnd(current_sol, current_cost);
    print_solution(current_sol, current_cost);

    free(m, &b, &t, &c);

    return 0;
}