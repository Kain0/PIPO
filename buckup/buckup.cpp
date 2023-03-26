//
// Created by andrei on 26.03.23.
//
#include <iostream>
#include <sys/stat.h>
#include <filesystem>

void make_copy(const std::filesystem::path &from, const std::filesystem::path &to) {
    auto to_zipped = std::string(to.c_str()) + ".gz";

    if (std::filesystem::exists(to_zipped)) {
        if (std::filesystem::last_write_time(from) <= std::filesystem::last_write_time(to_zipped)) {
            return;
        }
    }
    std::string command1 = std::string("cp ") + from.string() + " " + to.string(); //TODO check if work
    system(command1.c_str());

    std::string command2 = std::string("gzip ") + to.string(); //TODO check if work
    system(command2.c_str());
    std::cout << "update:  " << to.string() << "\n";

}

void backup(const std::filesystem::path &from, const std::filesystem::path &to) {
    // delete files that was actually deleted
    for (const auto &req: std::filesystem::directory_iterator(to)) {
        const std::filesystem::path &new_path = req.path();
        std::string name = new_path.filename().string();
        std::filesystem::path destination_path;

        if (name == "." || name == "..") {
            continue;
        }
        if (std::filesystem::is_directory(new_path)) { // dir case
            destination_path = std::string(from.c_str()) + "/" + name; //TODO check if work
        } else if (std::filesystem::is_regular_file(new_path)) { // .gz case
            destination_path = std::string(from.c_str()) + "/" + name.substr(0, name.size() - 3);
        }
        if (!std::filesystem::exists(destination_path)) {
            std::string command = std::string("rm -r ") + new_path.string();  //TODO check if work
            system(command.c_str());
            std::cout << "deleted: " << new_path.string() << "\n";
        }
    }

    for (const auto &req: std::filesystem::directory_iterator(from)) {
        const std::filesystem::path &new_path = req.path();
        std::string name = new_path.filename().string();

        std::filesystem::path copy = std::string(to.c_str()) + "/" + name;

        if (name == "." || name == "..") {
            continue;
        }

        if (std::filesystem::is_directory(new_path)) {
            if (!std::filesystem::is_directory(copy)) {
                mkdir(copy.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            }
            backup(new_path, copy);
        } else if (std::filesystem::is_regular_file(new_path)) {
            make_copy(new_path, copy);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "ERROR: 2 argument required: paths to dirs\n";
        return -1;
    }
    std::filesystem::path from(argv[1]);
    std::filesystem::path to(argv[2]);
    if (!std::filesystem::is_directory(to) || !std::filesystem::is_directory(from)) {
        std::cerr << "ERROR: arguments are not dirs\n";
        return -1;
    }

    try {
        backup(from, to);
    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what();
        return -1;
    }
    std::cout << "done!";
    return 0;
}