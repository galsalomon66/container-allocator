#include "rgw_stl_alloc.h"
#include <map>
#include <list>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



using namespace std;
using namespace RGW_allocator;
static int * rand_arr=0;

int my_rand()
{
	//purpose: saving bottle-necks
	static int idx = 0;
#define ARR_SZ 30000000

	if (!rand_arr) {
		rand_arr = (int*)malloc( sizeof(int) * ARR_SZ );

		for(int i=0;i<ARR_SZ;i++)rand_arr[ i ] = rand() % 123456789;
	}

	return rand_arr[ idx++ % ARR_SZ];
}

int test_map(int scale)
{

#ifdef MY_ALLOC
	typedef map<long long ,long long,less<long long>, ChunkAllocator< pair<long long ,long long >   > > my_map;
#else
	typedef map<long long ,long long,less<long long>   > my_map;
#endif

	my_map mm;

	for(int i=0;i<9000000;i++)
	{int a,b;
		a=my_rand();b=my_rand();

		if (mm.find(a) == mm.end())
		{
			mm[ a ] = b;
		}
	}

	return 0;
}



int test_list(int scale)
{
#ifdef MY_ALLOC
	typedef list<int,ChunkAllocator<int>  > my_list;
#else
	typedef list<int> my_list;
#endif

	my_list *ptr_my_list;

	for(int y=0;y<5;y++)
	{
		ptr_my_list = new my_list();

		for(int i=0;i<30000000;i++)
		{
			ptr_my_list->push_back( i );
		}

		delete ptr_my_list;
	}

	return 0;
}

void * start_test_list(void * arg)
{
	test_list(0);
	return 0;
}

void * start_test_map(void * arg)
{
	test_map(0);
	return 0;
}

typedef enum { List , Map } cont_tp_En_t;

void boot_thread(int num_of , cont_tp_En_t stl_type)
{

	pthread_t pt[64];

	void*(*f)(void*) = start_test_map;
	if(stl_type == List) f = start_test_list;

	for(int i=0;i<num_of;i++)
		pthread_create(&pt[i], NULL, f , NULL);

	for(int i=0;i<num_of;i++)
		pthread_join(pt[i] , NULL );
}

int main(int argc,char **argv)
{


	srand(time(0));
	my_rand();
	cont_tp_En_t cont_type=Map; 	

	if(argc<3) return -1;
	if(!strcasecmp(argv[1],"list")) {cont_type=List;}

	char*p;
	int num_of_threads=strtol(argv[2],&p,10);
	if(*p!=0) return -1;
	
	boot_thread( num_of_threads , cont_type);

	free( rand_arr );

	return 0;
}
