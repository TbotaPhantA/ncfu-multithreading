#include <iostream>
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
	таким образом может возникнуть DEADLOCK. Также 2 мьютекса не гарантируют, в каком порядке потоки получат
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

// -------------TASK 15------------------------

/*
    Хоть проблема с дедлоком в случае с семаформами остаётся возможной,
    однако с помощью семафоров можно управлять порядком выполнения программы,
    в отличии от ситуации с мьютеками, можно контролировать порядок.
*/

// -------------TASK 16------------------------

//#include <iostream>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//
//using namespace std;
//
//int main() {
//    // Create a pipe 1.
//    int pipefd1[2]; // child -> parent
//    if (pipe(pipefd1) == -1) {
//        perror("pipe");
//        exit(1);
//    }
//
//    // Create a pipe 2.
//    int pipefd2[2]; // parent -> child
//    if (pipe(pipefd2) == -1) {
//        perror("pipe");
//        exit(1);
//    }
//
//    // Fork a child process.
//    pid_t pid = fork();
//    if (pid == -1) {
//        perror("fork");
//        exit(1);
//    }
//
//    // In the parent process, write to the pipe.
//    if (pid != 0) {
//        // Parent process
//        for (int i = 0; i < 10; i++) {
//            // Write a message to the pipe.
//            string message = "first semaphore acquired!";
//            write(pipefd2[1], message.c_str(), message.length());
//
//            cout << "Process 1" << endl;
//            // Wait for the child process to respond.
//            char buffer[1024];
//            read(pipefd1[0], buffer, sizeof(buffer));
//        }
//    }
//    else {
//        // Child process
//        for (int i = 0; i < 10; i++) {
//            // Read a message from the pipe.
//            char buffer[1024];
//            read(pipefd2[0], buffer, sizeof(buffer));
//
//            cout << "Process 2" << endl;
//
//            // Write a message to the pipe.
//            string message = "second semaphore acquired!";
//            write(pipefd1[1], message.c_str(), message.length());
//        }
//    }
//
//    // Close the pipes.
//    close(pipefd1[0]);
//    close(pipefd1[1]);
//    close(pipefd2[0]);
//    close(pipefd2[1]);
//
//    // Wait for the child process to exit.
//    wait(NULL);
//
//    return 0;
//}

// -------------TASK 17------------------------

#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

class ThreadSafeList {
public:
    // Default constructor (empty list)
    ThreadSafeList() {}

    // Parameterized constructor (initializes with elements)
    ThreadSafeList(const std::initializer_list<string>& elements) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto element : elements) {
            data_.push_back(element);
        }
    }

    // Thread-safe bubble sort implementation
    void sort() {
        std::lock_guard<std::mutex> lock(mutex_);  // Acquire lock before modifications

        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                if (*it > *next) {
                    std::swap(*it, *next);
                    swapped = true;
                }
            }
        } while (swapped);
    }

    // Thread-safe size getter (avoids unnecessary locking)
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_); 
        return data_.size();
    }

    // Thread-safe print method
    void print() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& element : data_) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    // Thread-safe method to add a new element
    void add(const std::string& element) {
        std::lock_guard<std::mutex> lock(mutex_);  // Acquire lock before modification
        data_.push_back(element);
    }
private:
    std::list<std::string> data_;
    std::mutex mutex_;
};

void sortPeriodically(ThreadSafeList* list) {
    for (int i = 0; i < 5; i++) {
        this_thread::sleep_for(10s);
        cout << "Sorting list..." << endl;
        list->sort();
        list->print();
    }
}

void launchUserInput(ThreadSafeList* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        } else {
            list->add(line);
        }
    }
}

void task17() {
    ThreadSafeList* list = new ThreadSafeList();

    thread listenToUserThread(launchUserInput, list);
    thread sortingThread(sortPeriodically, list);

    listenToUserThread.join();
    sortingThread.join();

    delete list;
}

// -------------TASK 18------------------------

#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

class ThreadSafeListPerMutex {
public:
    // Default constructor (empty list)
    ThreadSafeListPerMutex() {}

