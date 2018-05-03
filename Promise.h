#ifndef FUTURE_PROMISE_H
#define FUTURE_PROMISE_H

#include "SharedState.h"
#include "Future.h"

template <class T>
class Promise
{
public:
    Promise()
    {
        state_ = std::shared_ptr< SharedState<T> > (new SharedState<T>);
    }

    Promise(const Promise<T> &) = delete;

    Promise(Promise<T> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
    }

    ~Promise()
    {
        if (state_ == nullptr)
        {
            return;
        }
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        state_->promiseExists_ = false;
        if (state_->futureGotten_ && !state_->calculated_)
        {
            state_->waiting_.notify_all();
        }
    }

    void set(T const & value)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->value_ = value;
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    void set(T&& value)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->value_ = std::move(value);
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    void setException(const std::exception_ptr & exception)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->error_ = exception;
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    Future<T> getFuture()
    {
        check();
        if (state_->futureGotten_)
        {
            throw std::runtime_error("future already gotten");
        }
        Future<T> future(state_);
        return future;
    }

private:
    void check() const
    {
        if (!state_)
        {
            throw std::runtime_error("invalid promise");
        }
    }

    std::shared_ptr< SharedState<T> > state_;
};



///////For T&///////



template <class T>
class Promise<T&>
{
public:
    Promise()
    {
        state_ = std::shared_ptr< SharedState<T&> > (new SharedState<T&>);
    }

    Promise(const Promise<T&> &) = delete;

    Promise(Promise<T&> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
    }

    ~Promise()
    {
        if (state_ == nullptr)
        {
            return;
        }
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        state_->promiseExists_ = false;
        if (state_->futureGotten_ && !state_->calculated_)
        {
            state_->waiting_.notify_all();
        }
    }

    void set(T & value)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->value_ = &value;
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    void setException(const std::exception_ptr & exception)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->error_ = exception;
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    Future<T&> getFuture()
    {
        check();
        if (state_->futureGotten_)
        {
            throw std::runtime_error("future already gotten");
        }
        Future<T&> future(state_);
        return future;
    }

private:
    void check() const
    {
        if (state_ == nullptr)
        {
            throw std::runtime_error("invalid promise");
        }
    }

    std::shared_ptr< SharedState<T&> > state_;
};



///////For void///////



template <>
class Promise<void>
{
public:
    Promise()
    {
        state_ = std::shared_ptr< SharedState<void> > (new SharedState<void>);
    }

    Promise(const Promise<void> &) = delete;

    Promise(Promise<void> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
    }

    ~Promise()
    {
        if (state_ == nullptr)
        {
            return;
        }
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        state_->promiseExists_ = false;
        if (state_->futureGotten_ && !state_->calculated_)
        {
            state_->waiting_.notify_all();
        }
    }

    void set()
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    void setException(const std::exception_ptr & exception)
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        if (state_->calculated_)
        {
            throw std::runtime_error("value alredy set");
        }
        state_->error_ = exception;
        state_->calculated_ = true;
        state_->waiting_.notify_all();
    }

    Future<void> getFuture()
    {
        check();
        if (state_->futureGotten_)
        {
            throw std::runtime_error("future already gotten");
        }
        Future<void> future(state_);
        return future;
    }

private:
    void check() const
    {
        if (state_ == nullptr)
        {
            throw std::runtime_error("invalid promise");
        }
    }

    std::shared_ptr< SharedState<void> > state_;
};


#endif //FUTURE_PROMISE_H
