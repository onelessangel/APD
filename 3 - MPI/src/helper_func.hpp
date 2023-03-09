#ifndef _HELPER_FUNC_HPP_
#define _HELPER_FUNC_HPP_

#include "mpi.h"
#include <iostream>
#include <fstream>
#include <vector>
 
using namespace std;

typedef struct {
    vector<int> cluster[4];
} topology_t;


#define TAG_COORD           0
#define TAG_NO_WORKERS      1
#define TAG_WORKER          2

#define NO_COORDS           4

#define TAG_WORKLOAD        5
#define TAG_WORK            6
#define TAG_VECTOR_SIZE     7
#define TAG_VECTOR_ELEM     8

bool proc_is_coord(int rank);
vector<int> save_workers_topology(int rank, int &known_ranks);
void add_to_topology(topology_t &topology, int target, vector<int> target_workers);
void log_message(int sender, int recv);
void send_coord_info(int rank, vector<int> workers);
int recv_coord_info();
void send_data(topology_t topology, int sender, int receiver, int target);
void recv_data(topology_t &topology, int receiver, int sender, int target);
void print_topology(topology_t topology, int rank);

void send_work(vector<int> v, int sender, int receiver, int worker_idx, int start, int workload);
void recv_work(vector<int> &v, int receiver, int sender);
void compute_work(vector<int> &v);
void send_completed_work(vector<int> v, int sender, int receiver);
void recv_completed_work(vector<int> &v, int receiver, int sender, int worker_idx, int start, int workload);
void send_vector(vector<int> v, int sender, int receiver);
void recv_vector(vector<int> &v, int receiver, int sender);
int compute_previous_workers(topology_t topoology, int rank);
int compute_start(topology_t topology, int rank, int workload);
void send_remaining_work(vector<int> v, int sender, int receiver, int start, int workload);
void recv_remaining_work(vector<int> &v, int receiver, int sender);
void recv_completed_remaining_work(vector<int> &v, int receiver, int sender, int start, int workload);
void print_vector(vector<int> v);

void compute_vector_fragment(int rank, vector<int> workers, vector<int> &v,
                             int vector_len, int worker_ranks, topology_t topology);

#endif  // _HELPER_FUNC_HPP_