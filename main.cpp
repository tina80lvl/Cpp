#include <iostream>
#include <thread>
#include <vector>
#include <tuple>
#include "Promise.h"
#include "Flatten.h"
#include "Map.h"

void square(int a)
{
    std::cout << a << ' ' << a * a << '\n';
};

int main()
{
/*
    const int x = 10;
    Promise<int> promise1;
    promise1.set(x);
    Future<int> future1 = std::move(promise1.getFuture());

    Promise<Future<int>> promise2;
    promise2.set(std::move(future1));
    Future<Future<int>> f = std::move(promise2.getFuture());
    Future<int> future = std::move(flatten(std::move(f)));
    */

//  ❕FLATTEN without tuple❕
    /*{
        Promise<Future<int>> p1_1;
        Promise<int> p1_2;
        p1_1.set(p1_2.getFuture());
        Future<Future<int>> f1 = p1_1.getFuture();
        Future<int> f2 = std::move(flatten(std::move(flatten(std::move(f1)))));
        p1_2.set(237);
        std::cout << "Waiting..." << std::flush;
        f2.wait();
        std::cout << "Done!\nResult is: " << f2.get() << '\n';
    }

    {
        Promise<Future<Future<int>>> p1;
        Promise<Future<int>> p2;
        Promise<int> p3;
        p3.set(10);
        p2.set(p3.getFuture());
        p1.set(p2.getFuture());
        Future<int> f = std::move(flatten(p1.getFuture()));
        std::cout << f.get() << std::endl;
    }

    {
        std::vector<Promise<int> > promises(10);
        std::vector<Future<int> > futures(10);

        for(int i = 0; i != 10; ++i) {
            promises[i].set(rand());
            futures[i] = promises[i].getFuture();
        }
        Future<std::vector<int> > result = std::move(flatten(futures));
    }

    {
        std::vector<Future<int> > v;
        Promise<int> p1;
        Promise<int> p2;
        Promise<int> p3;
        v.push_back(p1.getFuture());
        v.push_back(p2.getFuture());
        v.push_back(p3.getFuture());
        std::thread thread([&]
                           {
                               std::this_thread::sleep_for(std::chrono::seconds(1));
                               p1.set(3);
                               p2.set(2);
                               p3.set(1);
                           });
        Future<std::vector<int> > f = std::move(flatten(v));
        std::cout << f.isReady() << std::endl;
        std::vector<int> out = f.get();
        for (size_t i = 0; i < out.size(); ++i)
        {
            std::cout << out[i] << std::endl;
        }
        thread.join();
    }*/
/*MAP
    Promise<int> pm;
    Future<int> fm = pm.getFuture();
    std::thread thread1([&] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        pm.set(5);
    });

    std::cout << map(std::move(fm), &square).get() << std::endl;

    thread1.join();

    Promise<int> pm2;
    Future<int> fm2 = pm2.getFuture();
    std::thread thread2([&] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        pm2.set(7);
    });

    ThreadPool tp(2);
    tp.execute([&fm2]{
        std::cout << map(std::move(fm2), &square).get() << std::endl;
    });

    thread2.join();
*/
    {
        Promise<int> pt1;
        std::tuple<int, Future<int>, char> t1(1, std::move(pt1.getFuture()), 'd');
        auto res = flatten(t1);
        std::thread([](Promise<int> p_)
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        p_.set(2);
                    }, std::move(pt1)).detach();
        auto resTuple = res.get();
        std::cout << std::get<0>(resTuple) << " " << std::get<1>(resTuple) << " " << std::get<2>(resTuple) << std::endl;
    }

    {
        Promise<int> pt2;
        std::tuple<int, Future<int>, char> t2(1, std::move(pt2.getFuture()), 'd');
        std::thread threadT1([&]
                             {
                                 std::this_thread::sleep_for(std::chrono::seconds(1));
                                 pt2.set(7);
                             });

        ThreadPool tp(2);
        tp.execute([&t2]
                   {
                       auto res = flatten(t2);
                       auto resTuple = res.get();
                       std::cout << std::get<0>(resTuple) << " " << std::get<1>(resTuple) << " "
                                 << std::get<2>(resTuple) << std::endl;
                   });

        threadT1.join();
    }

    {
        std::tuple<> emptyTuple;
        auto resFutureEmptyTuple = flatten(emptyTuple);
        std::tuple<> resEmptyTuple = resFutureEmptyTuple.get();
    }

    {
        Promise<Future<int> > p1;
        Promise<int> p2;
        p1.set(p2.getFuture());
        std::tuple<Future<Future<int> >, int> t(p1.getFuture(), 99);
        std::thread thread([&p2]
                           {
                               std::this_thread::sleep_for(std::chrono::seconds(1));
                               p2.set(7);
                           });
        auto resft = flatten(t);
        std::tuple<int, int> resTuple = resft.get();
        std::cout << std::get<0>(resTuple) << " " << std::get<1>(resTuple) << std::endl;

        thread.join();
    }

    {
        std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
        ThreadPool tp(3);
        tp.parallel(v.begin(), v.end(), square);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}
