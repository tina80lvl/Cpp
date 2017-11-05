#include <iostream>
#include <thread>
#include <vector>
#include <stdatomic.h>
#include <queue>

class ThreadPool {
public:
    //создает заданное количество потоков, ожидающих поступления задач
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency()) : running_(true){ //вызывает конструктор, значениение по умолчанию есть
        for (auto i = 0u; i < num_threads; ++i) {//0u это беззнаковый ноль
            //дальше лямбда-функция, которая не берёт аргументов
            threads_.emplace_back([this]() -> void {//добавить поток (этот чувак собирает прям там), там ещё и лямбда
                while (running_.load()) {
                    std::unique_lock lock(mutex_);
                    if (tasks_.empty())
                        cv_.wait(lock);
                    if (tasks_.empty())
                        continue;
                    auto t = std::move(tasks_.front());
                    lock.unlock();
                    t();
                }
            });
        }
    }

    //ожидает завершения выполняющихся задач (при этом не начинает выполнения задач из очереди) и завершает все потоки
    void Run(std::function<void()> const & f) {
        std::unique_lock lock(mutex_);
        tasks_.push(f);
        cv_.notify_one();
    }

    //добавляет задачу в очередь выполнения, откуда ее забирает и один раз выполняет первый освободившийся поток
    ~ThreadPool() {
        running_.store(false);
        cv_.notify_all();
        for (auto & t : threads_) {
            t.join();
        }
    }
private:
    std::queue<std::function<void()>> tasks_;
    std::atomic_bool running_;//запись и чтение не используют кэш
    std::mutex mutex_;//
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}