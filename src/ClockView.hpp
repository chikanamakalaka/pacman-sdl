/*
 * ClockView.hpp
 *
 *  Created on: Apr 29, 2009
 *      Author: asantos
 */


class ClockView{
public:
	typedef void(TickHandler)(long,long);
	typedef void(StopClock)();
private:
	SignalBroker& signalbroker;
	bool running;
public:
	ClockView(SignalBroker& signalbroker, unsigned int milliseconds = 30):
		signalbroker(signalbroker),
		running(true){

		signalbroker.ConnectToSignal
				<StopClock>
				(	"/clock/stop",
					boost::bind(&ClockView::Stop, this));
	}
	void Start(){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Starting clock.");
		long t = SDL_GetTicks();
		long dt = 0;
		while(running){
			dt=SDL_GetTicks()-t;
			t+=dt;
			signalbroker.InvokeSignal<TickHandler>("/clock/tick", t, dt);
		}
	}
	void Stop(){
		running = false;
	}
};
