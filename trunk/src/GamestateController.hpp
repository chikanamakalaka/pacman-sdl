/*
 * GamestateController.hpp
 *
 *  Created on: Aug 11, 2009
 *      Author: asantos
 */

#ifndef GAMESTATECONTROLLER_HPP_
#define GAMESTATECONTROLLER_HPP_

class Gamestate{
protected:
	SignalBroker& signalbroker;
	const std::string name;
public:
	Gamestate(const std::string& name, SignalBroker& signalbroker):signalbroker(signalbroker), name(name){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "GameState::Unblocking " + name);
		signalbroker.Unblock(name);

		signalbroker.InvokeSignal
				<SceneGraphController::SelectSceneGraphHandler>
				(	"/scenegraphcontroller/selectscenegraph",
					name);
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "selected scenegraph " + name);

	}
	virtual ~Gamestate(){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "GameState::Blocking " + name);
		signalbroker.Block(name);
	}
};

class GamestateAlreadyExists : public virtual std::exception{
public:
	GamestateAlreadyExists(const char* msg){

	}
	char const* what()const throw(){
		return "GamestateAlreadyExists";
	}
};
class GamestateDoesNotExist : public virtual std::exception{
private:
		const char* msg;
public:
		GamestateDoesNotExist(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};

class GamestateController{
public:
	typedef void(StateChangeHandler)();
	typedef void(GetGamestateControllerHandler)(GamestateController&);
	typedef void(GetCurrentGamestateHandler)(Gamestate);
private:
	SignalBroker& signalbroker;
	Gamestate* currentgamestate;
	Signal<GetGamestateControllerHandler>& getgamestatecontrollersig;
	Signal<GetCurrentGamestateHandler>& getcurrentgamestatesig;
public:
	GamestateController(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		currentgamestate(0),
		getgamestatecontrollersig(signalbroker.EnsureSignal<GetGamestateControllerHandler>("/gamestatecontroller/get")),
		getcurrentgamestatesig(signalbroker.EnsureSignal<GetCurrentGamestateHandler>("/gamestatecontroller/getcurrentgamestate"))
	{
		signalbroker.ConnectToSignal<ClockView::TickHandler>("/clock/tick",
				boost::bind(&GamestateController::GetCurrentGamestate, this, _1, _2));
		signalbroker.ConnectToSignal<ClockView::TickHandler>("/clock/tick",
				boost::bind(&GamestateController::GetGamestateController, this, _1, _2));

	}
	void GetGamestateController(long t, long dt){
		getgamestatecontrollersig(*this);
	}
	void GetCurrentGamestate(long t, long dt){
		if(currentgamestate){
			getcurrentgamestatesig(*currentgamestate);
		}
	}
};


#endif /* GAMESTATECONTROLLER_HPP_ */
