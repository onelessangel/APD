#include "helper_func.hpp"

bool proc_is_coord(int rank)
{
    return rank == 0 || rank == 1 || rank == 2 || rank == 3;
}

vector<int> save_workers_topology(int rank, int &known_ranks)
{
    int no_workers, worker;
    vector<int> workers;
    string cluster_file = "cluster" + to_string(rank) + ".txt";
    ifstream input_file(cluster_file);

    input_file >> no_workers;
    known_ranks += no_workers;

    for (int i = 0; i < no_workers; i++) {
        input_file >> worker;
        workers.emplace_back(worker);
    }

    input_file.close();

    return workers;
}

void add_to_topology(topology_t &topology, int target, vector<int> target_workers)
{
    topology.cluster[target] = target_workers;
}

void log_message(int sender, int receiver)
{
    cout << "M(" << sender << "," << receiver << ")\n";
}

void send_coord_info(int rank, vector<int> workers)
{
    for (auto w : workers) {
        log_message(rank, w);
        MPI_Send(&rank, 1, MPI_INT, w, TAG_COORD, MPI_COMM_WORLD);
    }
}

int recv_coord_info()
{
    int coord;
    MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, TAG_COORD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return coord;
}

void send_data(topology_t topology, int sender, int receiver, int target)
{
    vector<int> workers = topology.cluster[target];
    int no_workers = workers.size();

    log_message(sender, receiver);
    MPI_Send(&no_workers, 1, MPI_INT, receiver, TAG_NO_WORKERS, MPI_COMM_WORLD);

    for (int i = 0; i < no_workers; i++) {
        log_message(sender, receiver);
        MPI_Send(&workers[i], 1, MPI_INT, receiver, TAG_WORKER, MPI_COMM_WORLD);
    }
}

