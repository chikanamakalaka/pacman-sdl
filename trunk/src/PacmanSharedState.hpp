/*
 * PacmanSharedState.hpp
 *
 *  Created on: Oct 22, 2009
 *      Author: asantos
 */

#ifndef PACMANSHAREDSTATE_HPP_
#define PACMANSHAREDSTATE_HPP_

#include "SignalBroker.hpp"

class PacmanSharedState{
public:
	typedef void(GetPacmanSharedStateHandler)(PacmanSharedState*);
private:
	SignalBroker& signalbroker;
	int highscore;
public:
	PacmanSharedState(SignalBroker& signalbroker):signalbroker(signalbroker), highscore(0){
		signalbroker.ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&PacmanSharedState::GetPacmanSharedState, this, _1, _2));
	}

	void GetPacmanSharedState(long t, long dt){
		signalbroker.InvokeSignal<GetPacmanSharedStateHandler>("/pacmansharedstate/get", this);
	}

	int GetHighScore()const{
		return highscore;
	}
	void SetHighScore(int highscore){
		this->highscore = highscore;
	}

};

#endif /* PACMANSHAREDSTATE_HPP_ */
