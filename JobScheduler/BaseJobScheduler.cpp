#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <initializer_list>
#include <set>

#define pid_t int
#define tid_t int

using Callback = std::function<void ()>;

enum STATE {
    DESTROYED,
    READY,
    RUNNABLE,
    RUNNING
};

struct JobDescriptor {
    pid_t PID;
    STATE state;
    int priority;
    int deadline;                                           // if used in Earliest deadline sched
    std::unordered_map<std::string, Callback> callbacks;
};


/* 
    Lifecycle of my threads

    READY   -->   RUNNABLE  --> RUNNING ---> DESTROYED
                     /|\          |
                      |           |
                      ------------
*/

class BaseJobScheduler {
public:
    std::map<pid_t, std::shared_ptr<JobDescriptor>> mRunQueue;

    virtual int registerProcess(pid_t PID, std::initializer_list<std::pair<std::string, Callback>> list) {
        std::set<std::string> requiredCallbacks = {"execute", "interrupt"};

        // mandatory callback checks
        std::set<std::string> provided;
        for (const auto& pair : list) {
            provided.insert(pair.first);
        }

        for (const auto& requiredcb : requiredCallbacks) {
            if (provided.find(requiredcb) == provided.end()) {
                std::cerr << "Required callback "<< requiredcb << "not registered.\n";
                return -1;
            }
        }

        auto it = mRunQueue.find(PID);
        if (it == mRunQueue.end()) {
            auto job = std::make_shared<JobDescriptor>();
            job->PID = PID;
            job->state = RUNNABLE;
            mRunQueue[PID] = job;
            it = mRunQueue.find(PID);
        }

        // Register callbacks
        auto& job = it->second;
        for (const auto& pair : list) {
            job->callbacks[pair.first] = pair.second;
        }

        return 0;
    }

private:
    virtual int schedule() {

    }

    virtual int execute(pid_t pid) {

    }

    virtual int interrupt(pid_t pid) {

    }

    virtual int destroy(pid_t pid) {

    }
};