    // Parameterized constructor (initializes with elements)
    ThreadSafeListPerMutex(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element);  // Use add() to ensure mutex creation
        }
    }

    ThreadSafeListPerMutex(const ThreadSafeListPerMutex& other) {
        // Iterate through the source list and create deep copies with new mutexes
        for (const auto& entry : other.data_) {
            data_.push_back({ entry.string });
        }
    }

    // Thread-safe bubble sort implementation (using data_mutex_)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                it->mutex.lock();
                next->mutex.lock();
                if (it->string > next->string) {
                    // Swap strings while holding the lock
                    std::swap(it->string, next->string);
                    swapped = true;
                }
                it->mutex.unlock();
                next->mutex.unlock();
            }
        } while (swapped);
    }


    // Thread-safe size getter (avoids unnecessary locking)
    size_t size() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        return data_.size();
    }

    // Thread-safe print method
    void print() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        for (const auto& element : data_) {
            std::cout << element.string << " ";
        }
        std::cout << std::endl;
    }

    // Thread-safe method to add a new element
    void add(std::string element) {
        std::lock_guard<std::mutex> lock(data_mutex_);  // Protect list modification
        data_.push_back({ element });      // Create entry with a new mutex
    }

    struct StringEntry {
        std::string string;
        std::mutex mutex;

        StringEntry(const std::string& element) : string(element), mutex(std::mutex{}) {}
        StringEntry(const StringEntry& other) : string(other.string), mutex(std::mutex{}) {}
    };

    std::list<StringEntry> data_;
    std::mutex data_mutex_;
};

void task18sortPeriodically(ThreadSafeListPerMutex* list) {
    for (int i = 0; i < 5; i++) {
        this_thread::sleep_for(10s);
        cout << "Sorting list..." << endl;
        list->sort();
        list->print();
    }
}

void task18launchUserInput(ThreadSafeListPerMutex* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        }
        else {
            list->add(line);
        }
    }
}

void task18() {
    ThreadSafeListPerMutex* list = new ThreadSafeListPerMutex();

    thread listenToUserThread(task18launchUserInput, list);
    thread sortingThread(task18sortPeriodically, list);

    listenToUserThread.join();
    sortingThread.join();

    delete list;
}

// -------------TASK 19------------------------

#include <iostream>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

class ThreadSafeListTask19 {
public:
    // Default constructor (empty list)
    ThreadSafeListTask19() {}

    // Parameterized constructor (initializes with elements)
    ThreadSafeListTask19(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element);  // Use add() to ensure mutex creation
        }
    }

    ThreadSafeListTask19(const ThreadSafeListTask19& other) {
        // Iterate through the source list and create deep copies with new mutexes
        for (const auto& entry : other.data_) {
            data_.push_back({ entry.string });
        }
    }

    // Thread-safe bubble sort implementation (using data_mutex_)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                it->mutex.lock();
                next->mutex.lock();
				this_thread::sleep_for(1s);
                cout << "Comparing " << it->string << " and " << next->string << endl;
				this_thread::sleep_for(1s);
                if (it->string > next->string) {
					cout << "Swaping " << it->string << " and " << next->string << endl;
                    std::swap(it->string, next->string);
                    swapped = true;
                }
                else {
					cout << "Keeping " << it->string << " and " << next->string << endl;
                }
                it->mutex.unlock();
                next->mutex.unlock();
            }
        } while (swapped);
    }


    // Thread-safe size getter (avoids unnecessary locking)
    size_t size() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        return data_.size();
    }

    // Thread-safe print method
    void print() {
        std::lock_guard<std::mutex> lock(data_mutex_);
        for (const auto& element : data_) {
            std::cout << element.string << " ";
        }
        std::cout << std::endl;
    }

    // Thread-safe method to add a new element
    void add(std::string element) {
        std::lock_guard<std::mutex> lock(data_mutex_);  // Protect list modification
        data_.push_back({ element });      // Create entry with a new mutex
    }

    struct StringEntry {
        std::string string;
        std::mutex mutex;

        StringEntry(const std::string& element) : string(element), mutex(std::mutex{}) {}
        StringEntry(const StringEntry& other) : string(other.string), mutex(std::mutex{}) {}
    };

    std::list<StringEntry> data_;
    std::mutex data_mutex_;
};

void task19sortPeriodically(ThreadSafeListTask19* list) {
    for (int i = 0; i < 5; i++) {
        this_thread::sleep_for(10s);
        cout << "Sorting list..." << endl;
        list->sort();
        list->print();
    }
}

void task19launchUserInput(ThreadSafeListTask19* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        }
        else {
            list->add(line);
        }
    }
}

