Copyright Teodora Stroe 331CA 2022

# Parallel processing using the Map-Reduce paradigm

## Entities:
- File class -- file manipulation
    - **file_path** and **size**
	- methods for **compute_file_size** [1] and **size_sort**
- Utils class -- globally used variables
	- mutex and barrier
	- number or mappers, reducers and the maximum power
	- a vector containing all the partial lists, as pairs: powerX - mapperY_partial_list_powerX
	- a vector of input files
- tema1.cpp
	- **main** function
	- thread functions: **mapper_func**, **reducer_func**
	- perfect power checking function: **check_nth_root**

## Program workflow:

- the mutex and the barrier are initialized;
- each input file is added into a globally **in_files** vector, sorted ascending by file size;
- in two consecutive loops the mappers and reducers are created, and then joined;
- the mutex and the barrier are destroyed.

## Mapper workflow:

Each mapper has an array containing partial lists for each power.

Each mapper works in a loop, while there are still input files to be processed:
- selects the file with the biggest size and pops it from the **in_files** vector (**mutex locked operation**);
- for each number in the input file:
	- checks if it's a perfect power for each exponent 2 to MAX_POWER;
	- adds it in the partial lists array for the correct exponents.

After the mapper has finished processing files, it adds its partial lists to the main partial lists array (**mutex locked operation**).

### Perfect power check:

A binary search [2] from 1 to sqrt(m) is performed to check if m is a perfect power of n.

## Reducer workflow:

A barrier of size *no_mappers + no_reducers* has been enforced at the end of the **mapper_func** and at the beginning of the **reducer_func** to force the reducer threads to process the reduction step after all the mappers have finished working.

Each **reducer_X** has a set containing the numbers that are perfect powers for **X + 2**.

Each **reducer_X**:
- selects the partial lists for the power **X + 2**;
- inserts the values from the partial lists into the set;
- writes into the output file how many entries there are in the set.

## Sources:

[1] https://www.tutorialspoint.com/how-can-i-get-a-file-s-size-in-cplusplus

[2] https://tutoriale-pe.net/algoritm-pentru-cautarea-binara-in-c/
