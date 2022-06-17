#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "data.h"
#include "utils.h"
#include "random.hpp"

using Random = effolkronium::random_static;

#define M 99999

int n;
int m;
int p;
int* b;
int** t;
int** c;

int optimal;

void print_solution(std::vector<std::vector<int>>& sol, int& cost, std::vector<int>& cap)
{
    for(int i = 0; i < m+1; i++)
    {
        if(i == 0)
            printf("N.A: ");
        else
            printf("S%d: ", i);

        int jobs = sol[i].size();
        for(int j = 0; j < jobs; j++)
        {
            printf("%d ", sol[i][j]+1);
        }
        if(i != 0)
            printf("Cap: %d", cap[i-1]);
        puts("");
    }
    printf("Cost: %d\n\n", cost);
}

void construction(std::vector<std::vector<int>>& current_sol, int& current_cost, std::vector<int>& current_capacities, float& alpha)
{
    current_sol = std::vector<std::vector<int>>(m+1, std::vector<int>());
    current_capacities = std::vector<int>(m);
    current_cost = 0;
    for(int i = 0; i < m; i++)
    {
        current_capacities[i] = b[i];
    }

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
                if(current_capacities[i-1] > t[i-1][job])
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
            choosen = Random::get(0, range-1);
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
            current_capacities[server-1] -= t[server-1][job];
        }
        candidate_list.erase(candidate_list.begin()+c_idx);
    }

    return;
}

bool swap(std::vector<std::vector<int>>& current_sol, int& current_cost, std::vector<int>& current_capacities)
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
            int cap_1_aux = current_capacities[i_1] + t[i_1][job_1]; //job_1 leaving server_1

            for(int i_2 = i_1+1; i_2 < m; i_2++)
            {
                int server_2_size = current_sol[i_2+1].size();
                int delta_aux = c[i_2][job_1] - c[i_1][job_1]; //job_1 leaving server_1 and entering server_2
                int cap_2_aux = current_capacities[i_2] - t[i_2][job_1]; //job_1 entering server_2

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
        current_capacities[server_pos_1] = cap_1_left - t[server_pos_1][best_job_2];
        current_capacities[server_pos_2] = cap_2_left + t[server_pos_2][best_job_2];
    }
    
    return improved;
}

bool reinsertion_allocated(std::vector<std::vector<int>>& current_sol, int& current_cost, std::vector<int>& current_capacities)
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
                    if(current_capacities[i_2] > t[i_2][job])
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
        current_capacities[server_pos_1] += t[server_pos_1][best_job];
        current_capacities[server_pos_2] -= t[server_pos_2][best_job];
        current_sol[server_pos_2+1].push_back(best_job);
        current_sol[server_pos_1+1].erase(current_sol[server_pos_1+1].begin()+job_pos);
    }

    return improved;
}

bool reinsertion_not_allocated(std::vector<std::vector<int>>& current_sol, int& current_cost, std::vector<int>& current_capacities)
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
            if(current_capacities[i] > t[i][job])
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
        current_capacities[server_pos] -= best_delta;
        current_sol[server_pos+1].push_back(best_job);
        current_sol[0].erase(current_sol[0].begin()+job_pos);
    }

    return improved;
}

void rvnd(std::vector<std::vector<int>> &current_sol, int& current_cost, std::vector<int>& current_capacities)
{
    //id = 1 for swap, 2 for reinsertion-allocated and 3 for reinsertion-not-allocated
    std::vector<int> nbh_ids = {1, 2, 3};
    Random::shuffle(nbh_ids.begin(), nbh_ids.end()); //Randomizes neighbourhood list

    int idx = 0;
    while(idx < 3)
    {
        int nbh = nbh_ids[idx]; //Gets neighbourhood from shuffled list
        int improved = false;

        switch(nbh)
        {
            case 1:
                improved = swap(current_sol, current_cost, current_capacities);
                break;
            case 2:
                improved = reinsertion_allocated(current_sol, current_cost, current_capacities);
                break;
            case 3:
                improved = reinsertion_not_allocated(current_sol, current_cost, current_capacities);
                break;
        }

        if(improved)
        {
            idx = 0;
            Random::shuffle(nbh_ids.begin(), nbh_ids.end()); //Randomizes neighbourhood list
        }
        else
        {
            idx += 1;
        }
    }
}

