/*
 * PacmansLogic.hpp
 *
 *  Created on: Jun 4, 2009
 *      Author: asantos
 */

#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>

#ifndef PACMANLOGIC_HPP_
#define PACMANLOGIC_HPP_

class PacmanLogic:public SignalSubscriber{
public:
	typedef void(ScoreChangedHandler)(int);
	typedef void(LevelChangedHandler)(int);
	typedef void(LoadInitialStateHandler)();
	typedef void(UnableToMoveHandler)();
	typedef void(GameOverHandler)();
	typedef void(UnableToRotateHandler)();
	typedef void(AbleToRotateHandler)();
	typedef void(AbleToMoveHandler)();
	typedef void(CurrentPieceStoppedHandler)();

	typedef boost::shared_ptr<SceneNode> SceneNodePtr;

private:
	SignalBroker& signalbroker;
	boost::signals::connection getselectedscenegraphconnection;
	boost::signals::connection getpacmansharedstateconnection;

	bool levelloaded;
	int score;
	int level;

	SceneNodePtr pacman;
	SceneNodePtr blinky;
	SceneNodePtr inky;
	SceneNodePtr pinky;
	SceneNodePtr clyde;

	std::vector<SceneNodePtr> dots;
	std::vector<SceneNodePtr> powerups;

	std::vector<std::vector<bool> > collisionmap;
	boost::mt19937 rng;
	boost::uniform_int<> seven;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die;


public:
	PacmanLogic(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Pacman", "PacmanLogic"),
		signalbroker(signalbroker),
		score(0),
		level(0),
		seven(0,6),
		die(rng, seven)
{
		rng.seed(time(0));

		signalbroker.ConnectToSignal
		<PacmanGamestateLoader::SceneGraphLoadedFromFileHandler>
		(	"/pacmangamestateloader/scenegraphloadedfromfile",
				boost::bind(&PacmanLogic::SceneGraphLoadedFromFile, this, _1));


		SignalSubscriber::ConnectToSignal
		<PacmanLogic::LoadInitialStateHandler>
		(	"/logic/loadinitialstate",
			boost::bind(&PacmanLogic::LoadInitialState, this));

		SignalSubscriber::ConnectToSignal<InputView::PlayerMovementHandler>
		(	"/input/player/movedown",
			boost::bind(&PacmanLogic::MovePlayerDown, this));


		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/moveup",
			boost::bind(&PacmanLogic::MovePlayerUp, this));

		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/moveleft",
			boost::bind(&PacmanLogic::MovePlayerLeft, this));

		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/moveright",
				boost::bind(&PacmanLogic::MovePlayerRight, this));

		LoadLevel(1);

	}
	void LoadLevel(int level){
		std::string filename;
		{
			std::stringstream ss;
			ss<<"level"<<level<<".xml";
			filename = ss.str();
		}

		std::string levelname;
		{
			std::stringstream ss;
			ss<<"level"<<level;
			levelname = ss.str();
		}

		signalbroker.InvokeSignal<PacmanGamestateLoader::LoadSceneGraphWithNameFromFileHandler>(
			"/pacmangamestateloader/loadscenegraphwithnamefromfile",
			filename, levelname);


	}
	void SceneGraphLoadedFromFile(boost::shared_ptr<SceneGraph> scenegraphptr){
		//TODO:capture characters
		pacman = scenegraphptr->GetNodePtrByPath("/root/characters/pacman");
		blinky = scenegraphptr->GetNodePtrByPath("/root/characters/blinky");
		inky = scenegraphptr->GetNodePtrByPath("/root/characters/inky");
		pinky = scenegraphptr->GetNodePtrByPath("/root/characters/pinky");
		clyde = scenegraphptr->GetNodePtrByPath("/root/characters/clyde");

		//TODO:capture collision grid
		int maxx = 0;
		int maxy = 0;


		//make a blank new collision map
		collisionmap.resize(20);
		for(int i=0; i<20; i++){
			collisionmap[i].resize(10);
			for(int j=0; j<10; j++){
				collisionmap[i][j] = false;
			}
		}
		//TODO:capture fruit
		//TODO:capture dots
		//TODO:capture powerups
		//TODO:capture eyes

			//Process logic at appropriate interval
			signalbroker.InvokeSignal<TimerView::IntervalHandler>(
					"/timer/setinterval",
					"/pacmanlogic/processlogic", boost::bind(&PacmanLogic::ProcessLogic, this, _1, _2), IntervalPerLevel(level));

	}

	void LoadInitialState(){
		//make a blank new collision map
		collisionmap.resize(20);
		for(int i=0; i<20; i++){
			collisionmap[i].resize(10);
			for(int j=0; j<10; j++){
				collisionmap[i][j] = false;
			}
		}

		score = 0;
		level = 0;

		signalbroker.InvokeSignal<PacmanLogic::ScoreChangedHandler>("/pacman/scorechanged", score);
		signalbroker.InvokeSignal<PacmanLogic::LevelChangedHandler>("/pacman/levelchanged", level);

		//reset update speed
		signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level));
		
		//stop process logic interval
		signalbroker.InvokeSignal
			<TimerView::UnpauseIntervalHandler>
			("/timer/unpauseinterval", "/pacmanlogic/processlogic");
		//SetUpNextPeice();
	}

	void MovePlayerDown(){
		if(levelloaded){
			signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level+6));
		}
	}
	void MovePlayerUp(){
		if(levelloaded){
			signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level));
		}
	}

	void MovePlayerLeft(){
		if(levelloaded){

		}
	}
	void MovePlayerRight(){
		if(levelloaded){

		}
	}
