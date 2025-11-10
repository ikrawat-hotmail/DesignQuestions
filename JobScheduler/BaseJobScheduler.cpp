#include <iostream>
#include <map>
#include <memory>

#define pid_t int

enum STATE {
    DESTROYED,
    BLOCKED,
    RUNNABLE,
    RUNNING
};

struct JobDescriptor {
    pid_t PID;
    STATE state;
};

class BaseJobScheduler {
public:
    std::map<pid_t, std::shared_ptr<JobDescriptor>> mRunQueue;
    virtual int registerProcess(pid PID, std::)
};