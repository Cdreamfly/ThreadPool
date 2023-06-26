#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

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
		using ThreadFunc = std::function<void()>;

		explicit Thread(ThreadFunc);

		virtual ~Thread();

		void start();

	private:
		ThreadFunc func_;
	};

	class ThreadPool {
	public:
		ThreadPool();

		virtual ~ThreadPool();

		//设置线程池的工作模式
		void setMode(const PoolMode &mode);

		//设置任务队列数量阈值
		void setTaskQueueMaxHold(std::size_t);

		void submitTask(const std::shared_ptr<Task> &);

		//开启线程池
		void start(std::size_t size = std::thread::hardware_concurrency());

	private:
		static void threadFunc();

	private:
		PoolMode poolMode_{};             //当前线程池的工作模式
		std::vector<std::unique_ptr<Thread>> threads_;  //线程列表
		std::size_t initThreadSize_{};    //初始线程数量
		std::queue<std::shared_ptr<Task>> taskQueue_;    //任务队列
		std::atomic_uint taskSize_{};     //任务数量
		std::size_t taskQueueMaxHold_{};  //任务队列数量阈值
		std::mutex taskQueueMtx_;       //保证任务队列线程安全
		std::condition_variable notEmpty_;  //任务队列不空
		std::condition_variable notFull_;   //任务队列不满
	};
}