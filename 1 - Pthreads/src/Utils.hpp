#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <pthread.h>
#include "File.hpp"

using namespace std;

class Utils
{
  public:
    pthread_mutex_t mutex;
    pthread_barrier_t barrier;
    int no_mappers;
    int no_reducers;
    int max_power;
    vector<pair<int, vector<int>>> partial_lists_array;
    vector<File *> in_files;
};

#endif