//
// Created by andrei on 26.03.23.
//
#include <random>
#include "merge_sort.hpp"

void write(const std::vector<int32_t> &v) {
    for (int i: v) {
        std::cout << i << " ";
    }
    std::cout << "\n";
}


int main(int argc, char *argv[]) {

    // random vector
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};  // Generates random integers
    std::uniform_int_distribution<std::int32_t> dist{1, 10};

    auto gen = [&dist, &mersenne_engine]() {
        return dist(mersenne_engine);
    };
    std::vector<std::int32_t> v(20);
    generate(begin(v), end(v), gen);
    write(v);

    //sort
    MergeSort<int32_t> sorter(v);
    v = sorter.sorted();
    write(v);
}