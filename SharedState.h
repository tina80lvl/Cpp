//промежуточная штука
#ifndef FUTURE_SHAREDSTATE_H
#define FUTURE_SHAREDSTATE_H

#include <mutex>
#include <condition_variable>

template <class T>
class Promise;

template <class T>
class Future;

template <class T>
class SharedState
{
private:
    SharedState()
    {
        promiseExists_ = true;
        calculated_ = false;
        futureGotten_ = false;
    }

    std::recursive_mutex locker_;
    std::condition_variable_any waiting_;
    std::exception_ptr error_;
    bool promiseExists_;
    bool futureGotten_;
    bool calculated_;
    T value_;

    friend class Promise<T>;
    friend class Future<T>;
};


///////For T&///////
template <class T>
class SharedState<T&>
{
private:
    SharedState()
    {
        promiseExists_ = true;
        calculated_ = false;
        futureGotten_ = false;
    }

    std::recursive_mutex locker_;
    std::condition_variable_any waiting_;
    std::exception_ptr error_;
    bool promiseExists_;
    bool futureGotten_;
    bool calculated_;
    T* value_;

    friend class Promise<T&>;
    friend class Future<T&>;
};



///////For void///////
template <>
class SharedState<void>
{
private:
    SharedState()
    {
        promiseExists_ = true;
        calculated_ = false;
        futureGotten_ = false;
    }

    std::recursive_mutex locker_;
    std::condition_variable_any waiting_;
    std::exception_ptr error_;
    bool promiseExists_;
    bool futureGotten_;
    bool calculated_;

    friend class Promise<void>;
    friend class Future<void>;
};


#endif //FUTURE_SHAREDSTATE_H
