#include <numeric>
#include <thread>
#include <vector>
#include <algorithm>
#include <future>

template<typename Iterator,typename T>
struct accumulate_block
{
	T operator()(Iterator first, Iterator last)//1
	{
		return std::accumulate(first, last, T());//2
	}
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);//1

	//if (!length)
	//	return init;
	//unsigned long const min_per_thread = 25;
	unsigned long const max_chunk_size = 25;
	if (length <= max_chunk_size)
		return std::accumulate(first, last, init);//2
	else
	{
		Iterator mid_point = first;
		std::advance(mid_point, length / 2);//3
		std::future<T> first_half_result =
			std::async(parallel_accumulate<Iterator, T>,
				first, mid_point, init);//4
		T second_half_result = parallel_accumulate(mid_point, last, T());//5
		return first_half_result.get() + second_half_result;
	}
	/*
	unsigned long const max_threads =
		(length + min_per_thread - 1) / min_per_thread;

	unsigned long const hardware_threads =
		std::thread::hardware_concurrency();

	unsigned long const num_threads =
		std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	unsigned long const block_size = length / num_threads;

	std::vector<std::future<T>> futures(num_threads-1);//3
	std::vector<std::thread> threads(num_threads - 1);
	join_threads joiner(threads);//1

	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);
		std::packaged_task<T(Iterator, Iterator)> task(//4
			accumulate_block<Iterator, T>());
		futures[i] = task.get_future();//5
		threads[i] = std::thread(std::move(task), block_start, block_end);//6
		block_start = block_end;
	}
	//accumulate_block()(block_start, last, results[num_threads - 1]);
	T last_result = accumulate_block()(block_start, last);//7

	//std::for_each(threads.begin(), threads.end(),
	//	std::mem_fn(&std::thread::join));

	T result = init;//8
	for (unsigend long i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();//9
	}
	result += last_result;//10

	return result;
	*/
};

class join_threads
{
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_)
		:threads(threads)
	{}
	~join_threads()
	{
		for (unsigned long i = 0; i < threads.size(); ++i)
			if (threads[i].joinable())
				threads[i].join();
	}
};
