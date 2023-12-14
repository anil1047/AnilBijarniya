// FileSystem.cpp

#include "FileSystem.h"

// Implementation of Directory class
Directory::Directory(const std::string& name) : name(name) {}

bool Directory::isDirectory() const {
    return true;
}

std::vector<std::string> Directory::ls() const {
    std::vector<std::string> contents;
    for (const auto& entry : children) {
        contents.push_back(entry.first);
    }
    return contents;
}

std::string Directory::getName() const {
    return name;
}

void Directory::addChild(const std::string& name, Node* child) {
    children[name] = child;
}

// Implementation of File class
File::File(const std::string& name, const std::string& content) : name(name), content(content) {}

bool File::isDirectory() const {
    return false;
}

std::vector<std::string> File::ls() const {
    return {}; // Files have no contents for ls
}

std::string File::getName() const {
    return name;
}

std::string File::getContent() const {
    return content;
}

void File::append(const std::string& str) {
    content.append(str);
}

public:
    FileSystem() : root(new Directory), currentDirectory(root) {}

    void executeCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "mkdir") {
            std::string dirName;
            iss >> dirName;
            mkdir(dirName);
        } else if (cmd == "cd") {
            std::string path;
            iss >> path;
            cd(path);
        } else if (cmd == "ls") {
            ls();
        } else if (cmd == "touch") {
            std::string fileName;
            iss >> fileName;
            touch(fileName);
        } else if (cmd == "echo") {
            std::string content;
            std::getline(iss, content);
            echo(content);
        } else if (cmd == "mv") {
            std::string source, destination;
            iss >> source >> destination;
            mv(source, destination);
        } else if (cmd == "cp") {
            std::string source, destination;
            iss >> source >> destination;
            cp(source, destination);
        } else if (cmd == "rm") {
            std::string path;
            iss >> path;
            rm(path);
        } else if (cmd == "grep") {
            std::string pattern, filePath;
            iss >> pattern >> filePath;
            grep(pattern, filePath);
        } else if (cmd == "exit") {
            std::exit(0);
        } else {
            std::cout << "Invalid command: " << command << std::endl;
        }
    }

