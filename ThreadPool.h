#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <thread>
#include <vector>
#include <condition_variable>
#include <queue>
#include <iterator>


class ThreadPool {
public:
    explicit ThreadPool(std::size_t);

    ~ThreadPool();

    void execute(std::function<void()> const &);

    void execute(std::function<void()> &&);

    template<class Iter, class F>
    void parallel(Iter begin, Iter end, F const &function) {

        std::size_t si = end - begin;
        unsigned int step = si > threads_.size() ? si / (threads_.size() + 1) : 1;
        for (size_t i = 0; i < threads_.size() && i < si; ++i) {
            execute([begin, step, &function] {
                for (auto iter = begin; iter != begin + step; ++iter) {
                    function(*iter);
                }
            });
            begin += step;
        }
        for (auto i = begin; i != end; ++i) {
            function(*i);
        }
    };

public:
    static thread_local ThreadPool *thisThreadPool;

private:
    void thrFunc();

private:
    std::mutex locker_;
    std::condition_variable queueEmpty_;
    std::vector<std::thread> threads_;
    std::queue<std::function<void()> > queueOfTasks_;
    bool isEnd_;
};


#endif //THREADPOOL_H
//Parallel(begin, end, f)
