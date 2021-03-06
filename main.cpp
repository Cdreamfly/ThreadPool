#include <deque>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <functional>
#include <condition_variable>

#include <iostream>

class ThreadPool {
public:
    explicit ThreadPool(size_t taskSize, size_t threadSize = std::thread::hardware_concurrency()) :
            _tasks(0),
            _maxTaskSize(taskSize),
            _running(false) {

        if (!_threads.empty())return;
        _running = true;
        _threads.reserve(threadSize);
        for (size_t i = 0; i < threadSize; ++i) {
            _threads.emplace_back([this]() -> void {
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lk(_mtx);
                        _not_empty.wait(lk, [this] {
                            return !_running.load() || !_tasks.empty();
                        });
                        if (!_running.load() && _tasks.empty())return;
                        task = std::move(_tasks.front());
                        _tasks.pop_front();
                    }
                    _not_full.notify_one();
                    task();
                }
            });
        }
    }

    ~ThreadPool() noexcept {
        _running = false;
        _not_empty.notify_all();
        _not_full.notify_all();
        for (auto &thr: _threads) {
            if (thr.joinable()) {
                thr.join();
            }
        }
    }

    bool Full() const {
        return _maxTaskSize > 0 && _tasks.size() >= _maxTaskSize;
    }

    template<typename F, typename... Args>
    auto Commit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
        if (!_running.load()) {
            throw std::runtime_error("Commit on ThreadPool is Stopped.");
        }
        using RetType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<RetType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::unique_lock<std::mutex> lk(_mtx);
            _not_full.wait(lk, [this] {
                return !_running.load() || !Full();
            });
            _tasks.emplace_back([task] {
                (*task)();
            });
        }
        _not_empty.notify_one();
        return future;
    }

private:
    using Task = std::function<void()>;

private:
    std::atomic<size_t> _maxTaskSize;
    std::deque<Task> _tasks;
    std::vector<std::thread> _threads;
    std::condition_variable _not_empty;
    std::condition_variable _not_full;
    std::mutex _mtx;
    std::atomic_bool _running;
};


int main() {

    ThreadPool *pool = new ThreadPool(4);
    std::vector<std::future<int>> rets;
    for (int i = 0; i < 100; i++) {
        rets.emplace_back(std::move(pool->Commit([i]() -> int {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "id:" << std::this_thread::get_id() << std::endl;
            return i;
        })));
    }
    for (auto &ret: rets) {
        std::cout << "ret:" << ret.get() << std::endl;
    }
    delete pool;
    return 0;
}