void task19() {
    ThreadSafeListTask19* list = new ThreadSafeListTask19();

    thread listenToUserThread(task19launchUserInput, list);
    thread sortingThread(task19sortPeriodically, list);

    listenToUserThread.join();
    sortingThread.join();

    delete list;
}

// -------------TASK 20------------------------

#include <shared_mutex>

class ThreadSafeListPerRWLock {
public:
    // Default constructor (empty list)
    ThreadSafeListPerRWLock() {}

    // Parameterized constructor (initializes with elements)
    ThreadSafeListPerRWLock(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element);  // Use add() to ensure mutex creation
        }
    }

    ThreadSafeListPerRWLock(const ThreadSafeListPerRWLock& other) {
        // Acquire a write lock to ensure consistent copying
        std::unique_lock<std::shared_mutex> write_lock(data_mutex_);

        // Iterate through the source list and create deep copies
        for (const auto& entry : other.data_) {
            data_.push_back({ entry.string });
        }
    }

    // Thread-safe bubble sort implementation (using data_mutex_ for write access)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                // Acquire shared locks for read access
                std::shared_lock<std::shared_mutex> rlock1(it->mutex);
                std::shared_lock<std::shared_mutex> rlock2(next->mutex);

                if (it->string > next->string) {
                    // Acquire a write lock to modify the strings
                    std::unique_lock<std::shared_mutex> wlock(data_mutex_);
                    std::swap(it->string, next->string);
                    swapped = true;
                }
            }
        } while (swapped);
    }

    // Thread-safe size getter (uses a shared lock for read access)
    size_t size() {
        std::shared_lock<std::shared_mutex> lock(data_mutex_);
        return data_.size();
    }

    // Thread-safe print method (uses a shared lock for read access)
    void print() {
        std::shared_lock<std::shared_mutex> lock(data_mutex_);
        for (const auto& element : data_) {
            std::cout << element.string << " ";
        }
        std::cout << std::endl;
    }

    // Thread-safe method to add a new element (uses a write lock for modification)
    void add(std::string element) {
        std::unique_lock<std::shared_mutex> lock(data_mutex_);  // Protect list modification
        data_.push_back({ element });  // Create entry with a new mutex
    }

    struct StringEntry {
        std::string string;
        std::shared_mutex mutex;  // Using shared mutex for individual entries

        StringEntry(const std::string& element) : string(element) {}
        StringEntry(const StringEntry& other) : string(other.string) {}
    };

    std::list<StringEntry> data_;
    std::shared_mutex data_mutex_;  // Using a shared mutex for overall list operations
};

void task20sortPeriodically(ThreadSafeListPerRWLock* list) {
    for (int i = 0; i < 5; i++) {
        this_thread::sleep_for(10s);
        cout << "Sorting list..." << endl;
        list->sort();
        list->print();
    }
}

void task20launchUserInput(ThreadSafeListPerRWLock* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        }
        else {
            list->add(line);
        }
    }
}

void task20() {
    ThreadSafeListPerRWLock* list = new ThreadSafeListPerRWLock();

    thread listenToUserThread(task20launchUserInput, list);
    thread sortingThread(task20sortPeriodically, list);

    listenToUserThread.join();
    sortingThread.join();

    delete list;
}


// -------------TASK 21------------------------
// The difference between shared_lock and unique_lock is...
// Use of std::shared_mutex: This mutex allows multiple readers to access the data concurrently, but only one writer at a time.


class ThreadSafeListTask21 {
public:
    // Default constructor (empty list)
    ThreadSafeListTask21() {}

    // Parameterized constructor (initializes with elements)
    ThreadSafeListTask21(const std::initializer_list<std::string>& elements) {
        for (auto element : elements) {
            add(element);  // Use add() to ensure mutex creation
        }
    }

    // Thread-safe bubble sort implementation (using data_mutex_ for write access)
    void sort() {
        bool swapped;
        do {
            swapped = false;
            auto it = data_.begin();
            auto next = std::next(it);
            for (; next != data_.end(); ++it, ++next) {
                // Acquire shared locks for read access
                std::shared_lock<std::shared_mutex> rlock1(it->mutex);
                std::shared_lock<std::shared_mutex> rlock2(next->mutex);

				this_thread::sleep_for(1s);
                if (it->string > next->string) {
                    // Acquire a write lock to modify the strings
                    std::unique_lock<std::shared_mutex> wlock(data_mutex_);
					cout << "Swaping " << it->string << " and " << next->string << endl;
                    std::swap(it->string, next->string);
                    swapped = true;
                }
                else {
					cout << "Keeping " << it->string << " and " << next->string << endl;
                }
            }
        } while (swapped);
    }

