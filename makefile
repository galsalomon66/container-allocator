
test_rgw_stl_chunkalloc : test_rgw_alloc.cpp rgw_stl_alloc.h
	g++ -ggdb -O3  -lstdc++ -ldl -lpthread  test_rgw_alloc.cpp  -o test_rgw_stl_chunkalloc -DMY_ALLOC

test_rgw_stl_no_chunkalloc : test_rgw_alloc.cpp rgw_stl_alloc.h
	g++ -ggdb -O3  -lstdc++ -ldl -lpthread  test_rgw_alloc.cpp  -o test_rgw_stl_no_chunkalloc 

all: test_rgw_stl_chunkalloc test_rgw_stl_no_chunkalloc