void perturbation(std::vector<std::vector<int>>& current_sol, int& current_cost, std::vector<int>& current_capacities)
{
    int server_pos_1 = -1;
    int server_pos_2 = -1;
    int job_pos_1 = -1;
    int job_pos_2 = -1;
    int job_1 = -1;
    int job_2 = -1;
    int cap_left_1 = -1;
    int cap_left_2 = -1;

    //Two random swaps
    for(int i = 0; i < 2; i++)
    {
        //Searchs for two feasible random servers and jobs to swap
        while(1)
        {
            server_pos_1 = Random::get(0, m-1);
            server_pos_2 = Random::get(0, m-1);
            int size_1 = current_sol[server_pos_1+1].size();
            int size_2 = current_sol[server_pos_2+1].size();
            if((server_pos_1 == server_pos_2) || size_1 == 0 || size_2 == 0)
                continue;

            //Choosing random jobs from each server
            job_pos_1 = Random::get(0, size_1-1);
            job_pos_2 = Random::get(0, size_2-1);
            job_1 = current_sol[server_pos_1+1][job_pos_1];
            job_2 = current_sol[server_pos_2+1][job_pos_2];

            cap_left_1 = current_capacities[server_pos_1] + t[server_pos_1][job_1] - t[server_pos_1][job_2];
            cap_left_2 = current_capacities[server_pos_2] + t[server_pos_2][job_2] - t[server_pos_2][job_1];

            if(cap_left_1 >= 0 && cap_left_2 >= 0)
                break;
        }
        //-------------------------------------------------------

        //Doing the swap
        current_cost += c[server_pos_2][job_1] + c[server_pos_1][job_2] - c[server_pos_1][job_1] - c[server_pos_2][job_2];

        current_sol[server_pos_2+1][job_pos_2] = job_1;
        current_sol[server_pos_1+1][job_pos_1] = job_2;
        current_capacities[server_pos_1] = cap_left_1;
        current_capacities[server_pos_2] = cap_left_2;
        //--------------
    }
}

float gap(int heuristic_value, int optimal_value)
{
    return (((float)(heuristic_value - optimal_value) / (float)optimal_value)) * 100.0;
}

std::chrono::duration<double> grasp(std::vector<std::vector<int>>& best_sol, int& best_cost, std::vector<int>& best_capacities, float& alpha, int& IGrasp)
{
    int current_cost;
    int better_cost;
    std::vector<int> current_capacities;
    std::vector<int> better_capacities;
    std::vector<std::vector<int>> current_sol;
    std::vector<std::vector<int>> better_sol;

    printf("Running GRASP\n");
    auto timerStart = std::chrono::system_clock::now();
    for(int iterGrasp = 0; iterGrasp < IGrasp; iterGrasp++)
    {
        construction(current_sol, current_cost, current_capacities, alpha);
        rvnd(current_sol, current_cost, current_capacities);

        if(current_cost < best_cost)
        {
            best_sol = current_sol;
            best_cost = current_cost;
            best_capacities = current_capacities;
        }
    }
    auto timerEnd = std::chrono::system_clock::now();

    return timerEnd - timerStart;
}