    // Thread-safe size getter (uses a shared lock for read access)
    size_t size() {
        std::shared_lock<std::shared_mutex> lock(data_mutex_);
        return data_.size();
    }

    // Thread-safe print method (uses a shared lock for read access)
    void print() {
        std::shared_lock<std::shared_mutex> lock(data_mutex_);
        for (const auto& element : data_) {
            std::cout << element.string << " ";
        }
        std::cout << std::endl;
    }

    // Thread-safe method to add a new element (uses a write lock for modification)
    void add(std::string element) {
        std::unique_lock<std::shared_mutex> lock(data_mutex_);  // Protect list modification
        data_.push_back({ element });  // Create entry with a new mutex
    }

    struct StringEntry {
        std::string string;
        std::shared_mutex mutex;  // Using shared mutex for individual entries

        StringEntry(const std::string& element) : string(element) {}
        StringEntry(const StringEntry& other) : string(other.string) {}
    };

    std::list<StringEntry> data_;
    std::shared_mutex data_mutex_;  // Using a shared mutex for overall list operations
};

void task21sortPeriodically(ThreadSafeListTask21* list) {
    for (int i = 0; i < 5; i++) {
        this_thread::sleep_for(10s);
        cout << "Sorting list..." << endl;
        list->sort();
        list->print();
    }
}

void task21launchUserInput(ThreadSafeListTask21* list) {
    string line;
    for (int i = 0; i < 20; i++) {
        getline(cin, line);
        if (line.empty()) {
            list->print();
            continue;
        }
        else {
            list->add(line);
        }
    }
}

void task21() {
    ThreadSafeListTask21* list = new ThreadSafeListTask21();

    thread listenToUserThread(task21launchUserInput, list);
    thread sortingThread(task21sortPeriodically, list);

    listenToUserThread.join();
    sortingThread.join();

    delete list;
}

// -------------TASK 22------------------------

#include <condition_variable>

void task22() {
    const int number_of_philosophers = 20;

    struct Forks {
    public:
        Forks() : is_left_available(true), is_right_available(true) {}
        std::mutex mu;
        std::condition_variable left_cond, right_cond;
        bool is_left_available;
        bool is_right_available;
    };

    auto eat = [](Forks& left_fork, Forks& right_fork, int philosopher_number) {
        std::unique_lock<std::mutex> lock(left_fork.mu);

        left_fork.left_cond.wait(lock, [&] { return left_fork.is_left_available; });

        int right_fork_index = (number_of_philosophers + (philosopher_number - 1)) % number_of_philosophers;
        {
            std::unique_lock<std::mutex> right_lock(right_fork.mu);
			right_fork.left_cond.wait(lock, [&] { return right_fork.is_right_available; });

            cout << "Философ " << philosopher_number << " ест..." << endl;

            std::chrono::milliseconds timeout(1500);
            std::this_thread::sleep_for(timeout);

            cout << "Философ " << philosopher_number << " закончил есть и думает..." << endl;

            left_fork.is_left_available = true;
            left_fork.left_cond.notify_one();  // Notify the waiting philosopher for the left fork

            right_fork.is_right_available = true;
            right_fork.right_cond.notify_one(); // Notify the waiting philosopher for the right fork
        }
    };

    // Create forks and philosophers
    Forks forks[number_of_philosophers];
    std::thread philosopher[number_of_philosophers];

    // Philosophers start thinking
    for (int i = 0; i < number_of_philosophers; ++i) {
        auto philosopher_number = i + 1;
        cout << "Философ " << philosopher_number << " думает.." << endl;
        int previous_fork_index = (number_of_philosophers + (i - 1)) % number_of_philosophers;
        philosopher[i] = std::thread(eat, std::ref(forks[i]), std::ref(forks[previous_fork_index]), philosopher_number);
    }

    for (auto& ph : philosopher) {
        ph.join();
    }
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
    // task14();
    // task17();
    // task18();
    // task19();
    // task20();
    // task21();
    task22();
    
    return 0;
}
