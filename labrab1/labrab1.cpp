#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
#include <string>
#include <future>

using namespace std;

void print10Lines() {
	for (int i = 0; i < 10; i++) {
		cout << i + 1 << " line\n";
	}
}

void task1() {
    thread thread1(print10Lines);
	thread1.join();
}

void task2() {
	// pthread_join в C позволяет возвращать результат из дочернего потока, того
	// же самомго можно добиться и с помощью future в C++
	future<void> resultFuture = async(print10Lines);
	cout << "Child:" << endl;
	resultFuture.get();
	cout << "Parent:" << endl;
	print10Lines();
}

void printTextLines(const vector<string>& lines, mutex& mtx) {
	lock_guard<mutex> lock(mtx);
	for (auto& line : lines) {
		cout << line << endl;
	}
	cout << endl;
}

void task3() {
	vector<vector<string>> lines = {
	  {"bananus", "orange", "apple"},
	  {"potato", "cucumber", "lime"},
	  {"lemon", "coconut", "penut"},
	  {"amogus", "fire", "ice"},
	};

	vector<thread> threads;
	mutex mtx;

	for (const auto& line : lines) {
		threads.emplace_back(printTextLines, ref(line), ref(mtx));
	}

	for (auto& thread : threads) {
		thread.join();
	}
}

void childThreadFunction() {
	for (int i = 0; i < 10; ++i) {
		this_thread::sleep_for(chrono::seconds(1));
		cout << "Child thread: Line " << i + 1 << endl;
	}
}

void task4() {
	thread childThread(childThreadFunction);
	this_thread::sleep_for(chrono::seconds(3));

	cout << "Parent thread: Canceling child thread..." << endl;
	childThread.detach(); // Прервать дочерний поток
	cout << "Parent thread: Child thread detached." << endl;
}



void childThreadFunctionGraceful(std::atomic_bool& detached) {
	for (int i = 0; i < 10; ++i) {
		this_thread::sleep_for(chrono::seconds(1));
		cout << "Child thread: Line " << i + 1 << endl;

		if (detached) {
			cout << "Child is detached";
			break;
		}
	}
}

void task5() {
	std::atomic_bool detached{ false };
	thread childThread(childThreadFunctionGraceful, ref(detached));
	this_thread::sleep_for(chrono::seconds(3));
	detached = true;
	childThread.detach(); // Прервать дочерний поток
	this_thread::sleep_for(chrono::milliseconds(500));
}

int main()
{
	// task1();
	// task2();
	// task3();
	// task4();
	task5();
	return 0;
}


