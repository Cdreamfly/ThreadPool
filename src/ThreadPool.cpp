#include <functional>
#include "ThreadPool.h"

const std::size_t TASK_MAX_THREAD_HOLD = 1024;

cm::ThreadPool::ThreadPool() : poolMode_(cm::PoolMode::MODE_FIXED),
                               initThreadSize_(std::thread::hardware_concurrency()),
                               taskSize_(0),
                               taskQueueMaxHold_(TASK_MAX_THREAD_HOLD) {}

cm::ThreadPool::~ThreadPool() = default;

void cm::ThreadPool::setMode(const cm::PoolMode &mode) {
	poolMode_ = mode;
}

void cm::ThreadPool::setTaskQueueMaxHold(const std::size_t size) {
	this->taskQueueMaxHold_ = size;
}

void cm::ThreadPool::submitTask(const std::shared_ptr<Task> &) {

}

void cm::ThreadPool::start(const std::size_t size) {
	this->initThreadSize_ = size;
	for (int i = 0; i < initThreadSize_; ++i) {
		threads_.emplace_back(new Thread([this] { threadFunc(); }));
	}
	for (int i = 0; i < initThreadSize_; ++i) {
		threads_[i]->start();
	}
}

void cm::ThreadPool::threadFunc() {

}

void cm::Thread::start() {

}

cm::Thread::Thread(const cm::Thread::ThreadFunc &func) {
	this->func_ = func;
}

cm::Thread::~Thread() = default;