private:
    void mkdir(const std::string& dirName) {
        if (currentDirectory->children.find(dirName) == currentDirectory->children.end()) {
            currentDirectory->children[dirName] = new Directory;
        } else {
            std::cout << "Directory already exists: " << dirName << std::endl;
        }
    }

    void cd(const std::string& path) {
        if (path == "/") {
            currentDirectory = root;
        } else if (path == "..") {
            if (currentDirectory != root) {
                currentDirectory = getParentDirectory(currentDirectory);
            }
        } else {
            Directory* newDir = getDirectory(path);
            if (newDir) {
                currentDirectory = newDir;
            } else {
                std::cout << "Invalid path: " << path << std::endl;
            }
        }
    }

    void ls() {
        std::cout << "Contents: ";
        for (const auto& entry : currentDirectory->ls()) {
            std::cout << entry << " ";
        }
        std::cout << std::endl;
    }

    void touch(const std::string& fileName) {
        if (currentDirectory->children.find(fileName) == currentDirectory->children.end()) {
            currentDirectory->children[fileName] = new File;
        } else {
            std::cout << "File already exists: " << fileName << std::endl;
        }
    }

    void echo(const std::string& content) {
        if (currentDirectory->children.empty()) {
            std::cout << "No file to write content to." << std::endl;
        } else {
            auto it = currentDirectory->children.begin();
            if (!it->second->isDirectory()) {
                static_cast<File*>(it->second)->content = content;
            } else {
                std::cout << "Cannot write to a directory." << std::endl;
            }
        }
    }

    void mv(const std::string& source, const std::string& destination) {
        Node* sourceNode = getNode(source);
        Node* destNode = getNode(destination);

        if (sourceNode && destNode && destNode->isDirectory()) {
            Directory* destinationDirectory = static_cast<Directory*>(destNode);
            destinationDirectory->children[sourceNode->getName()] = sourceNode;
            currentDirectory->children.erase(sourceNode->getName());
        } else {
            std::cout << "Invalid source or destination." << std::endl;
        }
    }

    void cp(const std::string& source, const std::string& destination) {
        Node* sourceNode = getNode(source);
        Node* destNode = getNode(destination);

        if (sourceNode && destNode && destNode->isDirectory()) {
            Directory* destinationDirectory = static_cast<Directory*>(destNode);
            if (sourceNode->isDirectory()) {
                copyDirectory(static_cast<Directory*>(sourceNode), destinationDirectory);
            } else {
                destinationDirectory->children[sourceNode->getName()] = new File(*(static_cast<File*>(sourceNode)));
            }
        } else {
            std::cout << "Invalid source or destination." << std::endl;
        }
    }

    void rm(const std::string& path) {
        Node* target = getNode(path);
        if (target && target != root) {
            getParentDirectoryFrom(root, target)->children.erase(target->getName());
        } else {
            std::cout << "Invalid path or cannot remove root." << std::endl;
        }
    }

    void grep(const std::string& pattern, const std::string& filePath) {
        File* file = getFile(filePath);
        if (file) {
            std::istringstream iss(file->getContent());
            std::string line;
            while (std::getline(iss, line)) {
                if (std::regex_search(line, std::regex(pattern))) {
                    std::cout << line << std::endl;
                }
            }
        } else {
            std::cout << "File not found: " << filePath << std::endl;
        }
    }

    Directory* getParentDirectoryFrom(Directory* directory, Node* node) {
        if (directory->children.find(node->getName()) != directory->children.end()) {
            return directory;
        }

        for (auto& entry : directory->children) {
            if (entry.second->isDirectory()) {
                Directory* result = getParentDirectoryFrom(static_cast<Directory*>(entry.second), node);
                if (result != nullptr) {
                    return result;
                }
            }
        }

        return nullptr;
    }

    Node* getNode(const std::string& path) {
        std::vector<std::string> pathComponents = tokenize(path);

        Node* current = currentDirectory;
        for (const auto& component : pathComponents) {
            if (component == "..") {
                if (current != root) {
                    current = getParentDirectory(current);
                }
            } else {
                Directory* directory = getDirectory(component);
                if (directory) {
                    current = directory;
                } else {
                    Node* file = current->children[component];
                    if (file) {
                        current = file;
                    } else {
                        return nullptr;
                    }
                }
            }
        }

        return current;
    }

    Directory* getDirectory(const std::string& name) {
        Node* node = currentDirectory->children[name];
        if (node && node->isDirectory()) {
            return static_cast<Directory*>(node);
        } else {
            return nullptr;
        }
    }

    void copyDirectory(Directory* source, Directory* destination) {
        Directory* newDirectory = new Directory(*source);
        destination->children[source->getName()] = newDirectory;

        for (auto& entry : source->children) {
            if (entry.second->isDirectory()) {
                copyDirectory(static_cast<Directory*>(entry.second), newDirectory);
            } else {
                newDirectory->children[entry.first] = new File(*(static_cast<File*>(entry.second)));
            }
        }
    }

    File* getFile(const std::string& path) {
        Node* node = getNode(path);
        if (node && !node->isDirectory()) {
            return static_cast<File*>(node);
        } else {
            return nullptr;
        }
    }

    Directory* getParentDirectory(Node* node) {
        if (node == root) {
            return root;
        }

        for (auto& entry : currentDirectory->children) {
            if (entry.second->isDirectory() && entry.second->children.find(node->getName()) != entry.second->children.end()) {
                return static_cast<Directory*>(entry.second);
            }
        }

        return nullptr;
    }

    std::vector<std::string> tokenize(const std::string& path) {
        std::vector<std::string> result;
        std::istringstream iss(path);
        std::string token;
        while (getline(iss, token, '/')) {
            if (!token.empty()) {
                result.push_back(token);
            }
        }
        return result;
    }

     void saveState(const std::string& fileName) {
        std::ofstream file(fileName);
        if (file.is_open()) {
            serialize(root, file);
            file.close();
            std::cout << "File system state saved to " << fileName << std::endl;
        } else {
            std::cout << "Unable to open file for saving: " << fileName << std::endl;
        }
    }

    // Function to deserialize the state of the file system
    void loadState(const std::string& fileName) {
        std::ifstream file(fileName);
        if (file.is_open()) {
            root = deserialize(file);
            currentDirectory = root;
            file.close();
            std::cout << "File system state loaded from " << fileName << std::endl;
        } else {
            std::cout << "Unable to open file for loading: " << fileName << std::endl;
        }
    }

    // Helper function to recursively serialize the state
    void serialize(Node* node, std::ostream& out) {
        if (node->isDirectory()) {
            Directory* directory = static_cast<Directory*>(node);
            out << "D " << directory->getName() << std::endl;
            for (const auto& entry : directory->children) {
                serialize(entry.second, out);
            }
        } else {
            File* file = static_cast<File*>(node);
            out << "F " << file->getName() << " " << file->getContent() << std::endl;
        }
    }

    // Helper function to recursively deserialize the state
    Node* deserialize(std::istream& in) {
        Node* node = nullptr;
        while (true) {
            char type;
            in >> type;
            if (type == 'D') {
                std::string name;
                in >> name;
                node = new Directory(name);
                Directory* directory = static_cast<Directory*>(node);
                while (true) {
                    char nextChar;
                    in >> nextChar;
                    if (nextChar == 'D') {
                        directory->children[name] = deserialize(in);
                    } else if (nextChar == 'F') {
                        std::string fileName, content;
                        in >> fileName >> content;
                        directory->children[fileName] = new File(fileName, content);
                    } else {
                        break;
                    }
                }
            } else if (type == 'F') {
                std::string fileName, content;
                in >> fileName >> content;
                node = new File(fileName, content);
            } else {
                break;
            }
        }
        return node;
    };
