//
// Created by andrei on 26.03.23.
//

#include <iostream>
#include <semaphore>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

std::int32_t pthread_count;

sem_t *sem;
std::uint64_t answer = 1;

struct pthread_data {
    std::int32_t thread_id;
    std::int32_t num;
};

void *factorial(void *args) {
    std::uint64_t local_result = 1;
    auto *data = reinterpret_cast<pthread_data *>(args);

    for (std::int32_t i = data->thread_id; i <= data->num; i += pthread_count) {
        local_result *= i;
    }

    sem_wait(sem);
    answer *= local_result;
    sem_post(sem);
    return nullptr;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        pthread_count = sysconf(_SC_NPROCESSORS_ONLN) + 1;
    } else {
        pthread_count = std::stoi(argv[1]);
    }

    std::int32_t num = -1;
    while (num < 0) {
        std::cout << "Write positive num:";
        std::cin >> num;
        std::cout << "\n";
    }

    std::uint32_t sem_value = 1;
    sem = sem_open("new_sem", O_CREAT | O_EXCL, 0644, sem_value);
    pthread_t pthreads[pthread_count];
    pthread_data all_data[pthread_count];
    for (int i = 0; i < pthread_count; ++i) {
        all_data[i].num = num;
        all_data[i].thread_id = 1 + i;
        std::int32_t res = pthread_create(&pthreads[i], nullptr, factorial, reinterpret_cast<void *>(&all_data[i]));
        if (res != 0) {
            std::cerr << "ERROR: cannot create pthread\n";
            sem_unlink("new_sem");
            sem_close(sem);
            return -1;
        }
    }

    for (int i = 0; i < pthread_count; ++i) {
        pthread_join(pthreads[i], nullptr);
    }

    sem_unlink("new_sem");
    sem_close(sem);
    std::cout << num << "! = " << answer;

}