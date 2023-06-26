#include <iostream>
#include "ThreadPool.h"

class MyTask : public cm::Task {
public:
	void run() override {
		std::cout<<"hello"<<std::endl;
		std::cout<<"id:"<<std::this_thread::get_id()<<std::endl;
	}
};

int main() {
	MyTask task;
	cm::ThreadPool pool;
	pool.submitTask(std::make_shared<MyTask>());
	pool.start();
	std::cin.get();
	return 0;
}
