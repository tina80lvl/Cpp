#ifndef FUTURE_FUTURE_H
#define FUTURE_FUTURE_H

#include "SharedState.h"

template <class T>
class Future
{
public:
    Future()
    {
        valid_ = false;
        state_ = nullptr;
    }

    Future(const Future<T> & other) = delete;

    Future(Future<T> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
        valid_ = other.valid_;
        other.valid_ = false;
    }

    Future& operator=(Future<T> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
        valid_ = other.valid_;
        other.valid_ = false;
        return *this;
    }

    T get()
    {
        check();
        std::unique_lock<std::recursive_mutex> locker(state_->locker_);
        if (!isReady())
        {
            state_->waiting_.wait(locker, [&]() { return this->isReady(); });
        }
        valid_ = false;
        if (state_->error_ == nullptr)
        {
            return std::move(state_->value_);
        }
        else
        {
            throw state_->error_;
        }
    }

    void wait()
    {
        check();
        std::unique_lock<std::recursive_mutex> locker(state_->locker_);
        if (isReady())
        {
            return;
        }
        else
        {
            state_->waiting_.wait(locker, [&]() { return this->isReady(); });
        }
    }

    bool isReady()
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        return state_->calculated_;
    }

private:
    explicit Future(std::shared_ptr< SharedState<T> > state)
    {
        valid_ = true;
        state_ = state;
        state_->futureGotten_ = true;
    }

    void check()
    {
        if (state_ && valid_)
        {
            std::lock_guard<std::recursive_mutex> locker(state_->locker_);
            if (!state_->calculated_ && !state_->promiseExists_)
            {
                valid_ = false;
            }
        }
        if (!valid_ || !state_)
        {
            throw std::runtime_error("invalid future");
        }
    }

    bool valid_;
    std::shared_ptr< SharedState<T> > state_;

    friend class Promise<T>;
};



///////For T&///////



template <class T>
class Future<T&>
{
public:
    Future()
    {
        valid_ = false;
        state_ = nullptr;
    }

    Future(const Future<T&> &) = delete;

    Future(Future<T&> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
        valid_ = other.valid_;
        other.valid_ = false;
    }

    Future& operator=(Future<T&> && other) noexcept
    {
        state_ = other.state_;
        other.state_ = nullptr;
        valid_ = other.valid_;
        other.valid_ = false;
        return *this;
    }

    T& get()
    {
        check();
        std::unique_lock<std::recursive_mutex> locker(state_->locker_);
        if (!isReady())
        {
            state_->waiting_.wait(locker, [&]() { return this->isReady(); });
        }
        valid_ = false;
        if (state_->error_ == nullptr)
        {
            return *state_->value_;
        }
        else
        {
            throw state_->error_;
        }
    }

    void wait()
    {
        check();
        std::unique_lock<std::recursive_mutex> locker(state_->locker_);
        if (isReady())
        {
            return;
        }
        else
        {
            state_->waiting_.wait(locker, [&]() { return this->isReady(); });
        }
    }

    bool isReady()
    {
        check();
        std::lock_guard<std::recursive_mutex> locker(state_->locker_);
        return state_->calculated_;
    }

private:
    explicit Future(std::shared_ptr< SharedState<T&> > state)
    {
        valid_ = true;
        state_ = state;
        state_->futureGotten_ = true;
    }

    void check()
    {
        if (state_ && valid_)
        {
            std::lock_guard<std::recursive_mutex> locker(state_->locker_);
            if (!state_->calculated_ && !state_->promiseExists_)
            {
                valid_ = false;
            }
        }
        if (!valid_ || !state_)
        {
            throw std::runtime_error("invalid future");
        }
    }

    bool valid_;
    std::shared_ptr< SharedState<T&> > state_;

    friend class Promise<T&>;
};



///////For void///////



template<>
void Future<void>::get()
{
    check();
    std::unique_lock<std::recursive_mutex> locker(state_->locker_);
    if (!isReady())
    {
        state_->waiting_.wait(locker, [&]() { return this->isReady(); });
    }
    valid_ = false;
    if (state_->error_ != nullptr)
    {
        throw state_->error_;
    }
}


#endif //FUTURE_FUTURE_H
