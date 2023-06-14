#include "ThreadPool.h"


void cm::ThreadPool::setMode(const cm::PoolMode &mode) {
    poolMode_ = mode;
}

void cm::ThreadPool::start() {

}

cm::ThreadPool::~ThreadPool() = default;
