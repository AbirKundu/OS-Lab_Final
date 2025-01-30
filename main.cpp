#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

class DiningPhilosophers {
private:
    enum State { THINKING, HUNGRY, EATING };
    int N; // Number of philosophers
    std::vector<State> state; // State of each philosopher
    std::vector<std::mutex> mutexes; // Mutex for each philosopher
    std::mutex mtx; // Monitor lock
    std::vector<std::condition_variable> cv; // Condition variables for philosophers

    // Test if a philosopher can eat
    void test(int philosopher) {
        if (state[philosopher] == HUNGRY &&
            state[(philosopher + N - 1) % N] != EATING &&
            state[(philosopher + 1) % N] != EATING) {
            state[philosopher] = EATING;
            cv[philosopher].notify_one();
        }
    }

public:
    // Constructor
    DiningPhilosophers(int num) : N(num), state(num, THINKING), cv(num) {}

    // Philosopher picks up forks
    void pickup_forks(int philosopher) {
        std::unique_lock<std::mutex> lock(mtx);
        state[philosopher] = HUNGRY;
        std::cout << "Philosopher " << philosopher << " is HUNGRY\n";
        test(philosopher);
        while (state[philosopher] != EATING) {
            cv[philosopher].wait(lock);
        }
        std::cout << "Philosopher " << philosopher
                  << " picked up forks " << philosopher
                  << " and " << (philosopher + 1) % N << "\n";
    }

    // Philosopher puts down forks
    void putdown_forks(int philosopher) {
        std::unique_lock<std::mutex> lock(mtx);
        state[philosopher] = THINKING;
        std::cout << "Philosopher " << philosopher
                  << " put down forks " << philosopher
                  << " and " << (philosopher + 1) % N << "\n";
        test((philosopher + N - 1) % N); // Notify left neighbor
        test((philosopher + 1) % N);     // Notify right neighbor
    }

    // Philosopher thinks
    void think(int philosopher) {
        std::cout << "Philosopher " << philosopher << " is THINKING\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Simulate thinking
    }

    // Philosopher eats+
    void eat(int philosopher) {
        std::cout << "Philosopher " << philosopher << " is EATING\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Simulate eating
    }
};

void philosopher(DiningPhilosophers& table, int id) {
    table.think(id);
    table.pickup_forks(id);
    table.eat(id);
    table.putdown_forks(id);
}

int main() {
    int N = 5; // Number of philosophers
    DiningPhilosophers table(N);

    // Start philosopher threads
    std::vector<std::thread> philosophers;
    for (int i = 0; i < N; ++i) {
        philosophers.emplace_back(philosopher, std::ref(table), i);
    }

    // Join philosopher threads
    for (auto& p : philosophers) {
        p.join();
    }

    std::cout << "All philosophers have finished their tasks. Program terminating.\n";
    return 0;
}


