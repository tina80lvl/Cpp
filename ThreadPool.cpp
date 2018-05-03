#include "ThreadPool.h"


void ThreadPool::thrFunc()
{
    thisThreadPool = this;
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> locker(locker_);
            while (queueOfTasks_.empty() && !isEnd_)
            {
                queueEmpty_.wait(locker);
            }
            if (isEnd_)
            {
                break;
            }
            task = queueOfTasks_.front();
            queueOfTasks_.pop();
        }
        task();
    }
};

ThreadPool::ThreadPool(std::size_t numOfThreads)
{
    isEnd_ = false;
    for (std::size_t i = 0; i < numOfThreads; ++i)
    {
        threads_.push_back(std::move(std::thread(&ThreadPool::thrFunc, this)));
    }
};

ThreadPool::~ThreadPool()
{
    locker_.lock();
    isEnd_ = true;
    locker_.unlock();
    queueEmpty_.notify_all();
    for (std::size_t i = 0; i < threads_.size(); ++i)
    {
        threads_[i].join();
    }
};

void ThreadPool::execute(std::function<void()> const & task)
{
    locker_.lock();
    queueOfTasks_.push(task);
    locker_.unlock();
    queueEmpty_.notify_one();
};

void ThreadPool::execute(std::function<void()> && task)
{
    locker_.lock();
    queueOfTasks_.push(std::move(task));
    locker_.unlock();
    queueEmpty_.notify_one();
};

thread_local ThreadPool* ThreadPool::thisThreadPool = nullptr;