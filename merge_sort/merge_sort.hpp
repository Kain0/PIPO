//
// Created by andrei on 26.03.23.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <concepts>
#include <pthread.h>

template<typename T> requires std::totally_ordered<T>
class MergeSort {

    struct pthread_data {
        std::size_t left;
        std::size_t right;
        std::vector<T> *elements;
    };

public:
    explicit MergeSort(const std::vector<T> &elements) : elements_(elements) {}

    std::vector<T> sorted() {
        pthread_data data = {0, static_cast<std::size_t>(elements_.size() - 1), &elements_};
        pthread_sort(reinterpret_cast<void *>(&data));
        return elements_;
        //
    }

private:

    static void merge(std::vector<T> *elements, std::size_t left, std::size_t m, std::size_t right) {
        std::size_t left_l = m - left + 1;
        std::size_t right_l = right - m;

        std::vector<T> left_vector(elements->begin() + left, elements->begin() + m + 1);
        std::vector<T> right_vector(elements->begin() + m + 1, elements->begin() + right + 1);

        std::size_t i = 0;
        std::size_t j = 0;
        while (i < left_l && j < right_l) {
            if (left_vector[i] <= right_vector[j]) {
                (*elements)[left + i + j] = left_vector[i];
                ++i;
            } else {
                (*elements)[left + i + j] = right_vector[j];
                ++j;
            }
        }

        while (i < left_l) {
            (*elements)[left + i + j] = left_vector[i];
            ++i;
        }

        while (j < right_l) {
            (*elements)[left + i + j] = right_vector[j];
            ++j;
        }
    }

    static void *pthread_sort(void *args) {
        auto *data = reinterpret_cast<pthread_data*>(args);
        if (data->right <= data->left) {
            return nullptr;
        }

        pthread_t pthread_left;
        pthread_t pthread_right;
        pthread_data data_left;
        pthread_data data_right;
        std::size_t m = (data->left + data->right) / 2;

        create_thread(pthread_left, data_left, data->left, m, data->elements);
        create_thread(pthread_right, data_right, m + 1, data->right, data->elements);

        pthread_join(pthread_left, nullptr);
        pthread_join(pthread_right, nullptr);
        merge(data->elements, data->left, m, data->right);
        return nullptr;

    }

    static void create_thread(pthread_t &thread, pthread_data &data, std::size_t left, std::size_t right,
                              std::vector<T> *elements) {
        data.left = left;
        data.right = right;
        data.elements = elements;
        pthread_create(&thread, nullptr, pthread_sort, reinterpret_cast<void *>(&data));
    }

private:
    std::vector<T> elements_;
};
