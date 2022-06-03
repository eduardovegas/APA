#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <math.h>

int pair(int x, int y);
std::pair<int,int> unpair(int z);
void zip(std::vector<int>& v1, std::vector<int>& v2, std::vector<std::pair<int,int>>& zipped);
void unzip(std::vector<int>& v1, std::vector<int>& v2, std::vector<std::pair<int,int>>& zipped);

#endif