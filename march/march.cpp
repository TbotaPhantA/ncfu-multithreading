#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
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

using namespace std;

// -------------TASK 6------------------------

void sleepSort(const string& word) {
    this_thread::sleep_for(chrono::milliseconds(word.length() * 100)); // Adjust multiplier for sleep time
    cout << word << endl;
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
        cout << "Copying file " << endl << "From: " << newSrc << endl << "To: " << newDst << endl;
        fileFutures.push_back(async(launch::async, copyFile, newSrc, newDst));
    }

    // Запустить копирование директорий асинхронно
    for (const auto& directory : directories) {
        string newSrc = src + "\\" + directory.filename().string();
        string newDst = dst + "\\" + directory.filename().string();
        cout << "Copying directory " << endl << "From: " << newSrc << endl << "To: " << newDst << endl;
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
    cout << "\n\n_________start__________\n\n";
    string src = "C:\\Users\\botak\\Desktop\\ncfu\\semester 6\\Многопоточка\\task7";
    string dst = "C:\\Users\\botak\\Desktop\\ncfu\\semester 6\\Многопоточка\\task7-copy";

    thread t(copyDir, src, dst);
    t.join();

    cout << "\n\n_______________end_______________\n\n";

    return 0;
}

// -------------TASK 8------------------------


int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // task6();
    // task7();

    return 0;
}