std::chrono::duration<double> ils(std::vector<std::vector<int>>& best_sol, int& best_cost, std::vector<int>& best_capacities, float& alpha, int& IIls)
{
    int current_cost;
    int better_cost;
    std::vector<int> current_capacities;
    std::vector<int> better_capacities;
    std::vector<std::vector<int>> current_sol;
    std::vector<std::vector<int>> better_sol;

    printf("Running ILS\n");
    auto timerStart = std::chrono::system_clock::now();
    construction(current_sol, current_cost, current_capacities, alpha);
    rvnd(current_sol, current_cost, current_capacities);

    best_sol = current_sol;
    best_cost = current_cost;
    best_capacities = current_capacities;

    int iterIls = 0;
    while(iterIls < IIls)
    {
        perturbation(current_sol, current_cost, current_capacities);
        rvnd(current_sol, current_cost, current_capacities);

        if(current_cost < best_cost)
        {
            iterIls = 0;
            best_sol = current_sol;
            best_cost = current_cost;
            best_capacities = current_capacities;
        }
        else
        {
            iterIls++;
            current_sol = best_sol;
            current_cost = best_cost;
            current_capacities = best_capacities;
        }
    }
    auto timerEnd = std::chrono::system_clock::now();

    return timerEnd - timerStart;
}

std::chrono::duration<double> gils_rvnd(std::vector<std::vector<int>>& best_sol, int& best_cost, std::vector<int>& best_capacities, float& alpha, int& IGrasp, int& IIls)
{
    int current_cost;
    int better_cost;
    std::vector<int> current_capacities;
    std::vector<int> better_capacities;
    std::vector<std::vector<int>> current_sol;
    std::vector<std::vector<int>> better_sol;

    printf("Running GILS-RVND\n");
    auto timerStart = std::chrono::system_clock::now();
    for(int iterGrasp = 0; iterGrasp < IGrasp; iterGrasp++)
    {
        construction(current_sol, current_cost, current_capacities, alpha);

        better_sol = current_sol;
        better_cost = current_cost;
        better_capacities = current_capacities;

        int iterIls = 0;
        while(iterIls < IIls)
        {
            rvnd(current_sol, current_cost, current_capacities);

            if(current_cost < better_cost)
            {
                iterIls = 0;
                better_sol = current_sol;
                better_cost = current_cost;
                better_capacities = current_capacities;
            }
            else
            {
                iterIls++;
                current_sol = better_sol;
                current_cost = better_cost;
                current_capacities = better_capacities;
            }

            perturbation(current_sol, current_cost, current_capacities);
        }

        if(better_cost < best_cost)
        {
            best_sol = better_sol;
            best_cost = better_cost;
            best_capacities = better_capacities;
        }
    }
    auto timerEnd = std::chrono::system_clock::now();

    return timerEnd - timerStart;
}

int main(int argc, char** argv)
{
    read_data(argc, argv, &n, &m, &p, &b, &t, &c);
    print_data(n, m, p, b, t, c);

    int optimal = atoi(argv[2]);
    int metaheuristic = atoi(argv[3]);
    float alpha = 0.8;
    int IGrasp = 16;
    int IIls = 16;

    int custos_somados = 0;

    for(int i = 0; i < 10; i++)
    {
        int best_cost = M;
        std::vector<int> best_capacities;
        std::vector<std::vector<int>> best_sol;
        std::chrono::duration<double> time;

        switch(metaheuristic)
        {
            case 1:
                time = grasp(best_sol, best_cost, best_capacities, alpha, IGrasp);
                break;
            case 2:
                time = ils(best_sol, best_cost, best_capacities, alpha, IIls);
                break;
            case 3:
                time = gils_rvnd(best_sol, best_cost, best_capacities, alpha, IGrasp, IIls);
                break;
        }

        std::cout << "Time: " << time.count() << "s" << std::endl;
        custos_somados += best_cost;
    }

    float average_custo = ((float)custos_somados)/10.0;
    float average_gap = gap(average_custo, optimal);
    std::cout << "Average Cost: " << average_custo << std::endl;
    std::cout << "Average Gap: " << average_gap << std::endl;

    free(m, &b, &t, &c);

    return 0;
}