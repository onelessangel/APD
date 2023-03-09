#include "tema3.hpp"
 
int main (int argc, char** argv) {
    int total_ranks, worker_ranks, rank;
    int known_ranks = 1;
    int vector_len, error;
    int workload_per_worker, remaining_workload;
    int start = 0, start_pos = 0;
    int worker_idx, last_worker;
    int coord;
    vector<int> v, worker_v;
    vector<int> workers;
    topology_t topo;
    
    MPI_Init(&argc, &argv);
 
    MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    worker_ranks = total_ranks - 4;
    vector_len = atoi(argv[1]);
    error = atoi(argv[2]);

    switch (error)
    {
        case 0: case 1:
            if (proc_is_coord(rank)) {
                // compute topology
                workers = save_workers_topology(rank, known_ranks);
                add_to_topology(topo, rank, workers);
                send_coord_info(rank, workers);

                if (rank == 0) {
                    send_data(topo, 0, 3, 0);
                    recv_data(topo, 0, 3, 1);
                    recv_data(topo, 0, 3, 2);
                    recv_data(topo, 0, 3, 3);
                } else if (rank == 1) {
                    recv_data(topo, 1, 2, 0);
                    recv_data(topo, 1, 2, 3);
                    recv_data(topo, 1, 2, 2);
                    send_data(topo, 1, 2, 1);
                } else if (rank == 2) {
                    recv_data(topo, 2, 3, 0);
                    recv_data(topo, 2, 3, 3);
                    send_data(topo, 2, 1, 0);
                    send_data(topo, 2, 1, 3);
                    send_data(topo, 2, 1, 2);
                    recv_data(topo, 2, 1, 1);
                    send_data(topo, 2, 3, 1);
                    send_data(topo, 2, 3, 2);
                } else if (rank == 3) {
                    recv_data(topo, 3, 0, 0);
                    send_data(topo, 3, 2, 0);
                    send_data(topo, 3, 2, 3);
                    recv_data(topo, 3, 2, 1);
                    recv_data(topo, 3, 2, 2);
                    send_data(topo, 3, 0, 1);
                    send_data(topo, 3, 0, 2);
                    send_data(topo, 3, 0, 3);
                }

                // send topology to workers
                if (topo.cluster[0].size() + topo.cluster[1].size() + topo.cluster[2].size() + topo.cluster[3].size() == worker_ranks) {
                    print_topology(topo, rank);
                    for (auto worker : workers) {
                        send_data(topo, rank, worker, 0);
                        send_data(topo, rank, worker, 1);
                        send_data(topo, rank, worker, 2);
                        send_data(topo, rank, worker, 3);
                    }
                }

                // receive vector from neighbor coords (coord 0 computes the vector)
                if (rank == 0) {
                    // compute vector
                    for (int i = 0; i < vector_len; i++) {
                        v.emplace_back(vector_len - i - 1);
                    }
                } else if (rank == 1) {
                    recv_vector(v, 1, 2);
                } else if (rank == 2) {
                    recv_vector(v, 2, 3);
                } else if (rank == 3) {
                    recv_vector(v, 3, 0);
                }

                compute_vector_fragment(rank, workers, v, v.size(), worker_ranks, topo);

                // communicate the vector with the neighbor coords
                if (rank == 0) {
                    send_vector(v, 0, 3);
                    recv_vector(v, 0, 3);
                    print_vector(v);
                } else if (rank == 1) {
                    send_vector(v, 1, 2);
                } else if (rank == 2) {
                    send_vector(v, 2, 1);
                    recv_vector(v, 2, 1);
                    send_vector(v, 2, 3);
                } else if (rank == 3) {
                    send_vector(v, 3, 2);
                    recv_vector(v, 3, 2);
                    send_vector(v, 3, 0);
                }
            } else  {
                // receive coord name
                coord = recv_coord_info();

                // receive topology from coord
                recv_data(topo, rank, coord, 0);
                recv_data(topo, rank, coord, 1);
                recv_data(topo, rank, coord, 2);
                recv_data(topo, rank, coord, 3);

                print_topology(topo, rank);

                // receive work from coord and send back the computed data
                recv_work(v, rank, coord);
                compute_work(v);
                send_completed_work(v, rank, coord);
            }
            break;
        
        default:
            break;
    }

    MPI_Finalize();
}