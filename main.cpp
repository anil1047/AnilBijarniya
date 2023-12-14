// main.cpp

#include "FileSystem.h"

int main() {
    FileSystem fs;

    while (true) {
        std::cout << "Enter a command: ";
        std::string command;
        std::getline(std::cin, command);

        if (command == "save") {
            std::cout << "Enter the file name to save: ";
            std::string fileName;
            std::getline(std::cin, fileName);
            fs.saveState(fileName);
        } else if (command == "load") {
            std::cout << "Enter the file name to load: ";
            std::string fileName;
            std::getline(std::cin, fileName);
            fs.loadState(fileName);
        } else {
            fs.executeCommand(command);
        }
    }

    return 0;
}
