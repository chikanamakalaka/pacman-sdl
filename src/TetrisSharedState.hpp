/*
 * TetrisSharedState.hpp
 *
 *  Created on: Oct 22, 2009
 *      Author: asantos
 */

#ifndef TETRISSHAREDSTATE_HPP_
#define TETRISSHAREDSTATE_HPP_

#include "SignalBroker.hpp"

class TetrisSharedState{
public:
	typedef void(GetTetrisSharedStateHandler)(TetrisSharedState*);
private:
	SignalBroker& signalbroker;
	int highscore;
public:
	TetrisSharedState(SignalBroker& signalbroker):signalbroker(signalbroker), highscore(0){
		signalbroker.ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&TetrisSharedState::GetTetrisSharedState, this, _1, _2));
	}

	void GetTetrisSharedState(long t, long dt){
		signalbroker.InvokeSignal<GetTetrisSharedStateHandler>("/tetrissharedstate/get", this);
	}

	int GetHighScore()const{
		return highscore;
	}
	void SetHighScore(int highscore){
		this->highscore = highscore;
	}

};

#endif /* TETRISSHAREDSTATE_HPP_ */
