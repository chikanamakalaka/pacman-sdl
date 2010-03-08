/*
 * KeyboardView.hpp
 *
 *  Created on: Jun 1, 2009
 *      Author: asantos
 */

#ifndef TETRISINPUTVIEW_HPP_
#define TETRISINPUTVIEW_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include "SignalSubscriber.hpp"


//Turns keyup keydown SDL events into semantically rich, domain specific events
//EG: z-key down -> move forward
class TetrisInputView: public SignalSubscriber{
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

	std::map<Uint8, boost::function<void()> > keydownbindings;
	std::map<Uint8, boost::function<void()> > keyupbindings;
	std::map<Uint8, boost::function<void(long, long)> > keyheldbindings;
	std::map<Uint8, bool> keys;
public:
	TetrisInputView(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Tetris", "TetrisInputView"),
		signalbroker(signalbroker){
		try{
			SignalSubscriber::ConnectToSignal
			<SDLEventsView::KeyDownHandler>
			(	"/sdlevent/keydown",
				boost::bind(&TetrisInputView::KeyDown, this, _1));

			SignalSubscriber::ConnectToSignal
			<SDLEventsView::KeyUpHandler>
			(	"/sdlevent/keyup",
				boost::bind(&TetrisInputView::KeyUp, this, _1));

			signalbroker.ConnectToSignal
			<ClockView::TickHandler>
			(	"/clock/tick",
				boost::bind(&TetrisInputView::ApplyInput, this, _1, _2));


			signalbroker.ConnectToSignal
			<TetrisInputView::RefreshKeyBindingsHandler>
			(	"/tetrisinput/refreshkeybindings",
				boost::bind(&TetrisInputView::RefreshKeyBindings, this));


			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/moveleft");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/moveright");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/movedown");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/movenormal");
			signalbroker.EnsureSignal<PlayerMovementHandler>("/input/player/rotate");

			RefreshKeyBindings();

			keyheldbindings[SDLK_ESCAPE] = boost::bind(&TetrisInputView::Quit, this, _1, _2);
		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string(e.what()));
		}
	}
	void RefreshKeyBindings(){
		//setup bindings
		TetrisDB tetrisdb;
		boost::bimap<int,std::string> keybindings = tetrisdb.GetKeyBindings();
		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Left");
			keydownbindings[itr->second] = boost::bind(&TetrisInputView::PlayerMoveLeft, this);
		}
		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Right");
			keydownbindings[itr->second] = boost::bind(&TetrisInputView::PlayerMoveRight, this);
		}

		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Rotate");
			keydownbindings[itr->second] = boost::bind(&TetrisInputView::PlayerRotate, this);
		}

		{
			boost::bimap<int,std::string>::right_const_iterator itr = keybindings.right.find("Down");
			keydownbindings[itr->second] = boost::bind(&TetrisInputView::PlayerMoveDown, this);
			keyupbindings[itr->second] = boost::bind(&TetrisInputView::PlayerMoveNormal, this);
		}



	}
	void KeyUp(SDL_Event event){
		SDLInput sdlinput;
		int gcnvalue = sdlinput.convertSDLEventToGuichanKeyValue(event);
		if (gcnvalue == -1)
		{
			gcnvalue = (int)event.key.keysym.unicode ? (int)event.key.keysym.unicode : (int)event.key.keysym.sym;
		}


		if(keyupbindings.find(gcnvalue)!=keyupbindings.end()){
			keyupbindings[gcnvalue]();
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

		if(keydownbindings.find(gcnvalue)!=keydownbindings.end()){
			keydownbindings[gcnvalue]();
		}
		keys[gcnvalue] = true;
	}

	void ApplyInput(long t, long dt){
		std::map<Uint8, bool>::const_iterator itr = keys.begin();
		for(;itr != keys.end(); itr++){
			if(itr->second == true){
				if(keyheldbindings.find(itr->first) != keyheldbindings.end()){
					keyheldbindings.find(itr->first)->second(t, dt);
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
		signalbroker.InvokeSignal<GamestateController::StateChangeHandler>("/tetrisgamestatecontroller/quit");
	}
};


#endif /* TETRISINPUTVIEW_HPP_ */
