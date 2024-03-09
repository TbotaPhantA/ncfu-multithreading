﻿#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <filesystem>
#include <chrono>
#include <future>
#include <math.h>
#include <conio.h>
#include <signal.h>

using namespace std;

// -------------TASK 6------------------------

void sleepSort(const string& word) {
    this_thread::sleep_for(chrono::milliseconds(word.length() * 100)); // Adjust multiplier for sleep time
    std::cout << word << endl;
}

void task6() {
    vector<string> words;

    // Читаем пользовательский ввод
    for (int i = 0; i < 5; i++) {
		string word;
        cin >> word;
        words.push_back(word);
    }

    // Создаём кладём потоки для сортировки
    vector<thread> threads;
    for (const string& word : words) {
        threads.emplace_back(sleepSort, word);
    }

    // Ждём, пока все потоки закончат выполняться
    for (thread& th : threads) {
        th.join();
    }
}

// -------------TASK 7------------------------

namespace fs = std::filesystem;

void copyFile(const string& src, const string& dst) {
    ifstream in(src, ios::binary);
    ofstream out(dst, ios::binary);
    out << in.rdbuf();
}

void copyDir(const string& src, const string& dst) {
    std::filesystem::create_directory(dst);

    std::vector<std::filesystem::path> files, directories;

    for (const auto& entry : std::filesystem::directory_iterator(src)) {
        if (std::filesystem::is_regular_file(entry)) {
            files.push_back(entry.path());
        }
        else if (std::filesystem::is_directory(entry)) {
            directories.push_back(entry.path());
        }
    }

    std::vector<std::future<void>> fileFutures, dirFutures;

    // Запустить копирование файлов асинхронно
    for (const auto& file : files) {
        string newSrc = src + "\\" + file.filename().string();
        string newDst = dst + "\\" + file.filename().string();
        std::cout << "Copying file " << endl << "From: " << newSrc << endl << "To: " << newDst << endl;
        fileFutures.push_back(async(launch::async, copyFile, newSrc, newDst));
    }

    // Запустить копирование директорий асинхронно
    for (const auto& directory : directories) {
        string newSrc = src + "\\" + directory.filename().string();
        string newDst = dst + "\\" + directory.filename().string();
        std::cout << "Copying directory " << endl << "From: " << newSrc << endl << "To: " << newDst << endl;
        dirFutures.push_back(async(launch::async, copyDir, newSrc, newDst));
    }

    for (auto& future : fileFutures) {
        future.get();
    }
    for (auto& future : dirFutures) {
        future.get();
    }
}

int task7() {
    std::cout << "\n\n_________start__________\n\n";
    string src = "C:\\Users\\botak\\Desktop\\ncfu\\semester 6\\Многопоточка\\task7";
    string dst = "C:\\Users\\botak\\Desktop\\ncfu\\semester 6\\Многопоточка\\task7-copy";

    thread t(copyDir, src, dst);
    t.join();

    std::cout << "\n\n_______________end_______________\n\n";

    return 0;
}

// -------------TASK 8------------------------

long double calculate_pi_portion(int start, int end) {
    long double sum = 0;
    for (int i = start; i <= end; i++) {
        sum += ((pow(-1, i)) / (2 * i + 1));
    }
    return sum;
}

void task8Single() {
	long double pi = 0;
    for (int i = 0; i < 1000000000; i++) {
        pi += ((pow(-1, i)) / (2 * i + 1));
    }
    pi *= 4;
    cout << setprecision(10) << pi << endl;
}

void task8Multi() {
    const int NUM_THREADS = 14;
    int iterations_per_thread = 1000000000 / NUM_THREADS;
	long double pi = 0;
    // Create a vector to store futures
    std::vector<std::future<long double>> futures(NUM_THREADS);
    // Launch threads and store their futures
    for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * iterations_per_thread;
        int end = (i == NUM_THREADS - 1) ? 1000000000 - 1 : start + iterations_per_thread - 1;
        futures[i] = std::async(std::launch::async, calculate_pi_portion, start, end);
    }
    // Wait for all threads to finish and accumulate results
    for (auto& future : futures) {
        long double result = future.get();
        pi = pi + result;
    }

    pi = pi * 4;

    std::cout << setprecision(10) << pi << endl;
}

