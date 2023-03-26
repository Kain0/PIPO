//
// Created by andrei on 24.03.23.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: 1 argument required: path to file\n";
        return -1;
    }

    std::ifstream ifs((std::filesystem::path(argv[1])));
    std::cout << argv[1] << "\n";
    if (!ifs.is_open()) {
        std::cerr << "ERROR: cannot open file for reading\n";
        return -1;
    }
    while (!ifs.eof()) {
        std::string delayed_program;
        std::int32_t time;

        ifs >> time >> delayed_program;
        std::cout << time << " " << delayed_program << "\n";
        if (time < 0) {
            std::cerr << "ERROR: negative time for program: " << delayed_program << " \n";
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "ERROR: cannot create process\n";
            return -1;
        } else if (pid == 0) {
            sleep(time);
            std::cout << "run: " << delayed_program << "\n";
            system(delayed_program.c_str());
            return 0;
        }
    }
    while (wait(nullptr) != 1);
    return 0;
}