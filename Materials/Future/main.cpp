#include <iostream>
#include <thread>
#include <mutex>

template<typename T>
class Future {
public:
    Future(Future &&);
    const T & Get() const;
    bool IsReady() const;
    void Wait();
};

template<typename T>
class Promise {
public:
    Promise(Promise<T>&&);
    Future<T> GetFuture();
    void Set(const T &);
    void Set(T&&);
    void SetException(const std::exception_ptr &);
};


int main() {


    return 0;
}