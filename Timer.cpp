#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <future>


class ITimer {
public:
	// returns a timer id
	// can be used to call cancel on this timer
	virtual int createTimer(std::chrono::steady_clock::time_point expiresAt, std::function<void()> callback) = 0;
	virtual void cancelTimer(int timerId) = 0;
};

class TimerData {
public:
    std::chrono::steady_clock::time_point mExpiresAt;
    std::function<void()> mCallback;
    int mTimerId;
    
    TimerData(std::chrono::steady_clock::time_point expiresAt,
        std::function<void()> callback,
        int timerId
    ) : mCallback(callback), mExpiresAt(expiresAt), mTimerId(timerId) {};
   
};

struct TimerCompare {
    bool operator () (const TimerData& t1, const TimerData& t2) {
        return t1.mExpiresAt < t2.mExpiresAt; // max-heap behavior
    }
};


class TimerImpl : public ITimer {
private:
	static ITimer *mTimerInstance;
	
	// delete copy operators
	TimerImpl(const TimerImpl&) = delete;
	TimerImpl operator=(const TimerImpl&) = delete;
	
	TimerImpl() {
		std::cout << "Initializing a timer class" << std::endl;
        mThread = std::thread(&TimerImpl::run, this);
	}	
	
	void run() {
        std::vector<std::future<void>> waitingList;
        std::unique_lock<std::mutex> lock(mQueueMutex);

        while (true) {
            while (mTimerQueue.size()) {
                TimerData timerData = mTimerQueue.top();
                auto now = std::chrono::steady_clock::now();
                mTimerQueue.pop();

                if (timerData.mExpiresAt > now) {
                    std::future<void> result = std::async(std::launch::async, timerData.mCallback);
                    waitingList.push_back(result);
                } else {
                    std::this_thread::sleep_for(timerData.mExpiresAt - now);
                }
            }
            mIdleWait.wait(lock);
        }
	}
	
	void updateTimers() {
		
	}
	
public:	
	static ITimer* getInstance() {
		if (mTimerInstance == nullptr)
			mTimerInstance = new TimerImpl;
		return mTimerInstance;
	}
	
	int createTimer(std::chrono::steady_clock::time_point expiresAt, std::function<void()> callback) {
		TimerData timerDataObj(expiresAt, callback, mUniqueTimerId);
		mTimerQueue.push(timerDataObj);
        if (mTimerQueue.size() == 1)
            mIdleWait.notify_one();
		mUniqueTimerId += 1;

        return mUniqueTimerId;
	}
	
	void cancelTimer(int timerId) {
	
	}
	

	std::priority_queue<TimerData&, std::vector<TimerData&>, TimerCompare> mTimerQueue;
	int mUniqueTimerId = 0;
    std::thread mThread;
    std::condition_variable mIdleWait;
    std::mutex mQueueMutex;
};


static ITimer *mTimerInstance = nullptr;
