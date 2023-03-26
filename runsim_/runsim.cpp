//
// Created by andrei on 25.03.23.
//

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

namespace {
    std::int32_t *process_count;
    std::int32_t max_processes_count;
    std::int32_t max_command_size = 2048;
}

int create_process(char *command) {
    if (*process_count >= max_processes_count) {
        std::cout << "ERROR: maximum number of processes reached\n";
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "ERROR: cannot create process\n";
        return -1;
    } else if (pid == 0) {
        (*process_count)++;
        std::cout << "count: " << *process_count << "\n";
        sleep(10);
        system(command);
        (*process_count)--;
        exit(0);
    }
    return 0;
}

int start() {
    std::int32_t shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        std::cerr << "ERROR: cannot create shared memory\n";
        return -1;
    }
    process_count = reinterpret_cast<std::int32_t *>(shmat(shm_id, nullptr, 0));
    *process_count = 0;

    while (!feof(stdin)) {
        char command[max_command_size];
        scanf("%s", command);
        create_process(command);

    }
    while (wait(nullptr) != -1);
    shmdt(process_count);
    shmctl(shm_id, IPC_RMID, nullptr);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: 1 argument required: max number of processes\n";
        return -1;
    }

    max_processes_count = std::stoi(argv[1]);
    return start();
}