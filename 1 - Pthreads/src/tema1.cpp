#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <iomanip>
#include <math.h>
#include <unordered_map>
#include <pthread.h>
#include "File.hpp"
#include "Utils.hpp"

using namespace std;

bool check_nth_root(int m, int n) {
	int low = 1, high = sqrt(m) + 1;
	int middle, sol = -1;
	long x;
	
	while (low <= high) {
		middle = (low + high) / 2;
		x = pow(middle, n);

		if (x == m) {
			sol = middle;
			break;
		}

		if (x < m) {
			low = middle + 1;
		} else {
			high = middle - 1;
		}
	}

	return sol != -1;
}

void *reducer_func(void *arg)
{
	// get thread ID and utils from function argument
	pair<int, Utils *> thread_args = *(pair<int, Utils *>  *) arg;
	int thread_id = thread_args.first;
	Utils *utils = thread_args.second;

	// get the power the current thread is working with
	int thread_pow = thread_id + 2;
	set<int> perfect_power_numbers;

	pthread_barrier_wait(&utils->barrier);

	// add all values from the mappers' partial lists into the set
	// working with the current reducer's power
	for (auto power_list : utils->partial_lists_array) {
		if (power_list.first != thread_pow) {
			continue;
		}

		for (auto number : power_list.second) {
			perfect_power_numbers.insert(number);
		}
	}

	// write output into file
	ofstream file("out" + to_string(thread_pow) + ".txt");
	file << perfect_power_numbers.size();

	return NULL;
}

void *mapper_func(void *arg)
{
	// get thread ID and utils from function argument
	pair<int, Utils *> thread_args = *(pair<int, Utils *>  *) arg;
	int thread_id = thread_args.first;
	Utils *utils = thread_args.second;

	// partial lists for current thread
	vector<int> thread_partial_lists[utils->max_power + 1];
	int n, number;
	File *file;

	while(true) {
		// extract file and pop it from the input files vector
		pthread_mutex_lock(&utils->mutex);

		if (utils->in_files.size() != 0) {
			file = utils->in_files.back();
			utils->in_files.pop_back();
		} else {
			pthread_mutex_unlock(&utils->mutex);
			break;
		}

		pthread_mutex_unlock(&utils->mutex);

		// open current file
		ifstream input(file->file_path);
		if (!input.is_open()) {
			cout << "Error opening file " << file->file_path << "\n";
		}

		input >> n;

		// check each number and add it into the correct partial list
		for (int i = 0; i < n; i++) {
			input >> number;

			for (int e = 2; e <= utils->max_power; e++) {
				if (check_nth_root(number, e)) {
					thread_partial_lists[e].push_back(number);
				}
			}
		}

		input.close();
	}

	// add the thread partial lists to the main partial lists array
	pthread_mutex_lock(&utils->mutex);
	for (int e = 2; e <= utils->max_power; e++) {
		utils->partial_lists_array.emplace_back(e, thread_partial_lists[e]);
	}
	pthread_mutex_unlock(&utils->mutex);

	pthread_barrier_wait(&utils->barrier);	
	return NULL;
}

int main(int argc, char *argv[])
{
	Utils *utils = new Utils();
	int r, max_no_threads, no_files;
	string file_name;
	File *main_file;

	utils->no_mappers = atoi(argv[1]);
	utils->no_reducers = atoi(argv[2]);
	utils->max_power = utils->no_reducers + 1;

	max_no_threads = max(utils->no_reducers, utils->no_mappers);

	main_file = new File(argv[3]);

	// init mutex
	if (pthread_mutex_init(&utils->mutex, NULL) != 0) {
		cout << "Error to init mutex\n";
		exit(-1);
	}

	// init barrier
	if (pthread_barrier_init(&utils->barrier, NULL, utils->no_mappers + utils->no_reducers) != 0) {
		printf("Error barrier init\n");
		exit(-1);
	}

	// open main file
	ifstream input_mf(main_file->file_path);
	if (!input_mf.is_open()) {
		cout << "Error opening file " << main_file->file_path << "\n";
		exit(-1);
	}

	input_mf >> no_files;

	// add input files in vector and sort them by size
	for (int i = 0; i < no_files; i++) {
		input_mf >> file_name;
		utils->in_files.push_back(new File(file_name));
	}
	sort(utils->in_files.begin(), utils->in_files.end(), File::size_sort);

	pthread_t mappers[utils->no_mappers];
	pthread_t reducers[utils->no_reducers];
	pair<int, Utils *> mapper_args[utils->no_mappers];
	pair<int, Utils *> reducer_args[utils->no_reducers];

	// create mappers and reducers
	for (int i = 0; i < max_no_threads; i++) {
		if (i < utils->no_mappers) {
			mapper_args[i] = make_pair(i, utils);
			r = pthread_create(&mappers[i], NULL, mapper_func, &mapper_args[i]);

			if (r) {
				printf("Error creating mapper %d\n", i);
				exit(-1);
			}
		}

		if (i < utils->no_reducers) {
			reducer_args[i] = make_pair(i, utils);
			r = pthread_create(&reducers[i], NULL, reducer_func, &reducer_args[i]);

			if (r) {
				printf("Error creating reducer %d\n", i);
				exit(-1);
			}
		}
		
	}

	// join mappers and reducers
	for (int i = 0; i < max_no_threads; i++) {
		if (i < utils->no_mappers) {
			r = pthread_join(mappers[i], NULL);

			if (r) {
				printf("Error joining mapper %d\n", i);
				exit(-1);
			}
		}
		
		if (i < utils->no_reducers) {
			r = pthread_join(reducers[i], NULL);

			if (r) {
				printf("Error joining reducer %d\n", i);
				exit(-1);
			}
		}
	}

	pthread_mutex_destroy(&utils->mutex);
	pthread_barrier_destroy(&utils->barrier);
	input_mf.close();
	return 0;
}