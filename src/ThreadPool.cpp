
#include <utility>
#include <iostream>
#include "ThreadPool.h"

const std::size_t TASK_MAX_THREAD_HOLD = 1024;

cm::ThreadPool::ThreadPool() : poolMode_(cm::PoolMode::MODE_FIXED),
                               initThreadSize_(std::thread::hardware_concurrency()),
                               taskSize_(0),
                               taskQueueMaxHold_(TASK_MAX_THREAD_HOLD) {}

cm::ThreadPool::~ThreadPool() = default;

void cm::ThreadPool::setMode(const cm::PoolMode &mode) {
	this->poolMode_ = mode;
}

void cm::ThreadPool::setTaskQueueMaxHold(const std::size_t size) {
	this->taskQueueMaxHold_ = size;
}

bool cm::ThreadPool::submitTask(const std::shared_ptr<Task> &ptr) {
	std::unique_lock<std::mutex> lock{taskQueueMtx_};
	if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&] { return taskQueue_.size() < taskQueueMaxHold_; })) {
		std::cerr << "task queue is full, submit task fail." << std::endl;
		return false;
	}
	taskQueue_.emplace(ptr);
	++taskSize_;
	notEmpty_.notify_all();
	return true;
}

void cm::ThreadPool::start(const std::size_t size) {
	this->initThreadSize_ = size;
	for (int i = 0; i < initThreadSize_; ++i) {
		threads_.emplace_back(std::make_unique<Thread>([this] { threadFunc(); }));
	}
	for (int i = 0; i < initThreadSize_; ++i) {
		threads_[i]->start();
	}
}

void cm::ThreadPool::threadFunc() {
	std::cout << "begin thread func" << std::endl;
	std::cout << "id:" << std::this_thread::get_id() << std::endl;
	std::cout << "end thread func" << std::endl;
}

void cm::Thread::start() {
	std::thread([&] { func_(); }).detach();
}

cm::Thread::Thread(cm::Thread::ThreadFunc func) : func_(std::move(func)) {}

cm::Thread::~Thread() = default;