void task8() {
    auto start = chrono::high_resolution_clock::now();

    // task8Single();
    task8Multi();
    
    // ________PRINT EXECUTE TIME________
    auto end = chrono::high_resolution_clock::now();
    auto ms_int = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << ms_int.count() << " ms\n";
}

// -------------TASK 9------------------------

long double calculate_pi_slice(const int THREAD_NUMBER, const int NUM_THREADS, bool* isFinishedExecuting) {
    long double sum = 0;
    long long n = THREAD_NUMBER; 
    for (long long i = 0; true; n += NUM_THREADS, i++) {
        sum += ((pow(-1, n)) / (2 * n + 1));

        if (i % 1000000000 == 0 && *isFinishedExecuting == true) {
            break;
        }
    }

    return sum;
}

bool* isFinishedExecuting = new bool(false);

void cancel_program(int num) {
    cout << "Cancllation started!!!!!!!!!!" << endl;
	*isFinishedExecuting = true;
}

void task9() {
    signal(SIGINT, cancel_program);
    const int NUM_THREADS = 14;
	long double pi = 0;
    // Create a vector to store futures
    std::vector<std::future<long double>> futures(NUM_THREADS);

    // Launch threads and store their futures
    for (int i = 0; i < NUM_THREADS; i++) {
        futures[i] = std::async(std::launch::async, calculate_pi_slice, i, NUM_THREADS, isFinishedExecuting);
    }

    // Wait for all threads to finish and accumulate results
    for (auto& future : futures) {
        long double result = future.get();
        pi = pi + result;
    }

    pi = pi * 4;

    std::cout << setprecision(10) << pi << endl;
    delete isFinishedExecuting;
}

// -------------TASK 10------------------------

void task10() {
    const int number_of_philosophers = 20;

    struct Forks
    {
    public:
        Forks() { ; }
        std::mutex mu;
    };

    auto eat = [](Forks& left_fork, Forks& right_fork, int philosopher_number) {
        std::unique_lock<std::mutex> llock(left_fork.mu);
        std::unique_lock<std::mutex> rlock(right_fork.mu);

        cout << "Философ " << philosopher_number << " ест..." << endl;

        std::chrono::milliseconds timeout(1500);
        std::this_thread::sleep_for(timeout);

        cout << "Философ " << philosopher_number << " закончил есть и думает..." << endl;
    };

    // Создаём вилки и философов
    Forks forks[number_of_philosophers];
    std::thread philosopher[number_of_philosophers];

    // Философы начитают думать
    for (int i = 0; i < number_of_philosophers; ++i) {
        auto philosopher_number = i + 1;
        cout << "Филосов " << philosopher_number << " думает.." << endl;
        auto previous_fork_index = (number_of_philosophers + (i - 1)) % number_of_philosophers;
        philosopher[i] = std::thread(eat, std::ref(forks[i]), std::ref(forks[previous_fork_index]), philosopher_number);
    }

    for (auto& ph : philosopher) {
        ph.join();
    }
}

// -------------TASK 11------------------------

class Task11 {
    int thread_count;
    vector<thread> threads;
    vector<std::thread::id> thread_ids;
    int thread_id;
    int allowed_thread;
    mutex mutex_lock;
    condition_variable cv;
public:
    Task11(int thread_count) : thread_count(thread_count) {
        thread_id = 0;
        allowed_thread = 0;
    }

    int getCurrentThreadId(const std::thread::id& id) {
        int thread_id = 0;
        for (auto& e : thread_ids) {
            if (e == id) return thread_id;
            thread_id++;
        }
        return -1;
    }

