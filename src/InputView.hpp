/*
 * InputView.hpp
 *
 *  Created on: Jun 1, 2009
 *      Author: asantos
 */

#ifndef INPUTVIEW_HPP_
#define INPUTVIEW_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include "SignalSubscriber.hpp"


//Turns keyup keydown SDL events into semantically rich, domain specific events
//EG: z-key down -> move forward
class InputView: public SignalSubscriber{
public:
	typedef void(PlayerMovementHandler)();
	typedef void(RefreshKeyBindingsHandler)();
private:
	class SDLInput:public gcn::SDLInput{
	public:
		int convertSDLEventToGuichanKeyValue(SDL_Event event){
			return gcn::SDLInput::convertSDLEventToGuichanKeyValue(event);
		}
	};
	SignalBroker& signalbroker;
	boost::bimap<int,std::string> keybindings;
	std::multimap<std::string, std::string> keydownbindings;
	std::multimap<std::string, std::string> keyupbindings;
	std::multimap<std::string, std::string> keyheldbindings;
	std::map<Uint8, bool> keys;
public:
	InputView(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Pacman", "InputView"),
		signalbroker(signalbroker){
		try{
			SignalSubscriber::ConnectToSignal
			<SDLEventsView::KeyDownHandler>
			(	"/sdlevent/keydown",
				boost::bind(&InputView::KeyDown, this, _1));

			SignalSubscriber::ConnectToSignal
			<SDLEventsView::KeyUpHandler>
			(	"/sdlevent/keyup",
				boost::bind(&InputView::KeyUp, this, _1));

			signalbroker.ConnectToSignal
			<ClockView::TickHandler>
			(	"/clock/tick",
				boost::bind(&InputView::ApplyInput, this, _1, _2));


			signalbroker.ConnectToSignal
			<InputView::RefreshKeyBindingsHandler>
			(	"/pacmaninput/refreshkeybindings",
				boost::bind(&InputView::RefreshKeyBindings, this));


			/*signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/moveleft");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/moveright");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/movedown");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/movenormal");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/rotate");*/

			RefreshKeyBindings();

			//keyheldbindings[SDLK_ESCAPE] = boost::bind(&InputView::Quit, this, _1, _2);
		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string(e.what()));
		}
	}
	void RefreshKeyBindings(){
		//setup bindings
		PacmanDB pacmandb;
		keybindings = pacmandb.GetKeyBindings();
		keyupbindings = pacmandb.GetKeyUpBindings();
		keydownbindings = pacmandb.GetKeyDownBindings();
		keyheldbindings = pacmandb.GetKeyHeldBindings();

		/*boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Left");
		{
			keydownbindings[itr->second] = boost::bind(&InputView::PlayerMoveLeft, this);
		}
		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Right");
			keydownbindings[itr->second] = boost::bind(&InputView::PlayerMoveRight, this);
		}

		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Rotate");
			keydownbindings[itr->second] = boost::bind(&InputView::PlayerRotate, this);
		}

		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Down");
			keydownbindings[itr->second] = boost::bind(&InputView::PlayerMoveDown, this);
			keyupbindings[itr->second] = boost::bind(&InputView::PlayerMoveNormal, this);
		}
		*/


	}
	void KeyUp(SDL_Event event){
		SDLInput sdlinput;
		int gcnvalue = sdlinput.convertSDLEventToGuichanKeyValue(event);
		if (gcnvalue == -1)
		{
			gcnvalue = (int)event.key.keysym.unicode ? (int)event.key.keysym.unicode : (int)event.key.keysym.sym;
		}

		boost::bimap<int,std::string>::left_map::const_iterator itr = keybindings.left.find(gcnvalue);
		if(itr != keybindings.left.end()){
			std::multimap<std::string, std::string>::const_iterator keyupitr = keyupbindings.find(itr->second);
			if(keyupitr != keyupbindings.end()){
				for(; keyupitr != keyupbindings.upper_bound(itr->second); keyupitr++){
					signalbroker.InvokeSignal<SignalBroker::GenericHandler>(keyupitr->second, ArgsList());
				}
			}
		}
		keys[gcnvalue] = false;

	}
	void KeyDown(SDL_Event event){
		SDLInput sdlinput;
		int gcnvalue = sdlinput.convertSDLEventToGuichanKeyValue(event);
		if (gcnvalue == -1)
		{
			gcnvalue = (int)event.key.keysym.unicode ? (int)event.key.keysym.unicode : (int)event.key.keysym.sym;
		}

		boost::bimap<int,std::string>::left_map::const_iterator itr = keybindings.left.find(gcnvalue);
		if(itr != keybindings.left.end()){
			std::multimap<std::string, std::string>::const_iterator keydownitr = keydownbindings.find(itr->second);
			if(keydownitr != keydownbindings.end()){
				for(; keydownitr != keydownbindings.upper_bound(itr->second); keydownitr++){
					signalbroker.InvokeSignal<SignalBroker::GenericHandler>(keydownitr->second, ArgsList());
				}
			}
		}
		keys[gcnvalue] = true;
	}

	void ApplyInput(long t, long dt){
		std::map<Uint8, bool>::const_iterator itr = keys.begin();
		for(;itr != keys.end(); itr++){
			if(itr->second == true){
				boost::bimap<int, std::string>::left_map::const_iterator itr = keybindings.left.find(itr->first);
				if(itr != keybindings.left.end()){
					std::multimap<std::string, std::string>::const_iterator keyhelditr = keyheldbindings.find(itr->second);
					if(keyhelditr != keyheldbindings.end()){
						for(; keyhelditr != keyheldbindings.upper_bound(itr->second); keyhelditr++){
							signalbroker.InvokeSignal<SignalBroker::GenericHandler>(keyhelditr->second, (ArgsList(), t, dt));
						}
					}
				}
			}
		}
	}

	void PlayerMoveLeft(){
		signalbroker.InvokeSignal<PlayerMovementHandler>("/input/player/moveleft");
	}
	void PlayerMoveRight(){
		signalbroker.InvokeSignal<PlayerMovementHandler>("/input/player/moveright");
	}
	void PlayerMoveDown(){
		signalbroker.InvokeSignal<PlayerMovementHandler>("/input/player/movedown");
	}
	void PlayerMoveNormal(){
		signalbroker.InvokeSignal<PlayerMovementHandler>("/input/player/movenormal");
	}
	void PlayerRotate(){
		signalbroker.InvokeSignal<PlayerMovementHandler>("/input/player/rotate");
	}
	void Quit(long t, long dt){
		signalbroker.InvokeSignal<GamestateController::StateChangeHandler>("/pacmangamestatecontroller/quit");
	}
};


#endif /* PACMANINPUTVIEW_HPP_ */
