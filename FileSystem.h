// FileSystem.h

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <regex>

class FileSystem {
private:
    class Node {
    public:
        virtual ~Node() = default;
        virtual bool isDirectory() const = 0;
        virtual std::vector<std::string> ls() const = 0;
        virtual std::string getContent() const { return ""; }
    };

    class Directory : public Node {
    public:
        Directory(const std::string& name);
        bool isDirectory() const override;
        std::vector<std::string> ls() const override;
        std::string getName() const;
        void addChild(const std::string& name, Node* child);

    private:
        std::string name;
        std::map<std::string, Node*> children;
    };

    class File : public Node {
    public:
        File(const std::string& name, const std::string& content);
        bool isDirectory() const override;
        std::vector<std::string> ls() const override;
        std::string getName() const;
        std::string getContent() const override;
        void append(const std::string& str);

    private:
        std::string name;
        std::string content;
    };

    Directory* root;
    Directory* currentDirectory;

public:
    FileSystem();
    ~FileSystem();

    void executeCommand(const std::string& command);

private:
    void mkdir(const std::string& dirName);
    void cd(const std::string& path);
    void ls() const;
    void touch(const std::string& fileName);
    void echo(const std::string& content);
    void mv(const std::string& source, const std::string& destination);
    void cp(const std::string& source, const std::string& destination);
    void rm(const std::string& path);
    void grep(const std::string& pattern, const std::string& filePath);

    void saveState(const std::string& fileName);
    void loadState(const std::string& fileName);

    void serialize(Node* node, std::ostream& out);
    Node* deserialize(std::istream& in);

    std::vector<std::string> tokenize(const std::string& path);

    Directory* getParentDirectory(Node* node);
    Directory* getParentDirectoryFrom(Directory* directory, Node* node);

    Directory* getDirectory(const std::string& name);
    File* getFile(const std::string& path);
    void copyDirectory(Directory* source, Directory* destination);
};

