#ifndef FUTURE_FLATTEN_H
#define FUTURE_FLATTEN_H

#include <thread>
#include <tuple>
#include <utility>

#include "Promise.h"
#include "ThreadPool.h"


template <class T>
struct InnerType
{
    using type = T;
};

template <class T>
struct InnerType< Future<T> >
{
    using type = typename InnerType<T>::type;
};

template <class T>
struct FlattenImpl;

template <class T>
struct FlattenImpl< Future<T> >
{
    FlattenImpl(Future<T> future)
    {
        ans = std::move(future);
    }

    Future<T> ans;
};

template <class T>
struct FlattenImpl< Future< Future<T> > >
{
    FlattenImpl(Future< Future<T> > future)
    {
        ans = std::move(FlattenImpl< Future<T> >(std::move(future.get())).ans);
    }

    Future<T> ans;
};

template <class T>
Future<T> flatten(Future<T> future)
{
    return std::move(future);
};

template <class T>
Future<class InnerType< Future<T> >::type > flatten(Future< Future<T> > future)
{
    Promise<class InnerType< Future<T> >::type> promise;
    Future<class InnerType< Future<T> >::type> futureOut = std::move(promise.getFuture());

    std::thread([] (Promise<class InnerType< Future<T> >::type> promise1, Future< Future<T> > future1) {
        Future<class InnerType< Future<T> >::type> f = std::move(FlattenImpl< Future< Future<T> > >(std::move(future1)).ans);
        try
        {
            promise1.set(f.get());
        }
        catch(...)
        {
            promise1.setException(std::current_exception());
        }
    }, std::move(promise), std::move(future)).detach();
    return std::move(futureOut);
};



template <template <class, class...> class C, class T, class A>
Future< C <T> > flattenImpl(C< Future<T>, A > & container)
{
    C<T> newContainer;
    auto i = newContainer.end();
    for (auto j = container.begin(); j != container.end(); ++j)
    {
        newContainer.insert(i, j->get());
        i = newContainer.end();
    }
    Promise< C<T> > promise;
    promise.set(std::move(newContainer));
    return std::move(promise.getFuture());
};

template <template <class, class...> class C, class T, class A>
Future< C<T> > flatten(C< Future<T>, A > & container)
{
    Promise< C<T> > promise;
    Future< C<T> > future = promise.getFuture();

    std::thread([] (Promise<C <T> > promise1, C< Future<T>, A > & container1) {
        Future< C<T> > f = flattenImpl(container1);
        try
        {
            promise1.set(f.get());
        }
        catch(...)
        {
            promise1.setException(std::current_exception());
        }
    }, std::move(promise), std::ref(container)).detach();
    return std::move(future);
};



template <class T>
struct RemoveFutures;

template <class... Args>
struct RemoveFutures< std::tuple<Args...> >
{
    using type = std::tuple<typename InnerType<Args>::type...>;
};

template <class T>
T WaitImpl(const T & arg)
{
    return arg;
}

template <class T>
typename InnerType< Future<T> >::type WaitImpl(Future<T> & arg)
{
    return flatten(std::move(arg)).get();
}

template <class... Args, size_t... Indices>
typename RemoveFutures< std::tuple<Args...> >::type wait(std::tuple<Args...> & tupleIn, std::index_sequence<Indices...>)
{
    return std::make_tuple(WaitImpl(std::get<Indices>(tupleIn))...);
}

template <class... Args>
Future<class RemoveFutures< std::tuple<Args...> >::type> flatten(std::tuple<Args...> & tupleIn)
{
    std::shared_ptr< Promise<class RemoveFutures< std::tuple<Args...> >::type> > promiseRef(new Promise<class RemoveFutures< std::tuple<Args...> >::type>());
    Future<class RemoveFutures< std::tuple<Args...> >::type> futureOut = promiseRef->getFuture();
    ThreadPool* thisThreadPool = ThreadPool::thisThreadPool;
    if (thisThreadPool)
    {
        thisThreadPool->execute(
            [promiseRef, &tupleIn]
            {
                try
                {
                    promiseRef->set(wait(tupleIn, std::make_index_sequence<sizeof...(Args)>()));
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
        std::thread([promiseRef, &tupleIn]
        {
            try
            {
                promiseRef->set(wait(tupleIn, std::make_index_sequence<std::tuple_size<std::tuple<Args...> >::value>()));
            }
            catch (...)
            {
                promiseRef->setException(std::current_exception());
            }
        }).detach();
    }
    return futureOut;
};


#endif //FUTURE_FLATTEN_H