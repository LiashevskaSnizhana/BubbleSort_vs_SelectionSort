#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <functional>

template<typename T>
void pop_front(std::vector<T> &v)
{
    if (v.size() > 0) {
        v.erase(v.begin());
    }
}

template<typename T>
void selectionSort(std::vector<T> &vec, bool ascending) {
    for (int i = 0; i < vec.size(); ++i) {
        T value = vec[i];
        int index = 0;

        for (int j = i + 1; j < vec.size(); ++j) {
            if (!ascending) {
                if (vec[j] > value) {
                    index = j;
                    value = vec[j];
                }
            } else {
                if (vec[j] < value) {
                    index = j;
                    value = vec[j];
                }
            }
        }

        if (value != vec[i]) {
            T tmp = vec[i];
            vec[i] = vec[index];
            vec[index] = tmp;
        }
    }
}

template<typename T>
void bubbleSort(std::vector<T> &vec, bool ascending) {
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec.size() - i - 1; j++) {
            if (ascending) {
                if (vec[j] > vec[j + 1]) {
                    T temp = vec[j];
                    vec[j] = vec[j + 1];
                    vec[j + 1] = temp;
                }
            } else {
                if (vec[j] < vec[j + 1]) {
                    T temp = vec[j];
                    vec[j] = vec[j + 1];
                    vec[j + 1] = temp;
                }
            }
        }
    }
}


template<typename T>
std::vector<T> sortAsyncBubble(std::vector<T> &data, int n, int m, bool ascending) {

    std::vector<std::thread> threads;
    std::vector<int> parts[m];

    for (int i = 0; i < m; i++)
    {
        if (i == m-1)
        {

            parts[i] = std::vector<int>(data.begin() + i * (n / m), data.end());
        }
        else
        {
            parts[i] = std::vector<int>(data.begin() + i * (n / m), data.begin() + i * (n / m) + (n / m));
        }
    }


    for (int i = 0; i < m; ++i) {
        threads.emplace_back([i, &parts, ascending]() {
            std::vector<T> &vec = parts[i];
            bubbleSort(vec, ascending);
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::vector<T> result;
    result.reserve(n);

    for (int i = 0; i < n; ++i) {
        int minValue;

        if (ascending) {
            minValue = std::numeric_limits<T>::max();
        } else {
            minValue = std::numeric_limits<T>::min();
        }

        std::vector<int> *minList = nullptr;
        for (auto &l : parts) {
            if (l.empty()) {
                continue;
            }
            auto v = l.front();

            if (ascending) {
                if (v < minValue) {
                    minValue = v;
                    minList = &l;
                }
            } else {
                if (v > minValue) {
                    minValue = v;
                    minList = &l;
                }
            }
        }

        pop_front(*minList);
        result.push_back(minValue);
    }

    return result;
}

template<typename T>
std::vector<T> sortAsyncSelection(std::vector<T> &data, int n, int m, bool ascending) {

    std::vector<std::thread> threads;
    std::vector<int> parts[m];

    for (int i = 0; i < m; i++)
    {
        if (i == m-1)
        {

            parts[i] = std::vector<int>(data.begin() + i * (n / m), data.end());
        }
        else
        {
            parts[i] = std::vector<int>(data.begin() + i * (n / m), data.begin() + i * (n / m) + (n / m));
        }
    }


    for (int i = 0; i < m; ++i) {
        threads.emplace_back([i, &parts, ascending]() {
            std::vector<T> &vec = parts[i];
            selectionSort(vec, ascending);
        });
    }

    for (auto &t : threads) {
        t.join();
    }

    std::vector<T> result;
    result.reserve(n);

    for (int i = 0; i < n; ++i) {
        int minValue;

        if (ascending) {
            minValue = std::numeric_limits<T>::max();
        } else {
            minValue = std::numeric_limits<T>::min();
        }

        std::vector<int> *minList = nullptr;
        for (auto &l : parts) {
            if (l.empty()) {
                continue;
            }
            auto v = l.front();

            if (ascending) {
                if (v < minValue) {
                    minValue = v;
                    minList = &l;
                }
            } else {
                if (v > minValue) {
                    minValue = v;
                    minList = &l;
                }
            }
        }

        pop_front(*minList);
        result.push_back(minValue);
    }

    return result;
}

std::chrono::milliseconds measure(const std::function<void()> &func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto stop = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
}

void benchmark_bubble(int n, int m) {
    const int tests = 5;

    std::ofstream file("sorted_bubble.txt");
    std::ofstream f("bubble.csv");
    f << "threads, time_ms, speedup\n";

    std::vector<int> data;
    data.reserve(n);

    for (int i = 0; i < n; ++i) {
        data.push_back(std::rand());
    }

    int def = measure([&data]() { bubbleSort(data, true); }).count();
    int avg_ms[m];

    for (int i = 0; i < tests; ++i) {
        auto func = [&data, n, i, m]() {
            if (m <= 1) {
                bubbleSort(data, true);
            } else {
                sortAsyncBubble(data, n, m, true);
            }
        };

        avg_ms[i] = 0;

        file << "Threads: " << (i + 1) << std::endl;
        for (int j = 0; j < m; ++j) {
            auto ms = measure(func).count();
            avg_ms[i] += ms;

            file << "    Run: " << j << " Time: " << ms << " ms" << std::endl;
        }

        avg_ms[i] /= tests;
    }

    for (int i = 0; i < m; i++) {
        float speedup = (float) def / (float) avg_ms[i];
        f << (i + 1) << ", " << avg_ms[i] << ", " << speedup << "\n";
    }

    f.flush();
    f.close();
}


int main() {
    int n = 15000;
    int m = 10;

    benchmark_bubble(n, m);

    std::cout << "\n";

    srand(42);

    std::vector<int> data;
    data.reserve(10);

    for (int i = 0; i < 10; ++i) {
        data.push_back(std::rand()%100);
    }
    for (int i = 0; i < 10; ++i) {
        std::cout << data[i] << std::endl;
    }

    std::cout << "Selection sort: \n";

    auto start2 = std::chrono::high_resolution_clock::now();

    sortAsyncSelection(data, 10, 3, true);
    auto stop2 = std::chrono::high_resolution_clock::now();

    std::cout << "Async execution time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2).count() << std::endl;


    std::cout << "Bubble sort: \n";
    auto start1 = std::chrono::high_resolution_clock::now();
    sortAsyncBubble(data, 10, 3, true);
    auto stop1 = std::chrono::high_resolution_clock::now();
    std::cout << "Async execution time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1).count() << std::endl;

    return 0;
}