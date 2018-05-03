#ifndef FUTURE_MAP_H
#define FUTURE_MAP_H

#include "ThreadPool.h"

template <class T, class F>
Future<class std::result_of<F(T)>::type> map(Future<T> futureIn, F const & function)
{
    std::shared_ptr< Promise<class std::result_of<F(T)>::type> > promiseRef(new Promise<class std::result_of<F(T)>::type>());
    Future<class std::result_of<F(T)>::type> futureOut = promiseRef->getFuture();
    ThreadPool* thisThreadPool = ThreadPool::thisThreadPool;
    if (thisThreadPool)
    {
        thisThreadPool->execute(
            [&futureIn, promiseRef, &function]
            {
                try
                {
                    promiseRef->set(function(futureIn.get()));
                }
                catch (...)
                {
                    promiseRef->setException(std::current_exception());
                }
            }
        );
    }
    else
    {
        std::thread([&futureIn, promiseRef, &function]
        {
            try
            {
                promiseRef->set(function(futureIn.get()));
            }
            catch (...)
            {
                promiseRef->setException(std::current_exception());
            }
        }).detach();
    }
    return std::move(futureOut);
};


#endif //FUTURE_MAP_H
