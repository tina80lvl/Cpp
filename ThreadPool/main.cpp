#include <iostream>
#include <thread>
#include <vector>
#include <stdatomic.h>
#include <queue>

int it = 0;

class ThreadPool {
public:
    //создает заданное количество потоков, ожидающих поступления задач
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency()) : running_(true){ //вызывает конструктор, значениение по умолчанию есть
        std::cout << "Creating" << std::endl;
        for (auto i = 0u; i < num_threads; ++i) {
            threads_.emplace_back([this]() -> void {//добавить поток (этот чувак собирает прям там), там ещё и лямбда
                while (running_.load()) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    if (tasks_.empty())
                        cv_.wait(lock);//если задачи кончились, блочит все потоки
                    if (tasks_.empty())
                        continue;
                    auto t = std::move(tasks_.front()); // переменная t - текущая выполняемая задача
                    lock.unlock();
                    t();
                }
            });
        }
    }

    //ожидает завершения выполняющихся задач (при этом не начинает выполнения задач из очереди) и завершает все потоки
    void Run(std::function<void()> const & f) {// функция, которая запускает операцию потока и запускает задачу
        std::cout << "Running" << std::endl;
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(f);
        cv_.notify_one();// будит 1 поток
    }

    //добавляет задачу в очередь выполнения, откуда ее забирает и один раз выполняет первый освободившийся поток
    ~ThreadPool() {
        running_.store(false);
        cv_.notify_all();// будит все потоки, чтобы разрушить
        for (auto & t : threads_) {//?????????
            t.join();
        }
    }
private:
    std::queue<std::function<void()>> tasks_;
    std::atomic_bool running_;//запись и чтение не используют кэш
    std::mutex mutex_;//для защиты разделяемых данных от одновременного доступа нескольких потоков
    std::condition_variable cv_;//для блокирования потока или нескольких потоков одновременно, пока не произойдет любое из событий
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
            std::cout << "Value: " << it << std::endl;
        });
    }

    return 0;
}