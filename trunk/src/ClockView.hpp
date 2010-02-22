/*
 * ClockView.hpp
 *
 *  Created on: Apr 29, 2009
 *      Author: asantos
 */


class ClockView{
public:
	typedef void(TickHandler)(long,long);
private:
	SignalBroker& signalbroker;
public:
	ClockView(SignalBroker& signalbroker, unsigned int milliseconds = 30):
		signalbroker(signalbroker)/*,
		signalbroker.EnsureSignal<TickHandler>("/clock/tick"))*/{

	}
	void Start(){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Starting clock.");
		long t = SDL_GetTicks();
		long dt = 0;
		while(true){
			dt=SDL_GetTicks()-t;
			t+=dt;
			signalbroker.InvokeSignal<TickHandler>("/clock/tick", t, dt);
		}
	}
};