    void run() {
        for (int i = 0; i < thread_count; i++) {
            thread t(&Task11::print_thread, this);
            thread_ids.push_back(t.get_id());
            threads.push_back(move(t));
        }

        for (int i = 0; i < thread_count; i++) {
            threads[i].join();
        }
    }

    void waitforallthreadinit() {
        while (1) {
            if (thread_count == thread_ids.size()) return;
        }
    }

    void print_thread() {
        for (int i = 0; i < 10; i++) {
            waitforallthreadinit();
            unique_lock<mutex> lock(mutex_lock);
            // Ждёт, пока условие == false
            cv.wait(lock, [this] { return std::this_thread::get_id() == thread_ids[allowed_thread]; });
            print_chars();
            allowed_thread += 1;
            if (allowed_thread == thread_count) allowed_thread = 0;
            lock.unlock();
            cv.notify_all();
        }
    }

    void print_chars() {
        cout << std::this_thread::get_id() << ": Printing some text..." << endl;
    }
};

void task11() {
    Task11 task11(2);
    task11.run();
}

// -------------TASK 12------------------------

/*
	Прооблемы с 2-мя мьютексами заключаются в том, что 2 мьютекса могут быть получены в разном порядке
	таким образом может возникнуть DEADLOCK. Также 2 мьютекса не харантируют, в каком порядке потоки получат
	доступ к секции кода. 
*/


// -------------TASK 13------------------------

class Task13 {
    int thread_count;
    vector<thread> threads;
    vector<std::thread::id> thread_ids;
    int thread_id;
    int allowed_thread;
    mutex mutex_lock;
    condition_variable cv;
public:
    Task13(int thread_count) : thread_count(thread_count) {
        thread_id = 0;
        allowed_thread = 0;
    }

    int getCurrentThreadId(const std::thread::id& id) {
        int thread_id = 0;
        for (auto& e : thread_ids) {
            if (e == id) return thread_id;
            thread_id++;
        }
        return -1;
    }

    void run() {
        for (int i = 0; i < thread_count; i++) {
            thread t(&Task13::print_thread, this);
            thread_ids.push_back(t.get_id());
            threads.push_back(move(t));
        }

        for (int i = 0; i < thread_count; i++) {
            threads[i].join();
        }
    }

    void waitforallthreadinit() {
        while (1) {
            if (thread_count == thread_ids.size()) return;
        }
    }

    void print_thread() {
        for (int i = 0; i < 10; i++) {
            waitforallthreadinit();
            unique_lock<mutex> lock(mutex_lock);
            // Ждёт, пока условие == false
            cv.wait(lock, [this] { return std::this_thread::get_id() == thread_ids[allowed_thread]; });
            print_chars();
            allowed_thread += 1;
            if (allowed_thread == thread_count) allowed_thread = 0;
            lock.unlock();
            cv.notify_all();
        }
    }

    void print_chars() {
        cout << std::this_thread::get_id() << ": Printing some text..." << endl;
    }
};

void task13() {
    Task13 task13(2);
    task13.run();
}

// -------------TASK 14------------------------

#include <semaphore>

// Семафоры
binary_semaphore sem_first(1);
binary_semaphore sem_second(0);

void thread_first() {
    for (int i = 0; i < 10; i++) {
        // Ждем, пока освободится первый семафор
        sem_first.acquire();

        // Выводим сообщение
        cout << "Первый поток" << endl;

        // Сигнализируем второму потоку
        sem_second.release();
    }
}

void thread_second() {
    for (int i = 0; i < 10; i++) {
        // Ждем, пока освободится второй семафор
        sem_second.acquire();

        // Выводим сообщение
        cout << "Второй поток" << endl;

        // Сигнализируем первому потоку
        sem_first.release();
    }
}

void task14() {
    // Запускаем два потока
    thread t1(thread_first);
    thread t2(thread_second);

    // Ждем завершения потоков
    t1.join();
    t2.join();
}

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // task6();
    // task7();
    // task8();
    // task9();
    // task10();
    // task11();
    // task13();
    task14();
    
    return 0;
}