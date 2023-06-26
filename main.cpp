#include <iostream>
#include "ThreadPool.h"

int main() {
	std::cout << "Hello, World!" << std::endl;
	cm::ThreadPool pool;
	pool.start();
	std::cin.get();
    return 0;
}
