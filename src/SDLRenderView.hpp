class SDLRenderView: public SignalSubscriber{
public:
	typedef void(Initialized)();
private:
	SignalBroker& signalbroker;
	boost::signals::connection connection;

public:
	SDLRenderView(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "", "SDLRenderView"),
		signalbroker(signalbroker){
		try{
			connection = SignalSubscriber::ConnectToSignal
			<ClockView::TickHandler>
			(	"/clock/tick",
				boost::bind(&SDLRenderView::Initialize, this));
		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string("Caught EventDoesNotExist ") + e.what());
		}
	}
	void Initialize(){
		try{
			if ( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0 ) {
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Unable to init SDL");
				signalbroker.InvokeSignal<ClockView::StopClock>("/clock/stop");
			}else{
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initialized SDL successfully");


				SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL);
				if(screen == NULL){
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Unable to set 640x480 video: " );
					signalbroker.InvokeSignal<ClockView::StopClock>("/clock/stop");
				}
				else{
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Able to set 640x480 video successfully");

				}
			}
			//disconnect when done. don't want to initialize every tick
			connection.disconnect();
			signalbroker.InvokeSignal<Initialized>("/render/sdlinitialized");
		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string("Caught EventDoesNotExist ") + e.what());
		}
	}

};



