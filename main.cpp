#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <condition_variable>
using namespace std;

condition_variable cv;
mutex cv_mutex;
unique_lock<mutex> ul(cv_mutex);

mutex cout_mutex;
std::vector<std::mutex *> forks(5);

void mutexInit() {
    for (int i=0; i<5; ++i)
        forks[i] = new std::mutex();
}

bool tryTakeFork( int left_fork, int righr_fork ) {
    if ( forks[left_fork]->try_lock() ) {
        if ( forks[righr_fork]->try_lock() ) {
            return true;
        }
        else {
            forks[left_fork]->unlock();
        }
    }
    return false;
}

void putForksDown( int left_fork, int right_fork ) {
    forks[left_fork]->unlock();
    forks[right_fork]->unlock();
}

void tryEat( string philosopher, int left_fork, int right_fork ) {

    int left = left_fork;
    int right = right_fork;
    string recurvePhylo = philosopher;

    if ( tryTakeFork( left, right ) ) {

        cout_mutex.lock();
        cout << philosopher << " ест" << endl;
        cout_mutex.unlock();
        this_thread::sleep_for(chrono::milliseconds(3000));
        putForksDown( left, right );
        cv.notify_all();
        cout_mutex.lock();
        cout << philosopher << " думает о философии,  потому что уже поел" << endl;
        cout_mutex.unlock();
        this_thread::sleep_for(chrono::milliseconds((rand() % 3 +1)) * 1000);
        tryEat( recurvePhylo, left, right );

    }
    else {

        cout_mutex.lock();
        cout << philosopher << " думает о философии, потому что не смог поесть" << endl;
        cout_mutex.unlock();
        cv.wait(ul);
        tryEat( recurvePhylo, left, right );

    }
}


int main() {
    mutexInit();

    thread ph1(tryEat, "Сократ", 0, 1);
    thread ph2(tryEat, "Демагог", 1, 2);
    thread ph3(tryEat, "Фрейд", 2, 3);
    thread ph4(tryEat, "Ницше", 3, 4);
    thread ph5(tryEat, "Аристотель", 4, 0);

    ph1.join();
    ph2.join();
    ph3.join();
    ph4.join();
    ph5.join();

    for(int i=0;i<5;++i) delete forks[i];

    return 0;
}