void recv_data(topology_t &topology, int receiver, int sender, int target)
{
    int no_workers;
    MPI_Recv(&no_workers, 1, MPI_INT, sender, TAG_NO_WORKERS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int worker;
    vector<int> target_workers;
    for (int i = 0; i < no_workers; i++) {
        MPI_Recv(&worker, 1, MPI_INT, sender, TAG_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        target_workers.emplace_back(worker);
    }

    add_to_topology(topology, target, target_workers);
}

void print_topology(topology_t topology, int rank)
{
    string topology_string;

    for (int i = 0; i < NO_COORDS; i++) {
        topology_string.append(to_string(i) + ":");
        for (int j = 0; j < topology.cluster[i].size(); j++) {
            if (j > 0) {
                topology_string.append(",");
            }
            topology_string.append(to_string(topology.cluster[i][j]));
        }
        topology_string.append(" ");
    }
    cout << rank << " -> " << topology_string << "\n";
}

void send_work(vector<int> v, int sender, int receiver, int worker_idx, int start, int workload)
{
    log_message(sender, receiver);
    MPI_Send(&workload, 1, MPI_INT, receiver, TAG_WORKLOAD, MPI_COMM_WORLD);

    int end = start + worker_idx + workload;

    for (int i = start; i < end; i++) {
        log_message(sender, receiver);
        MPI_Send(&v[i], 1, MPI_INT, receiver, TAG_WORK, MPI_COMM_WORLD);
    }
}

void recv_work(vector<int> &v, int receiver, int sender)
{
    int workload;
    MPI_Recv(&workload, 1, MPI_INT, sender, TAG_WORKLOAD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int vector_elem;
    for (int i = 0; i < workload; i++) {
        MPI_Recv(&vector_elem, 1, MPI_INT, sender, TAG_WORK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v.emplace_back(vector_elem);
    }
}

void compute_work(vector<int> &v)
{
    for (int i = 0; i < v.size(); i++) {
        v[i] *= 5;
    }
}

void send_completed_work(vector<int> v, int sender, int receiver)
{
    for (int i = 0; i < v.size(); i++) {
        log_message(sender, receiver);
        MPI_Send(&v[i], 1, MPI_INT, receiver, TAG_WORK, MPI_COMM_WORLD);
    }
}

void recv_completed_work(vector<int> &v, int receiver, int sender, int worker_idx, int start, int workload)
{
    int vector_elem;
    int end = start + workload;

    for (int i = start; i < end; i++) {
        MPI_Recv(&vector_elem, 1, MPI_INT, sender, TAG_WORK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v[i] = vector_elem;
    }
}

void send_vector(vector<int> v, int sender, int receiver)
{
    int v_size = v.size();

    log_message(sender, receiver);
    MPI_Send(&v_size, 1, MPI_INT, receiver, TAG_VECTOR_SIZE, MPI_COMM_WORLD);

    for (int i = 0; i < v_size; i++) {
        log_message(sender, receiver);
        MPI_Send(&v[i], 1, MPI_INT, receiver, TAG_VECTOR_ELEM, MPI_COMM_WORLD);
    }
}

void recv_vector(vector<int> &v, int receiver, int sender)
{
    int v_size;
    MPI_Recv(&v_size, 1, MPI_INT, sender, TAG_VECTOR_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int vector_elem;
    v.clear();
    for (int i = 0; i < v_size; i++) {
        MPI_Recv(&vector_elem, 1, MPI_INT, sender, TAG_VECTOR_ELEM, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v.emplace_back(vector_elem);
    }
}

int compute_previous_workers(topology_t topology, int rank)
{
    int previous_clusters_workers;

    switch (rank)
    {
        case 0:
            previous_clusters_workers = 0;
            break;
    
        case 1:
            previous_clusters_workers = topology.cluster[0].size() + topology.cluster[2].size() + topology.cluster[3].size();
            break;

        case 2:
            previous_clusters_workers = topology.cluster[0].size() + topology.cluster[3].size();
            break;

        case 3:
            previous_clusters_workers = topology.cluster[0].size();
            break;

        default:
            previous_clusters_workers = 0;
            break;
    }

    return previous_clusters_workers;
}

int compute_start(topology_t topology, int rank, int workload)
{
    return workload * compute_previous_workers(topology, rank);
}

void send_remaining_work(vector<int> v, int sender, int receiver, int start, int workload)
{
    log_message(sender, receiver);
    MPI_Send(&workload, 1, MPI_INT, receiver, TAG_WORKLOAD, MPI_COMM_WORLD);

    for (int i = start; i < start + workload; i++) {
        log_message(sender, receiver);
        MPI_Send(&v[i], 1, MPI_INT, receiver, TAG_WORK, MPI_COMM_WORLD);
    }
}

void recv_remaining_work(vector<int> &v, int receiver, int sender)
{
    int workload;
    MPI_Recv(&workload, 1, MPI_INT, sender, TAG_WORKLOAD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int vector_elem;
    for (int i = 0; i < workload; i++) {
        MPI_Recv(&vector_elem, 1, MPI_INT, sender, TAG_WORK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v.emplace_back(vector_elem);
    }

    cout << "sunt aici\n\n\n";
}

void recv_completed_remaining_work(vector<int> &v, int receiver, int sender, int start, int workload)
{
    int vector_elem;
    for (int i = start; i < start + workload; i++) {
        MPI_Recv(&vector_elem, 1, MPI_INT, sender, TAG_WORK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v[i] = vector_elem;
    }
}

void print_vector(vector<int> v)
{
    string vector_string = "Rezultat: ";

    for (int i = 0; i < v.size(); i++) {
        vector_string.append(to_string(v[i]));
        if (i != v.size() - 1) {
            vector_string.append(" ");
        }
    }

    cout << vector_string << "\n";
}

void compute_vector_fragment(int rank, vector<int> workers, vector<int> &v,
                             int vector_len, int worker_ranks, topology_t topology)
{
    int start, start_pos, workload_per_worker, workers_count;
    int remaining_workload, worker_idx, last_worker;

    // compute workload for each worker
    workload_per_worker = vector_len / worker_ranks;

    // compute start position for current cluster
    start = compute_start(topology, rank, workload_per_worker);

    // communicate with workers
    workers_count = workers.size();

    if (rank == 1) {
        workers_count--;
    }

    // process common workers
    for (int i = 0; i < workers_count; i++) {
        start_pos = start + i * workload_per_worker;
        send_work(v, rank, workers[i], i, start_pos, workload_per_worker);
        recv_completed_work(v, rank, workers[i], i, start_pos, workload_per_worker);
    }

    // communicate remaining data with last worker in cluster 1
    if (rank == 1) {
        remaining_workload = vector_len % worker_ranks + workload_per_worker;
        start += (workers.size() - 1) * workload_per_worker;
        worker_idx = workers.size() - 1;
        last_worker = topology.cluster[rank][worker_idx];

        send_remaining_work(v, rank, last_worker, start, remaining_workload);
        recv_completed_remaining_work(v, rank, last_worker, start, remaining_workload);
    }
}