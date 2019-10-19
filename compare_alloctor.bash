#!/bin/bash

build_test_alloc()
{
	g++ -ggdb -O3  -lstdc++ -ldl -lpthread  test_rgw_alloc.cpp  -o test_rgw_alloc -DMY_ALLOC
}

watch_my_exe()
{
#sampling the process for resident-memory
rm /tmp/rss > /dev/null 2>&1 

while (( 1 == 1 ))
do	
	#pgrep $1 > /dev/null || break
	ps -a -o pid -o cmd | grep -v grep | grep $1 > /dev/null || break
	ps -p $(ps -a -o pid -o cmd | grep -v grep | grep $1 | awk '{print $1;}') -o rss >> /tmp/rss
	sleep 0.1
done


cat /tmp/rss | grep -v RSS | sort -n| tail -1 | awk '{print $1/(1000*1024)" Gb";}'
}

make -q all

read -ep "number of thread:" number_of_thread 
read -ep "type list or map:" stl_type 

e=test_rgw_stl_no_chunkalloc
echo running with no chunk-allocator
eval time  ./${e} ${stl_type} ${number_of_thread}  > /dev/null  & 
watch_my_exe ${e}
wait

e=test_rgw_stl_chunkalloc
echo running with chunk-allocator
eval time  ./${e} ${stl_type} ${number_of_thread}  > /dev/null  & 
watch_my_exe ${e}
wait


