#include <iostream>
#include <thread>

void foo()
{
    std::cout << "Starting foo.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Exiting foo.\n";
}

int main()
{
    std::cout << "Starting main.\n";
    {
        std::thread t(foo);
        t.detach();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Exiting main.\n";

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}