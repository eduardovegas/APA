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
std::vector<int> servers_capacity;


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


    for(int i = 0; i < m; i++)
    {
        servers_capacity.push_back(b[i]);
    }

    while(!candidate_list.empty())
    {
        int c_list_size = candidate_list.size();

        //unique_edge_ids = cantor_pair(job,server)
        std::vector<int> allocation_costs;
        std::vector<int> unique_edge_ids;
        //----------------------------------------

        //all possible combination
        for(int k = 0; k < c_list_size; k++)
        {
            int candidate_idx = k;
            int job = candidate_list[candidate_idx];

            //job not alocated
            allocation_costs.push_back(p);
            unique_edge_ids.push_back(pair(candidate_idx, 0));

            //job alocated in each possible server
            for(int i = 1; i < m+1; i++)
            {
                if(servers_capacity[i-1] > t[i-1][job])
                {
                    allocation_costs.push_back(c[i-1][job]);
                    unique_edge_ids.push_back(pair(candidate_idx, i));
                }
            }
        }
        //------------------------

        std::vector<std::pair<int,int>> zipped;
        zip(allocation_costs, unique_edge_ids, zipped);

        //sorts in ascending order based on pair.first
        sort(zipped.begin(), zipped.end());

        unzip(allocation_costs, unique_edge_ids, zipped);

        int choosen = 0;
        int range = floor(alpha*allocation_costs.size());
        if(range != 0)
        {
            //choosen = rand([0,range))
            choosen = rand() % range;
        }

        int cost = allocation_costs[choosen];
        std::pair<int,int> inv_cantor = unpair(unique_edge_ids[choosen]);
        int candidate_idx = inv_cantor.first;
        int server = inv_cantor.second;
        int job = candidate_list[candidate_idx];

        current_sol[server].push_back(job);
        current_cost += cost;
        if(server != 0)
        {
            servers_capacity[server-1] -= t[server-1][job];
        }
        candidate_list.erase(candidate_list.begin()+candidate_idx);
    }

    return;
}


void swap(std::vector<std::vector<int>>& current_sol, int& current_cost){

    int smaller_cost = current_cost;
    int num_servers = m + 1;
    /*int job_pos1;
    int job_pos2;
    int j_1_swapped;
    int j_2_swapped;
    int i_1_swapped;
    int i_2_swapped;
    */
    for (int i_1 = 1; i_1 < num_servers; i_1++){
        
        int  c_server_1 = i_1 -1;
        int jobs_1 = current_sol[i_1].size();
        
        for(int j_1 = 0; j_1 < jobs_1; j_1++){
                
                int job_1 = current_sol[i_1][j_1];
           
            for(int i_2 = i_1 + 1; i_2 < num_servers; i_2++){
                
                int c_server_2 = i_2 -1;
                int jobs_2 = current_sol[i_2].size();
                
                for(int j_2 = 0; j_2 < jobs_2; j_2++){
                    
                    int job_2 = current_sol[i_2][job_2];
                          
                    //Condition to check if the cost found is smaller than the current cost
                    if(current_cost > (current_cost - c[c_server_1][job_1] - c[c_server_2][job_2] + c[c_server_1][job_2] + c[c_server_2][job_1])){
                        
                        
                        //Condition to check if the servers have enough capacity to make the swap
                        if (servers_capacity[c_server_1] >= (servers_capacity[c_server_1] - t[c_server_1][job_1] + t[c_server_1][job_2])
                            && servers_capacity[c_server_2] >= servers_capacity[c_server_2] - t[c_server_2][job_2] + t[c_server_2][job_1]){
                            
                            smaller_cost = current_cost - c[c_server_1][job_1] - c[c_server_2][job_2] + c[c_server_1][job_2] + c[c_server_2][job_1];
                            
                            /*
                            j_1_swapped = current_sol[i_1][j_1];
                            j_2_swapped = current_sol[i_2][j_2];
                            job_pos1 = j_1;
                            job_pos2 = j_2;

                            i_1_swapped = i_1;
                            i_2_swapped = i_2;*/    
                        }                  
                    }
                }
            }
        }
    }

    //Fazendo efetivamente o swap
    current_cost = smaller_cost;
    //current_sol[i_1_swapped][job_pos1] = j_2_swapped;
    //current_sol[i_2_swapped][job_pos2] = j_1_swapped;

}



//void rvnd(){



    
//}



int main(int argc, char** argv)
{
    read_data(argc, argv, &n, &m, &p, &b, &t, &c);
    print_data(n, m, p, b, t, c);

    float alpha = strtof(argv[2], NULL);
    srand(time(NULL));

    std::vector<std::vector<int>> current_sol(m+1, std::vector<int>());
    int current_cost = 0;

    construction(current_sol, current_cost, alpha);
    print_solution(current_sol, current_cost);

    swap(current_sol, current_cost);
    print_solution(current_sol, current_cost);

    free(m, &b, &t, &c);

    return 0;
}