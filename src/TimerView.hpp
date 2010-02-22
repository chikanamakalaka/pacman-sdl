/*
 * ClockView.hpp
 *
 *  Created on: Apr 29, 2009
 *      Author: asantos
 */
class TimeoutAlreadyExists : public virtual std::exception{
private:
		const char* msg;
public:
	TimeoutAlreadyExists(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};
class IntervalAlreadyExists : public virtual std::exception{
private:
		const char* msg;
public:
	IntervalAlreadyExists(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};

class TimerView{
public:
	typedef void(TimerHandler)(long,long);
private:
	SignalBroker& signalbroker;
	long lastclocktick;
public:
	class Timeout{
	public:
		typedef boost::function<void(long,long)> Callback;
	private:
		Callback callback;
		long timeout;
		long lastinvoked;
		bool paused;
	public:
		Timeout(Callback callback, long timeout, long lastclocktick):callback(callback), timeout(timeout), lastinvoked(lastclocktick), paused(false){}
		long GetTimeout()const {
			return timeout;
		}
		void SetTimeout(long timeout){
			this->timeout = timeout;
		}
		void Pause(){
			paused=true;
		}
		void Unpause(){
			paused=false;
		}
		bool InvokeIfPastDue(long t, long dt){
			if(t>lastinvoked+timeout && !paused){
				callback(t, dt);
				lastinvoked = t;
				return true;
			}
			return false;
		}
	};
	class Interval{
	public:
		typedef boost::function<void(long,long)> Callback;
	private:
		Callback callback;
		long interval;
		long lastinvoked;
		bool paused;
		public:
		Interval(Callback callback, long interval, long lastinvoked):callback(callback), interval(interval), lastinvoked(lastinvoked),paused(false){}
		long GetInterval()const {
			return interval;
		}
		void SetInterval(long interval){
			this->interval = interval;
		}
		void Pause(){
			paused=true;
		}
		void Unpause(){
			paused=false;
		}
		bool InvokeIfPastDue(long t, long dt){
			if(t>lastinvoked+interval && !paused){
				callback(t, dt);
				lastinvoked = t;
				return true;
			}
			return false;
		}
	};

	typedef void(TimeoutHandler)(const std::string&, Timeout::Callback, long);
	typedef void(IntervalHandler)(const std::string&, Interval::Callback, long);

	typedef void(RemoveTimeoutHandler)(const std::string&);
	typedef void(RemoveIntervalHandler)(const std::string&);

	typedef void(UpdateTimeoutLengthHandler)(const std::string&, long);
	typedef void(UpdateIntervalLengthHandler)(const std::string&, long);


	typedef void(PauseTimeoutHandler)(const std::string&);
	typedef void(PauseIntervalHandler)(const std::string&);

	typedef void(UnpauseTimeoutHandler)(const std::string&);
	typedef void(UnpauseIntervalHandler)(const std::string&);

	typedef std::map<std::string, Timeout> Timeouts;
	typedef std::map<std::string, Interval> Intervals;
private:
	Timeouts timeouts;
	Intervals intervals;
public:
	TimerView(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		lastclocktick(0){

		signalbroker.ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&TimerView::OnTick, this, _1, _2));

		signalbroker.ConnectToSignal
		<TimeoutHandler>
		(	"/timer/settimeout",
			boost::bind(&TimerView::SetTimeout, this, _1, _2, _3));


		signalbroker.ConnectToSignal
		<IntervalHandler>
		(	"/timer/setinterval",
			boost::bind(&TimerView::SetInterval, this, _1, _2, _3));


		signalbroker.ConnectToSignal
		<RemoveTimeoutHandler>
		(	"/timer/removetimeout",
			boost::bind(&TimerView::RemoveTimeout, this, _1));


		signalbroker.ConnectToSignal
		<RemoveIntervalHandler>
		(	"/timer/removeinterval",
			boost::bind(&TimerView::RemoveInterval, this, _1));


		signalbroker.ConnectToSignal
		<UpdateTimeoutLengthHandler>
		(	"/timer/updatetimeout",
			boost::bind(&TimerView::UpdateTimeoutLength, this, _1, _2));


		signalbroker.ConnectToSignal
		<UpdateIntervalLengthHandler>
		(	"/timer/updateinterval",
			boost::bind(&TimerView::UpdateIntervalLength, this, _1, _2));


		signalbroker.ConnectToSignal
		<PauseTimeoutHandler>
		(	"/timer/pausetimeout",
			boost::bind(&TimerView::PauseTimeout, this, _1));


		signalbroker.ConnectToSignal
		<PauseIntervalHandler>
		(	"/timer/pauseinterval",
			boost::bind(&TimerView::PauseInterval, this, _1));


		signalbroker.ConnectToSignal
		<UnpauseTimeoutHandler>
		(	"/timer/unpausetimeout",
			boost::bind(&TimerView::UnpauseTimeout, this, _1));


		signalbroker.ConnectToSignal
		<UnpauseIntervalHandler>
		(	"/timer/unpauseinterval",
			boost::bind(&TimerView::UnpauseInterval, this, _1));


	}
	void OnTick(long t, long dt){
		{
			Timeouts::iterator itr = timeouts.begin();
			for(;itr!=timeouts.end();itr++){
				if(itr->second.InvokeIfPastDue(t, dt)){
					timeouts.erase(itr);
				}
			}
		}
		{
			Intervals::iterator itr = intervals.begin();
			for(;itr!=intervals.end();itr++){
				itr->second.InvokeIfPastDue(t, dt);
			}
		}
		lastclocktick = dt;
	}
	void SetTimeout(const std::string& name, Timeout::Callback callback, long timeout){
		if(timeouts.find(name)==timeouts.end()){
			timeouts.insert(Timeouts::value_type(name, Timeout(callback, timeout, lastclocktick)));
		}else{
			throw TimeoutAlreadyExists(name.c_str());
		}
	}
	void SetInterval(const std::string& name, Interval::Callback callback, long interval){
		if(intervals.find(name)==intervals.end()){
			intervals.insert(Intervals::value_type(name, Interval(callback, interval, lastclocktick)));
		}else{
			throw IntervalAlreadyExists(name.c_str());
		}
	}
	void RemoveTimeout(const std::string& name){
		timeouts.erase(name);
	}
	void RemoveInterval(const std::string& name){
		intervals.erase(name);
	}
	void UpdateTimeoutLength(const std::string& name, long timeout){
		if(timeouts.find(name)!=timeouts.end()){
			timeouts.find(name)->second.SetTimeout(timeout);
		}
	}
	void UpdateIntervalLength(const std::string& name, long interval){
		if(intervals.find(name)!=intervals.end()){
			intervals.find(name)->second.SetInterval(interval);
		}
	}

	void PauseTimeout(const std::string& name){
		if(timeouts.find(name)!=timeouts.end()){
			timeouts.find(name)->second.Pause();
		}
	}
	void PauseInterval(const std::string& name){
		if(intervals.find(name)!=intervals.end()){
			intervals.find(name)->second.Pause();
		}
	}

	void UnpauseTimeout(const std::string& name){
		if(timeouts.find(name)!=timeouts.end()){
			timeouts.find(name)->second.Unpause();
		}
	}
	void UnpauseInterval(const std::string& name){
		if(intervals.find(name)!=intervals.end()){
			intervals.find(name)->second.Unpause();
		}
	}

};
