#include "observer/observer.h"
#include <iostream>
#include <algorithm>

void Subject::attach(IObserver* observer) {
    observers.push_back(observer);
}

void Subject::detach(IObserver* observer) {
    for (auto it = observers.begin(); it != observers.end();) {
        if (*it == observer) {
            it = observers.erase(it);
        } else {
            ++it;
        }
    }
}

void Subject::notify(const std::string& message) {
    for (auto observer : observers) {
        observer->update(message);
    }
}

void ConsoleObserver::update(const std::string& message) {
    std::cout << "[Battle] " << message << std::endl;
}

FileObserver::FileObserver() {
    logFile.open("log.txt", std::ios::app);
}

FileObserver::~FileObserver() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void FileObserver::update(const std::string& message) {
    if (logFile.is_open()) {
        logFile << "[Battle] " << message << std::endl;  
    }
}