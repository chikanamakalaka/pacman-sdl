//============================================================================
// Name        : main.cpp
// Author      : Aaron
// Version     :
// Copyright   :
// Description : Pacman
//============================================================================

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <iterator>
#include <exception>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/signals.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/program_options.hpp>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "math.hpp"
#include "FileSystem.hpp"
#include "SignalBroker.hpp"
#include "SceneNode.h"
#include "SceneGraph.h"
#include "OutputStreamView.hpp"
#include "ClockView.hpp"
#include "TimerView.hpp"
#include "OpenGL/util.hpp"
#include "TetrisDB.hpp"
#include "SceneNodeProperties.hpp"
#include "SceneGraphController.hpp"
#include "GamestateController.hpp"
#include "AnimationController.hpp"
#include "SDLEventsView.hpp"
#include "TetrisInputView.hpp"
#include "SDLRenderView.hpp"
#include "OpenGLRenderView.hpp"
#include "GuiChanRenderView.hpp"
#include "TetrisEnterNameGUIController.hpp"
#include "TetrisLogic.hpp"
#include "TetrisHighScoresGUIController.hpp"
#include "PacmanGamestateLoader.hpp"
#include "SDLTetrisAudio.hpp"
#include "TetrisMainMenuController.hpp"
#include "TetrisConfigurationMenuController.hpp"
#include "TetrisGameGUIController.hpp"
#include "TetrisCreditsGUIController.hpp"



int main(int argc, char *argv[]) try{
	// Declare the supported options.
	boost::program_options::options_description desc(
			"Usage: pileofblocks [OPTION...]\n"
	);
	desc.add_options()
	    ("help", "help message")
	    ("version,V", "version message")
	    ("log,L", "enable log messages")
	;

	boost::program_options::variables_map vm;
	try{
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	}catch(boost::program_options::error e){
		std::cout<<e.what()<<std::endl;
		return 1;
	}

	if (vm.count("help") || vm.count("h")) {
	    std::cout << desc << std::endl;
	    return 1;
	}else if(vm.count("version") || vm.count("V")){
		std::cout<<"pileofblocks - Version 1.0.0"<<std::endl;
		return 1;
	}
	bool log = false;
	if (vm.count("log") || vm.count("L")) {
		log = true;
	}

	SignalBroker signalbroker;
	//application agnostic
	ClockView clockview(signalbroker);
	TimerView timerview(signalbroker);
	SDLEventsView sdleventsview(signalbroker);
	OutputStreamView errorview(signalbroker, std::cout, log);
	SDLRenderView renderview(signalbroker);
	SceneGraphController scenegraphcontroller(signalbroker);
	GamestateController gamestatecontroller(signalbroker);
	OpenGLRenderView openglrenderview(signalbroker);


	//application specific
	TetrisInputView tetrisinputview(signalbroker);
	//TetrisLogic tetrislogic(signalbroker);
	//SDLTetrisAudio tetrisaudio(signalbroker);
	PacmanGamestateLoader pacmangamestateloader(signalbroker);
	TetrisMainMenuController tetrismainmenucontroller(signalbroker);
	TetrisConfigurationMenuController tetrisconfigurationmenucontroller(signalbroker);
	TetrisGameGUIController tetrisgameguicontroller(signalbroker);
	TetrisEnterNameGUIController tetrisenternameguicontroller(signalbroker);
	TetrisHighScoresGUIController tetrishighscoresguicontroller(signalbroker);
	TetrisCreditsGUIController tetriscreditsguicontroller(signalbroker);
	clockview.Start();

    return 0;
}catch(std::exception& e){
	std::cout << "Error occured. Caught in main(). " << e.what() << std::endl;
	return 0;
}

