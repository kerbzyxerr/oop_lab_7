#pragma once
#include <string>
#include <vector>
#include <fstream>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const std::string& message) = 0;
};

class Subject {
    std::vector<IObserver*> observers;
public:
    void attach(IObserver* observer);
    void detach(IObserver* observer);
    void notify(const std::string& message);
};

class ConsoleObserver : public IObserver {
public:
    void update(const std::string& message) override;
};

class FileObserver : public IObserver {
    std::ofstream logFile;
public:
    FileObserver();
    ~FileObserver();
    void update(const std::string& message) override;
};