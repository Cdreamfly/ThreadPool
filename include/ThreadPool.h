#pragma once

#include "Noncopyable.hpp"

#include <functional>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace cm {
	class Semaphore {
	public:
		Semaphore() : resLimit_(0) {}

		~Semaphore() = default;

		void wait() {
			std::unique_lock<std::mutex> lock{mtx_};
			cond_.wait(lock, [&] { return resLimit_ > 0; });
			--resLimit_;
		}

		void post() {
			std::unique_lock<std::mutex> lock{mtx_};
			++resLimit_;
			cond_.notify_all();
		}

	private:
		std::size_t resLimit_;
		std::mutex mtx_;
		std::condition_variable cond_;
	};

	class Any : private noncopyable {
	public:
		template<typename T>
		explicit Any(T t):base_(std::make_unique<Base>(t)) {}

		template<typename T>
		T cast_() {
			return dynamic_cast<Data <T> *>(base_.get())->data_;
		}

	private:
		class Base {
		public:
			virtual ~Base() = default;
		};

		template<typename T>
		class Data : public Base {
		public:
			explicit Data(T t) : data_(t) {}

		public:
			T data_;
		};

	private:
		std::unique_ptr<Base> base_;
	};

	class Task {
	public:
		//自动逸任务类型，从Task继承，重写run方法，实现自定义任务处理
		virtual void run() = 0;

		virtual ~Task() = default;
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

		bool submitTask(const std::shared_ptr<Task> &);

		//开启线程池
		void start(std::size_t size = std::thread::hardware_concurrency());

	private:
		void threadFunc();

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