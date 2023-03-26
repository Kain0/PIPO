//
// Created by andrei on 26.03.23.
//

#include <iostream>
#include <semaphore>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

std::int32_t process_count;

sem_t *sem;
std::uint64_t *answer;

std::uint64_t factorial(int32_t process_id, std::int32_t num) {
    std::uint64_t local_result = 1;

    for (std::int32_t i = process_id; i <= num; i += process_count) {
        local_result *= i;
    }

    return local_result;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        process_count = sysconf(_SC_NPROCESSORS_ONLN) + 1;
    } else {
        process_count = std::stoi(argv[1]);
    }

    std::int32_t num = -1;
    while (num < 0) {
        std::cout << "Write positive num:";
        std::cin >> num;
        std::cout << "\n";
    }
    std::int32_t shm_id = shmget(IPC_PRIVATE, sizeof(std::uint64_t), IPC_CREAT | 0666);
    if (shm_id < 0) {
        std::cerr << "ERROR: cannot create shared memory\n";
        return -1;
    }
    answer = reinterpret_cast<std::uint64_t *>(shmat(shm_id, nullptr, 0));
    *answer = 1;

    std::uint32_t sem_value = 1;
    sem = sem_open("new_sem3", O_CREAT | O_EXCL, 0644, sem_value);
    for (int i = 0; i < process_count; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "ERROR: cannot create process\n";
            sem_unlink("new_sem3");
            sem_close(sem);
            shmdt(answer);
            shmctl(shm_id, IPC_RMID, nullptr);
            return -1;
        } else if (pid == 0) {
            uint64_t local_result = factorial(i + 1, num);
            sem_wait(sem);
            *answer *= local_result;
            sem_post(sem);
            return 0;
        }

    }
    while (wait(nullptr) != -1);
    sem_unlink("new_sem3");
    sem_close(sem);
    std::cout << num << "! = " << *answer;
    shmdt(answer);
    shmctl(shm_id, IPC_RMID, nullptr);

}