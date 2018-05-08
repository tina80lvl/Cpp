#include <iostream>
#include <thread>
#include <vector>
#include <stdatomic.h>
#include <queue>
#include <sstream>

int it = 0;

// RAII

class ThreadPool {
public:
    //создает заданное количество потоков, ожидающих поступления задач
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency())
        : running_(true)
    {
//        std::cout << "Creating" << std::endl;
        for (auto i = 0u; i < num_threads; ++i) {
            threads_.emplace_back([this] {
                // perepisat' s RAII
                while (running_) {
                    std::unique_lock<std::mutex> lock(mutex_);

                    // научиться пользоваться wait, почитать в интернете, переписать на conditional wait или while
                    if (tasks_.empty())
                        cv_.wait(lock);//если задачи кончились, блочит все потоки


                    if (tasks_.empty())
                        continue;



                    auto task = std::move(tasks_.front());
                    lock.unlock();

                    task();
                }
            });
        }
    }

    //ожидает завершения выполняющихся задач (при этом не начинает выполнения задач из очереди) и завершает все потоки
    void Run(std::function<void()> const & f) {
//        std::cout << "Running" << std::endl;
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(f);
        cv_.notify_one();
    }

    //добавляет задачу в очередь выполнения, откуда ее забирает и один раз выполняет первый освободившийся поток
    ~ThreadPool() {
        running_.store(false);
        cv_.notify_all();// будит все потоки, чтобы разрушить
        for (auto & t : threads_) {
            t.join();
        }
    }
private:
    std::queue<std::function<void()>> tasks_;
    std::atomic_bool running_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
};

int main() {

    std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl;

    int n;
    std::cout << "Input: ";
    std::cin >> n;
    for (auto i = 0; i < n; i++) {
        ThreadPool *tp = new ThreadPool();
        tp->Run([]() -> void {
            it++;
            std::stringstream ss;

//            std::cout << "Value: " << it << std::endl;
            ss << "Value: " << it << std::endl;

            std::cout << ss.str();

        });
    }

    std::operator<<(std::cout, "Value: ");
//    std::cout.operator<<("HERE");

    return 0;
}