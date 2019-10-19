# container-allocator
## Installation: make all

## Running : ./compare_alloctor.bash

an improved allocator for STL container's
this allocator declaration can improve insert-time and memory consumption greatlly.

main assumption is that cotainer (such as list/map/set) are mainly used in "append-mode", thus
it much more efficient to use chunk-allocation, i.e. ontainer is pre-allocating its own storage.

doing so(pre-allocation) , container is handling its own local-storage without contention, with reduced system-calls, 
and has the benefit of data-locality.

by running the /compare_alloctor.bash/ , entring number-of-threads and container type, its possible to observe the allocator effiency.

moving from std::map to std::map with-chunk-allocator , is trivial change since all container interfaces remain un-changed.

note: it's better to typedef container (i.e typedef std::map<int,int> my_map_t)
