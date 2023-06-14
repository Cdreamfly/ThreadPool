#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace cm {

    class Task {
    public:
        //自动逸任务类型，从Task继承，重写run方法，实现自定义任务处理
        virtual void run() = 0;
    };

    enum class PoolMode {
        MODE_FIXED,     //线程数量固定模式
        MODE_CACHED     //线程数量动态增长模式
    };

    class Thread {
    public:
    private:
    };

    class ThreadPool {
    public:
        ThreadPool() = default;

        virtual ~ThreadPool();

        void setMode(const PoolMode &mode);

        void start();

    private:
        PoolMode poolMode_{};             //当前线程池的工作模式
        std::vector<Thread *> thread_;  //线程列表
        std::size_t initThreadSize_{};    //初始线程数量
        std::queue<std::shared_ptr<Task>> taskQueue_;    //任务队列
        std::atomic_uint taskSize_{};     //任务数量
        std::size_t taskQueueMaxHold_{};  //任务队列数量阈值
        std::mutex taskQueueMtx_;       //保证任务队列线程安全
        std::condition_variable notEmpty_;  //任务队列不空
        std::condition_variable notFull_;   //任务队列不满
    };
}