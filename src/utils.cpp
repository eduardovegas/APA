#include "utils.h"

//Cantor pairing function
int pair(int x, int y)
{
    return ((x+y)*(x+y+1))/2 + y;
}

//Inverse Cantor pairing function
std::pair<int,int> unpair(int z)
{
    int w = floor((sqrt((8*z)+1)-1)/2);
    int t = ((w*w)+w)/2;
    int y = z-t;
    int x = w-y;
    return std::make_pair(x, y);
}

void zip(std::vector<int>& v1, std::vector<int>& v2, std::vector<std::pair<int,int>>& zipped)
{
    int size = v1.size();
    for(int i = 0; i < size; i++)
    {
        zipped.push_back(std::make_pair(v1[i], v2[i]));
    }
    return;
}

void unzip(std::vector<int>& v1, std::vector<int>& v2, std::vector<std::pair<int,int>>& zipped)
{
    int size = v1.size();
    for(int i = 0; i < size; i++)
    {
        v1[i] = zipped[i].first;
        v2[i] = zipped[i].second;
    }
    return;
}