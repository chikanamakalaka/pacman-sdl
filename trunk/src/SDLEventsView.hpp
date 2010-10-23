/*
 * SDLEventsView.hpp
 *
 *  Created on: May 1, 2009
 *      Author: asantos
 */

class SDLEventsView{
public:
	typedef void(KeyDownHandler)(SDL_Event);
	typedef void(KeyUpHandler)(SDL_Event);
	typedef void(SDLEventHandler)(SDL_Event);
private:
	SignalBroker& signalbroker;
public:
	SDLEventsView(SignalBroker& signalbroker):signalbroker(signalbroker){
		try{
			signalbroker.ConnectToSignal
			<ClockView::TickHandler>(
			"/clock/tick",
			boost::bind(&SDLEventsView::PollEvents, this, _1, _2));

			signalbroker.EnsureSignal<KeyDownHandler>("/sdlevent/keydown");
			signalbroker.EnsureSignal<KeyUpHandler>("/sdlevent/keyup");

			SDL_Init(SDL_INIT_EVERYTHING);


		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string(e.what()));
		}
	}
	~SDLEventsView(){
		SDL_Quit();
	}
	void PollEvents(long t, long dt){
		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_PollEvent");
		SDL_Event event = {};
		while ( SDL_PollEvent(&event) ) {
			//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Got SDL Event");
			signalbroker.InvokeSignal<SDLEventHandler>("/sdlevent/event", event);
			switch (event.type) {
				case SDL_ACTIVEEVENT:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_ACTIVEEVENT");
					break;
				case SDL_VIDEOEXPOSE:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_VIDEOEXPOSE");
					break;
				case SDL_SYSWMEVENT:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_SYSWMEVENT");
					break;
				case SDL_VIDEORESIZE:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_VIDEORESIZE");
					break;
				case SDL_MOUSEMOTION:
					{
					/*std::stringstream ss;
					ss << "Mouse moved by " << event.motion.xrel << ","
						<< event.motion.yrel << " to (" << event.motion.x
						<< "," << event.motion.y << ")";
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());
					*/
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
					std::stringstream ss;
					ss << "Mouse button " << event.button.button << " pressed at "
						<< "(" << event.button.x << "," << event.button.y << ")";
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());
					}
					break;
				case SDL_MOUSEBUTTONUP:
					{
					std::stringstream ss;
					ss << "Mouse button " << event.button.button << " unpressed at "
						<< "(" << event.button.x << "," << event.button.y << ")";
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());
					}
					break;
				case SDL_KEYDOWN:
					signalbroker.InvokeSignal<KeyDownHandler>("/sdlevent/keydown", event);
					break;
				case SDL_KEYUP:
					signalbroker.InvokeSignal<KeyUpHandler>("/sdlevent/keyup", event);
					break;
				case SDL_QUIT:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SDL_QUIT");
					signalbroker.InvokeSignal
						<ClockView::StopClock>(	"/clock/stop");
					break;
				default:
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Unhandled SDL event");
					{
						std::stringstream ss;
						ss << "event.type:" << event.type;
						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());
					}
			}
		}
	}
};
