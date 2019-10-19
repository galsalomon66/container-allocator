
#ifndef RGW_CHUNK_ALLOC
#define RGW_CHUNK_ALLOC

#include <list>
#include <cstdlib>

class alloc_fix_size {

#define DBG( ... ) 

	unsigned int m_fix_size_alloc;//size of single item
	void * m_block;
	unsigned int m_block_ptr; //pointer to location within block
	unsigned long long m_memory_in_use;//count how much memory is actually in-use.
	unsigned long long m_block_size;//allocation size per block 
	std::list<void*> m_pool_of_blocks; //container for all blocks
	unsigned long long m_available_mem;//avaialble memory on last block
		
	public:

	/******************************/
	alloc_fix_size(unsigned int xi_size):m_fix_size_alloc(xi_size),m_block(0),m_block_ptr(0),m_memory_in_use(0),m_block_size(10*1024*1024),m_available_mem(0)
	{
	}

	~alloc_fix_size()
	{
		std::list<void*>::iterator it;
		for(it = m_pool_of_blocks.begin();it != m_pool_of_blocks.end();it++)
		{
			free( *it );
		}
	}

	/******************************/
	void set_block_size(unsigned int xi_Count)
	{
		m_block_size = xi_Count * m_fix_size_alloc;
	}

	/******************************/
	void* alloc(size_t xi_count)
	{
	void* res =0;

		//TODO computation could optimize (pre compute)
		unsigned int alloc_sz = (m_fix_size_alloc*xi_count) + (  (m_fix_size_alloc*xi_count) % sizeof(void*) == 0 ? 0 : (sizeof(void*) - (m_fix_size_alloc % sizeof(void*))) );//adding allignment

		if(alloc_sz > m_available_mem)
		{

			if(alloc_sz > m_block_size )//may allocate big buffers
				m_block_size = alloc_sz;
	
			m_block = malloc( m_block_size );
			m_block_ptr = 0;
			m_available_mem = m_block_size;
			m_pool_of_blocks.push_back(m_block);
		}
			

		m_available_mem -= alloc_sz;

		res = (char*)m_block + m_block_ptr;

		m_block_ptr += alloc_sz;

		m_memory_in_use += m_fix_size_alloc;//allocation done by caller

		return res;		
	}

	/******************************/
	void dealloc(void * ptr)
	{//not freeing memeory. main assumption  , append only data-structure(and short live) ,  delete is done upon object destruction
		m_memory_in_use -= m_fix_size_alloc;

	}

};

#include <stdio.h>
namespace RGW_allocator
{
	template <typename T, unsigned int pool_size = 10000 >
	class ChunkAllocator: public std::allocator<T>
	{
	public:
		typedef size_t size_type;
		typedef T* pointer;
		typedef const T* const_pointer;

		alloc_fix_size* m_pool_allocator;

		template<typename _Tp1>
		struct rebind
		{
			typedef ChunkAllocator<_Tp1> other;
		};
		//==================================
		void* fix_size_alloc(size_t xi_count)
		{
			return m_pool_allocator->alloc( xi_count );
		}

		//==================================
		inline T *_Allocate(size_t _Count, T *)
		{	// allocate storage for _Count elements of type T

			void *_Ptr = 0;

			if (_Count == 0)
				;
			else if (((size_t)(-1) / sizeof (T) < _Count)
				|| (_Ptr = fix_size_alloc(_Count)) == 0)
				return (T*)_Ptr;

			return ((T *)_Ptr);
		}

		//==================================
		inline pointer allocate(size_type n, const void *hint=0)
 		{
			return (_Allocate(n, (pointer)0));
 		}

		//==================================
		inline void deallocate(pointer p, size_type n)
		{
			m_pool_allocator->dealloc(0);
		}

		//==================================
		ChunkAllocator() throw(): std::allocator<T>()
		{
			m_pool_allocator = new alloc_fix_size( sizeof(T));
			m_pool_allocator->set_block_size(pool_size * sizeof(T));
		}

		//==================================
		ChunkAllocator(const ChunkAllocator &other) throw(): std::allocator<T>(other)
		{
			m_pool_allocator = new alloc_fix_size( sizeof(T));
			m_pool_allocator->set_block_size(pool_size * sizeof(T));
		}

		//==================================
		~ChunkAllocator() throw()
		{
			if(m_pool_allocator)
				{delete m_pool_allocator;m_pool_allocator=0;}
		}
	};
}

#endif

