// running as a service in some code

class ITimer {
public:
	// returns a timer id
	// can be used to call cancel on this timer
	virtual int createTimer(std::chrono::steady_clock::time_point expiresAt, std::function<void()> callback) = 0;
	virtual void cancelTimer(int timerId) = 0;
};


class TimerImpl : public ITimer {
public:	
	ITimer* getInstance() {
		if (timer == nullptr)
			timer = new TimerImpl;
		return timer;
	}
	
	int createTimer(std::chrono::steady_clock::time_point expiresAt, std::function<void()> callback) {
		TimerData *timerDataObj = new TimerData(expiresAt, callback, uniqueTimerId);
		timerDataObj.insert(timerDataObj);
		int uniqueTimerId += 1;
	}
	
	void cancelTimer(int timerId) {
	
	}
	
private:
	static ITimer *timer;
	
	// delete copy operators
	ITimer(const ITimer&) = delete;
	ITimer operator=(const ITimer&) = delete;
	
	ITimer() : timer {
		cout << "Initializing a timer class" << endl;
		std::thread t1;
	}
	
	struct TimerData {
		std::chrono::steady_clock::time_point mExpiresAt;
		std::function<void()> mCallback;
		int mTimerId;
		
		TimerData(std::chrono::steady_clock::time_point expiresAt,
			std::function<void()> callback,
			timerId
		) : mCallback(callback), mExpiresAt(expiresAt), mTimerId(timerId);
		
		
		bool operator()(const TimerData& t1, const TimerData& t2) {
			return t1.mExpiresAt < t2.mExpiresAt; // max-heap behavior
		}
	}
	
	void run() {
	}
	
	void updateTimers() {
		
	}
	
	std::priority_queue<TimerData, std::vector<TimerData>, > timerExpiryQueue;
	int uniqueTimerId = 0;
};


static ITimer *timer = nullptr;



// Key is
// 1. One timer service, hence use singleton pattern
// 2. Timer service can be implemeted using a map or priority_queue
// 3. Running a separate thread for sleeping the closest expiry timer
