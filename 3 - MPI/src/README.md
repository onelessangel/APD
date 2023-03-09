Copyright Teodora Stroe 331CA 2023

# Collaborative calculations in distributed systems

The path between coord 0 and coord 1 is ignored from the start, to fit the
topologies for tasks 1, 2 and 3.

## Task 1 - Establishing the topology

Each coordiator:
- saves worker topology from cluster file;
- sends message to workers, communicating they are the cluster coordinator;
- sends data about known cluster topologies to right neighbor coord until
  it reaches coord 1;
- receives known cluster topologies from right neighbor cluster and communicates
  it to left neighbor coord;
- when it knows the entire topology, prints it and sends it to its workers.

Each worker:
- receives the cluster coordinator name;
- receives topology from coordinator.

## Task 2 - Computing the vector

Each coordinator:
- receives the modified vector from left neighbor coordinator 
  (coord 0 creates the vector);
- computes allocated vector fragment by computing the workload for each
  worker, sending each worker the assigned vector slice, and combining
  all computed slices from its workers in a global cluster vector;
- sends the computed vector to next right neghbor coordinator;
- if coord 1 has N workers, it sends the usual workload of data to first N - 1
  workers and sends to the Nth worker the remaining of the data;
- sends back computed vector, until it reaches coord 0;
- coord 0 prints the vector.

Each worker:
- receives the data, computes it and sends it back to the cluster coord.

## Task 3 - Communication channel errors

The approach used from the beginning makes the solution viable in case the
link between coord 0 and coord 1 is lost.