protected:
	void ProcessLogic(long t, long dt){

		//TODO:Run Logic for ghosts
		//TODO:MOAR?

		//TODO: Check for endgame
		//current piece hit bottom?
		if(false/*endgame*/){
			signalbroker.InvokeSignal
			<PacmanLogic::CurrentPieceStoppedHandler>("/pacmanlogic/currentpiecestopped");

			if(false/*lose?*/)
				//lose condition, next piece immediately stopped :(
				signalbroker.InvokeSignal
				<PacmanLogic::GameOverHandler>
				("/pacmanlogic/gameover");

				//stop updating logic, we're exiting to highscores or enter name menu
				signalbroker.InvokeSignal
					<TimerView::PauseIntervalHandler>
					("/timer/pauseinterval", "/pacmanlogic/processlogic");

			//check for high score
			PacmanDB pacmandb;
			if(pacmandb.QualifiesForHighScore(score)){
				signalbroker.InvokeSignal
					<PacmanEnterNameGUIController::RefreshSharedScoreHandler>
					("/pacmanenternamegui/refreshsharedscore", score);
				//If has high score
				//enter name
				signalbroker.InvokeSignal
					<GamestateController::StateChangeHandler>
					("/pacmangamestatecontroller/endgame");

			}else{
				//else quit and go to high scores list
				signalbroker.InvokeSignal
					<GamestateController::StateChangeHandler>
					("/pacmangamestatecontroller/quit");

				signalbroker.InvokeSignal
					<GamestateController::StateChangeHandler>
					("/pacmangamestatecontroller/enterhighscores");
			}

			return;

		}
		//reset update speed
		signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level));
	}

	bool CanMoveToPosition(int x, int y)const{
		
		return true;
	}
	void IncrementScore(int rowscleared){
		if(rowscleared>0){
			score+=rowscleared*std::pow(1.5, level)*100;
			signalbroker.InvokeSignal<PacmanLogic::ScoreChangedHandler>("/pacman/scorechanged", score);
			int newlevel = score/1000;
			if(newlevel > level){
				level = newlevel;
				signalbroker.InvokeSignal<PacmanLogic::LevelChangedHandler>("/pacman/levelchanged", level);
			}
			
		}
	}

	void HideNodeAndDescendants(SceneNodePtr scenenode){
		if(scenenode->HasSceneNodeProperty("renderable")){
			scenenode->GetSceneNodeProperty<RenderableProperty>("renderable").GetRenderable().SetVisibility(false);
		}
		SceneNode::SceneNodes::iterator itr = scenenode->GetChildNodes().begin();
		for(; itr!=scenenode->GetChildNodes().end();itr++){
			HideNodeAndDescendants(*itr);
		}
	}
	void ShowNodeAndDescendants(SceneNodePtr scenenode){
		if(scenenode->HasSceneNodeProperty("renderable")){
			scenenode->GetSceneNodeProperty<RenderableProperty>("renderable").GetRenderable().SetVisibility(true);
		}
		SceneNode::SceneNodes::iterator itr = scenenode->GetChildNodes().begin();
		for(; itr!=scenenode->GetChildNodes().end();itr++){
			ShowNodeAndDescendants(*itr);
		}
	}
	long IntervalPerLevel(int level){
		return (long)(std::pow(0.8f, level+3)*1000);
	}
	template<typename T>
	int RoundToInt(T t){
		/*float dceil = std::ceil(t)-t;
		float dflood = t-std::floor(t);
		if(dceil<dflood){
			return (int)std::ceil(t);
		}else{
			return (int)std::floor(t);
		}*/
		return static_cast<int>(t);
	}
};

#endif /* PACMANLOGIC_HPP_ */
