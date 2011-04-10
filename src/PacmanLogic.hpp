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

	std::vector<int> pacmanposition;


	std::vector<SceneNodePtr> dots;
	std::vector<SceneNodePtr> powerups;

	boost::numeric::ublas::matrix<bool> collisionmap;
	boost::mt19937 rng;
	boost::uniform_int<> seven;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die;

	static const float pxpersec = 16.0f;


public:
	PacmanLogic(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Pacman", "PacmanLogic"),
		signalbroker(signalbroker),
		score(0),
		level(0),
		pacmanposition(std::vector<int>(2)),
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

		SignalSubscriber::ConnectToSignal<SignalBroker::GenericHandler>
		(	"/input/player/movedown",
			boost::bind(&PacmanLogic::MovePlayerDown, this, _1));


		SignalSubscriber::ConnectToSignal
		<SignalBroker::GenericHandler>
		(	"/input/player/moveup",
			boost::bind(&PacmanLogic::MovePlayerUp, this, _1));

		SignalSubscriber::ConnectToSignal
		<SignalBroker::GenericHandler>
		(	"/input/player/moveleft",
			boost::bind(&PacmanLogic::MovePlayerLeft, this, _1));

		SignalSubscriber::ConnectToSignal
		<SignalBroker::GenericHandler>
		(	"/input/player/moveright",
				boost::bind(&PacmanLogic::MovePlayerRight, this, _1));

		//LoadLevel(1);

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
		if(scenegraphptr->GetName() == "Pacman")
		//TODO:capture characters
		try{
			pacman = scenegraphptr->GetNodePtrByPath("/level/characters/pacman");
			blinky = scenegraphptr->GetNodePtrByPath("/level/characters/blinky");
			inky = scenegraphptr->GetNodePtrByPath("/level/characters/inky");
			pinky = scenegraphptr->GetNodePtrByPath("/level/characters/pinky");
			clyde = scenegraphptr->GetNodePtrByPath("/level/characters/clyde");

			pacmanposition = GetSceneNodeGridCoordinates(pacman);

		}catch(SceneNodeDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string(e.what()));
			throw;
		}
		//TODO:capture collision grid
		int maxx = 0;
		int maxy = 0;


		//get the level's collision map
		try{
			SceneNodePtr level = scenegraphptr->GetNodePtrByPath("/level");
			if(level->HasSceneNodeProperty("pacmanlogicdata")){
				const boost::numeric::ublas::matrix<bool>& collisionmap = level->GetSceneNodeProperty<PacmanLogicDataProperty>("pacmanlogicdata").GetCollisionMap();
				this->collisionmap.resize(collisionmap.size1(), collisionmap.size2());
				this->collisionmap = collisionmap;
			}
		}catch(SceneNodeDoesNotExist e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string(e.what()));
			throw;
		}

		//TODO:capture fruit
		//TODO:capture dots
		//TODO:capture powerups
		//TODO:capture eyes

		//Process logic at appropriate interval
		signalbroker.InvokeSignal<TimerView::IntervalHandler>(
			"/timer/setinterval",
			"/pacmanlogic/processlogic", boost::bind(&PacmanLogic::ProcessLogic, this, _1, _2), 100);

	}

	void LoadInitialState(){

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

	void MovePlayerDown(const ArgsList& args){
		const std::vector<boost::any> values = args.GetValues();
		float t = boost::any_cast<long>(values[0])/1000.0f;
		float dt = boost::any_cast<long>(values[1])/1000.0f;
		if(pacman){
			std::vector<int> position = pacmanposition;
			position[1]+=dt;
			if(CanMoveToPosition(position)){
				PositionProperty& pacmanposition = pacman->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mpacman = pacmanposition.GetPosition();
				mpacman(1,3)+=pxpersec * dt;
				this->pacmanposition = position;
			}
		}
	}
	void MovePlayerUp(const ArgsList& args){
		const std::vector<boost::any> values = args.GetValues();
		float t = boost::any_cast<long>(values[0])/1000.0f;
		float dt = boost::any_cast<long>(values[1])/1000.0f;
		if(pacman){
			std::vector<int> position = pacmanposition;
			position[1]-=dt;
			if(CanMoveToPosition(position)){
				PositionProperty& pacmanposition = pacman->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mpacman = pacmanposition.GetPosition();
				mpacman(1,3)-=pxpersec * dt;
				this->pacmanposition = position;
			}
		}
	}

	void MovePlayerLeft(const ArgsList& args){
		const std::vector<boost::any> values = args.GetValues();
		float t = boost::any_cast<long>(values[0])/1000.0f;
		float dt = boost::any_cast<long>(values[1])/1000.0f;
		if(pacman){
			std::vector<int> position = pacmanposition;
			position[0]-=dt;
			if(CanMoveToPosition(position)){
				PositionProperty& pacmanposition = pacman->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mpacman = pacmanposition.GetPosition();
				mpacman(0,3)-=pxpersec * dt;
				this->pacmanposition = position;
			}
		}
	}
	void MovePlayerRight(const ArgsList& args){
		const std::vector<boost::any> values = args.GetValues();
		float t = boost::any_cast<long>(values[0])/1000.0f;
		float dt = boost::any_cast<long>(values[1])/1000.0f;
		if(pacman){
			std::vector<int> position = pacmanposition;
			position[0]+=dt;
			if(CanMoveToPosition(position)){
				PositionProperty& pacmanposition = pacman->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mpacman = pacmanposition.GetPosition();
				mpacman(0,3)+=pxpersec * dt;
				this->pacmanposition = position;
			}
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

	bool CanMoveToPosition(const std::vector<int> position)const{
		return CanMoveToPosition(position[0], position[1]);
	}

	bool CanMoveToPosition(int x, int y)const{
		std::stringstream ss;
		ss<<"rows:"<<collisionmap.size1()<<" cols:"<<collisionmap.size2()<<" x:"<<x<<" y:"<<y<<std::endl;
		//for each row
		for(int i=0; i<collisionmap.size1(); i++){
			//for each column
			for(int j=0; j<collisionmap.size2(); j++){
				if(i==y/16 && j==x/16){
					ss<<"p";
				}else if(collisionmap(i, j)){
					ss<<"x";
				}else{
					ss<<"-";
				}
			}
			ss<<std::endl;
		}
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());

		if(y<collisionmap.size1() && x<collisionmap.size2()){
			return !collisionmap(y/16, x/16);
		} else {
			return true;
		}
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
	std::vector<int> GetSceneNodeGridCoordinates(SceneNodePtr scenenodeptr)const{
		std::vector<int> result(2);
		if(scenenodeptr){
			const PositionProperty& position = scenenodeptr->GetSceneNodeProperty<PositionProperty>("position");
			const Matrix4& m = position.GetPosition();
			result[0] = m(0,3);
			result[1] = m(1,3);
		}
		return result;
	}
};

#endif /* PACMANLOGIC_HPP_ */
