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

	int score;
	int level;

	bool initialized;
	SceneNodePtr currentpiece;
	SceneNodePtr nextpiece;
	SceneNodePtr piece0;
	SceneNodePtr piece1;
	SceneNodePtr piece2;
	SceneNodePtr piece3;
	SceneNodePtr piece4;
	SceneNodePtr piece5;
	SceneNodePtr piece6;
	std::vector<SceneNodePtr> pieces;

	int currentpiecenumber;
	int nextpiecenumber;

	int currentpieceorientation;

	SceneNodePtr placedpieces;
	std::map<int, SceneNodePtr > lines;
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
		currentpieceorientation(0),
		seven(0,6),
		die(rng, seven)
{
		rng.seed(time(0));

		getselectedscenegraphconnection = SignalSubscriber::ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/scenegraphcontroller/getselectedscenegraph",
			boost::bind(&PacmanLogic::GetSelectedSceneGraph, this, _1));


		SignalSubscriber::ConnectToSignal
		<PacmanLogic::LoadInitialStateHandler>
		(	"/logic/loadinitialstate",
			boost::bind(&PacmanLogic::LoadInitialState, this));

		SignalSubscriber::ConnectToSignal<InputView::PlayerMovementHandler>
		(	"/input/player/movedown",
			boost::bind(&PacmanLogic::MovePlayerDown, this));


		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/movenormal",
			boost::bind(&PacmanLogic::MovePlayerNormal, this));

		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/moveleft",
			boost::bind(&PacmanLogic::MovePlayerLeft, this));

		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/moveright",
				boost::bind(&PacmanLogic::MovePlayerRight, this));

		SignalSubscriber::ConnectToSignal
		<InputView::PlayerMovementHandler>
		(	"/input/player/rotate",
			boost::bind(&PacmanLogic::RotatePlayer, this));

		nextpiecenumber = die();

		//make a blank new collision map
		collisionmap.resize(20);
		for(int i=0; i<20; i++){
			collisionmap[i].resize(10);
			for(int j=0; j<10; j++){
				collisionmap[i][j] = false;
			}
		}
	}
	void GetSelectedSceneGraph(boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "PacmanLogic::GetSelectedSceneGraph");

		try{
			currentpiece = scenegraph->GetRoot().GetChildNodePtrByName("currentpiece");
			nextpiece = scenegraph->GetRoot().GetChildNodePtrByName("nextpiece");
			placedpieces = scenegraph->GetRoot().GetChildNodePtrByName("placedpieces");

			pieces.resize(7);

			pieces[0] = piece0 = scenegraph->GetRoot().GetChildNodePtrByName("piece0");
			pieces[1] = piece1 = scenegraph->GetRoot().GetChildNodePtrByName("piece1");
			pieces[2] = piece2 = scenegraph->GetRoot().GetChildNodePtrByName("piece2");
			pieces[3] = piece3 = scenegraph->GetRoot().GetChildNodePtrByName("piece3");
			pieces[4] = piece4 = scenegraph->GetRoot().GetChildNodePtrByName("piece4");
			pieces[5] = piece5 = scenegraph->GetRoot().GetChildNodePtrByName("piece5");
			pieces[6] = piece6 = scenegraph->GetRoot().GetChildNodePtrByName("piece6");

			lines[0] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line1");
			lines[2] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line2");
			lines[3] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line3");
			lines[4] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line4");
			lines[5] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line5");
			lines[6] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line6");
			lines[7] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line7");
			lines[8] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line8");
			lines[9] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line9");
			lines[10] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line10");
			lines[11] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line11");
			lines[12] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line12");
			lines[13] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line13");
			lines[14] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line14");
			lines[15] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line15");
			lines[16] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line16");
			lines[17] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line17");
			lines[18] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line18");
			lines[19] = scenegraph->GetRoot().GetChildNodeByName("placedpieces").GetChildNodePtrByName("line19");

			getselectedscenegraphconnection.disconnect();
			initialized = true;


			//Process logic at appropriate interval
			signalbroker.InvokeSignal<TimerView::IntervalHandler>(
					"/timer/setinterval",
					"/pacmanlogic/processlogic", boost::bind(&PacmanLogic::ProcessLogic, this, _1, _2), IntervalPerLevel(level));


			SetUpNextPeice();

		}catch(...){

		}
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

		//reset all places pieces in lines
		std::map<int, SceneNodePtr >::iterator itr = lines.begin();
		for(; itr!=lines.end();itr++){
			itr->second->DeleteChildNodes();
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
		if(initialized){
			signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level+6));
		}
	}
	void MovePlayerNormal(){
		if(initialized){
			signalbroker.InvokeSignal<TimerView::UpdateIntervalLengthHandler>("/timer/updateinterval", "/pacmanlogic/processlogic", IntervalPerLevel(level));
		}
	}

	void MovePlayerLeft(){
			if(initialized){
				if(currentpiece->HasSceneNodeProperty("position")){
					PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
					Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

					if(CanMoveToLeft()){
						signalbroker.InvokeSignal
						<PacmanLogic::AbleToMoveHandler>("/pacmanlogic/abletomove");

						{
							std::stringstream ss;
							ss << "current piece position " << mcurrentpieceposition(0,3) << ", " << mcurrentpieceposition(1,3);
							signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
								ss.str());
						}

						mcurrentpieceposition(0,3)-=24;

						{
							std::stringstream ss;
							ss << "new piece position " << mcurrentpieceposition(0,3) << ", " << mcurrentpieceposition(1,3);
							signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
								ss.str());
						}

					}else{
						signalbroker.InvokeSignal
						<PacmanLogic::UnableToMoveHandler>("/pacmanlogic/unabletomove");
					}
				}
			}
		}
		void MovePlayerRight(){
			if(initialized){
				if(currentpiece->HasSceneNodeProperty("position")){
					PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
					Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

					if(CanMoveToRight()){
						signalbroker.InvokeSignal
						<PacmanLogic::AbleToMoveHandler>("/pacmanlogic/abletomove");

						mcurrentpieceposition(0,3)+=24;
					}else{
						signalbroker.InvokeSignal
						<PacmanLogic::UnableToMoveHandler>("/pacmanlogic/unabletomove");
					}
				}
			}
		}
		void RotatePlayer(){
			using namespace boost::numeric::ublas;

			if(initialized){
				if(CanRotate()){
					signalbroker.InvokeSignal
					<PacmanLogic::AbleToRotateHandler>("/pacmanlogic/abletorotate");

					bounded_matrix<float,4,4> center(identity_matrix<float>(4));

					bounded_matrix<float,4,4> rotatecc(zero_matrix<float>(4));
					rotatecc(0,1)=-1.0f;
					rotatecc(1,0)=1.0f;
					rotatecc(2,2)=1.0f;
					rotatecc(3,3)=1.0f;
					switch(currentpiecenumber){
						case 0:
							center(0,3) = 0.0f;
							center(1,3) = -48.0f;
						break;
						case 1:
							center(0,3) = -12.0f;
							center(1,3) = -12.0f;
						break;

						case 2:
						case 3:
						case 4:
							center(0,3) = 0.0f;
							center(1,3) = -24.0f;
							break;
						case 5:
						case 6:
							center(0,3) = -24.0f;
							center(1,3) = -24.0f;
						break;
					}
					bounded_matrix<float,4,4> inverse(Matrix4::Inverse(center));
					bounded_matrix<float,4,4> A(prod(inverse, rotatecc));
					bounded_matrix<float,4,4> mrotatedposition(prod(A, center));

					SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
					SceneNode::SceneNodes::iterator itr=childnodes.begin();
					for(;itr!=childnodes.end();itr++){
						try{
							PositionProperty& positionproperty = (*itr)->GetSceneNodeProperty<PositionProperty>("position");
							Matrix4& mposition = positionproperty.GetPosition();

							bounded_vector<float, 4> t;
							t(0) = mposition(0,3);
							t(1) = mposition(1,3);
							t(3) = 1.0f;

							bounded_vector<float,4> u(prod(mrotatedposition, t));

							mposition(0,3)=u(0);
							mposition(1,3)=u(1);
						}catch(SceneNodePropertyDoesNotExist e){
							signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
								"Could not get position property from scenennode:" + (*itr)->GetName());
						}
					}
				}else{
					signalbroker.InvokeSignal
					<PacmanLogic::UnableToRotateHandler>("/pacmanlogic/unabletorotate");
				}
			}
		}
protected:
	void ProcessLogic(long t, long dt){

		//current piece hit bottom?
		if(StopCurrentPiece()){
			signalbroker.InvokeSignal
			<PacmanLogic::CurrentPieceStoppedHandler>("/pacmanlogic/currentpiecestopped");

			MoveCurrentPieceIntoLines();
			RemoveCompleteRows();
			SetUpNextPeice();
			if(StopCurrentPiece()){
				//lose condition, next piece immediately stopped :(
				signalbroker.InvokeSignal
				<PacmanLogic::GameOverHandler>
				("/pacmanlogic/gameover");

				//stop updating logic, we're exiting to highscores or enter name menu
				/*signalbroker.InvokeSignal
					<PacmanLogic::PauseIntervalHandler>
					("/timer/pauseinterval", "/pacmanlogic/processlogic");*/

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

		//drop current piece down
		MoveCurrentPieceDown();
	}
	void MoveCurrentPieceDown(){
		PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();
		mcurrentpieceposition(1,3)+= 24.0f;
	}
	bool StopCurrentPiece(){
		PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

		PositionProperty& placedpiecesposition = placedpieces->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcplacedpiecesposition = placedpiecesposition.GetPosition();


		SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
		SceneNode::SceneNodes::iterator itr=childnodes.begin();
		for(;itr!=childnodes.end();itr++){
			try{
				PositionProperty& positionproperty = (*itr)->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mposition = positionproperty.GetPosition();

				PositionProperty& linepositionproperty = lines[(int)(mcurrentpieceposition(1,3)/24+mposition(1,3)/24)]->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4 mlineposition = linepositionproperty.GetPosition();


				int line = (int)(mcurrentpieceposition(1,3)+mposition(1,3))/24;
				int column = (int)(mcurrentpieceposition(0,3) + mposition(0,3) - mcplacedpiecesposition(0,3) - mlineposition(0,3))/24;
				if(line==19){
					return true;
				}
				if(collisionmap[line+1][column]){
					return true;
				}
			}catch(SceneNodePropertyDoesNotExist e){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
					"Could not get position property from scenennode:" + (*itr)->GetName());
			}
		}
		return false;
	}
	void MoveCurrentPieceIntoLines(){
		PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

		PositionProperty& placedpiecesposition = placedpieces->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcplacedpiecesposition = placedpiecesposition.GetPosition();

		SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
		SceneNode::SceneNodes::iterator itr=childnodes.begin();
		for(;itr!=childnodes.end();itr++){
			try{
				SceneNode::SceneNodePtr clone = (*itr)->Clone();
				PositionProperty& clonepositionproperty = clone->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4& mcloneposition = clonepositionproperty.GetPosition();

				PositionProperty& linepositionproperty = lines[(int)(mcurrentpieceposition(1,3)/24+mcloneposition(1,3)/24)]->GetSceneNodeProperty<PositionProperty>("position");
				Matrix4 mlineposition = linepositionproperty.GetPosition();

				//copy *itr scenenode from current piece to lines[y]
				int line = (int)((mcurrentpieceposition(1,3)+mcloneposition(1,3))/24);
				lines[line]->AddChildNode(clone);
				float x = mcurrentpieceposition(0,3) + mcloneposition(0,3) - mcplacedpiecesposition(0,3) - mlineposition(0,3);
				int column = (int)(x/24);
				mcloneposition(0,3)=x;
				mcloneposition(1,3)=0.0;

				//update collision map
				collisionmap[line][column]=true;
			}catch(SceneNodePropertyDoesNotExist e){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
					"Could not get position property from scenennode:" + (*itr)->GetName());
			}
		}
		currentpiece->DeleteChildNodes();


	}
	void RemoveCompleteRows(){
		int rowscleared = 0;
		for(int i=0;i<20;i++){
			bool contiguous = true;
			for(int j=0;j<10;j++){
				if(!collisionmap[i][j]){
					contiguous=false;
				}
			}
			if(!contiguous){
				continue;
			}

			//line contiguous
			//move collision map down and child nodes down
			for(int k=i;k>0;k--){
				for(int j=0;j<10;j++){
					collisionmap[k][j]=collisionmap[k-1][j];
				}
				lines[k]->DeleteChildNodes();
				lines[k-1]->CopyChildNodesToTargetParent(*(lines[k]));
			}
			//blank top row
			for(int j=0;j<10;j++){
				collisionmap[0][j]=false;
			}
			lines[0]->DeleteChildNodes();

			rowscleared++;
		}

		//increment score per rows cleared
		IncrementScore(rowscleared);
	}
	void SetUpNextPeice(){

		//Hide last piece
		HideNodeAndDescendants(pieces[nextpiecenumber]);

		//get new peice
		currentpiecenumber = nextpiecenumber;
		nextpiecenumber = die();

		//Show new next piece
		ShowNodeAndDescendants(pieces[nextpiecenumber]);
		

		//Clone pieces[currentpiecenumber]
		CopyIntoCurrentPiece(currentpiecenumber);


		ShowNodeAndDescendants(currentpiece);

		//reset current piece location to top of screen
		PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();
		mcurrentpieceposition(0,3) = 320.0f;
		mcurrentpieceposition(1,3) = 0.0f;

	}
	bool CanMoveToLeft()const{
		PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

		PositionProperty& placedpiecesposition = placedpieces->GetSceneNodeProperty<PositionProperty>("position");
		Matrix4 mplacedpiecesposition = placedpiecesposition.GetPosition();

		const SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
		SceneNode::SceneNodes::const_iterator itr=childnodes.begin();
		for(;itr!=childnodes.end();itr++){
			try{
				const PositionProperty& positionproperty = (*itr)->GetSceneNodeProperty<PositionProperty>("position");
				const Matrix4& mposition = positionproperty.GetPosition();

				int line = (int)(mcurrentpieceposition(1,3)+mposition(1,3))/24;


				const PositionProperty& linepositionproperty = lines.find(line)->second->GetSceneNodeProperty<PositionProperty>("position");
				const Matrix4& mlineposition = linepositionproperty.GetPosition();

				int column = (int)(mcurrentpieceposition(0,3) + mposition(0,3) - mplacedpiecesposition(0,3) - mlineposition(0,3))/24;

				//Left wall boundary?
				if(column<=0){
					return false;
				}
				//placed pieces collision?
				if(collisionmap[line][column-1]){
					return false;
				}
			}catch(SceneNodePropertyDoesNotExist e){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
						"Could not get position property from scenennode:" + (*itr)->GetName());
			}
		}
		return true;
	}

	bool CanMoveToRight()const{
		const PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		const Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

		const PositionProperty& placedpiecesposition = placedpieces->GetSceneNodeProperty<PositionProperty>("position");
		const Matrix4& mplacedpiecesposition = placedpiecesposition.GetPosition();


		const SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
		SceneNode::SceneNodes::const_iterator itr=childnodes.begin();
		for(;itr!=childnodes.end();itr++){
			try{
				const PositionProperty& positionproperty = (*itr)->GetSceneNodeProperty<PositionProperty>("position");
				const Matrix4& mposition = positionproperty.GetPosition();

				int line = (int)(mcurrentpieceposition(1,3)+mposition(1,3))/24;

				const PositionProperty& linepositionproperty = lines.find(line)->second->GetSceneNodeProperty<PositionProperty>("position");
				const Matrix4 mlineposition = linepositionproperty.GetPosition();

				int column = (int)(mcurrentpieceposition(0,3) + mposition(0,3) - mplacedpiecesposition(0,3) - mlineposition(0,3))/24;
				//Right wall boundary?
				if(column>=9){
					return false;
				}
				//placed pieces collision?
				if(collisionmap[line][column+1]){
					return false;
				}
			}catch(SceneNodePropertyDoesNotExist e){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
					"Could not get position property from scenennode:" + (*itr)->GetName());
			}
		}
		return true;
	}
	bool CanRotate(){
		using namespace boost::numeric::ublas;

		bounded_matrix<float,4,4> center(identity_matrix<float>(4));

		bounded_matrix<float,4,4> rotatecc(zero_matrix<float>(4));
		rotatecc(0,1)=-1.0f;
		rotatecc(1,0)=1.0f;
		rotatecc(2,2)=1.0f;
		rotatecc(3,3)=1.0f;
		switch(currentpiecenumber){
			case 0:
				center(0,3) = 0.0f;
				center(1,3) = -48.0f;
			break;
			case 1:
				center(0,3) = -12.0f;
				center(1,3) = -12.0f;
			break;

			case 2:
			case 3:
			case 4:
				center(0,3) = 0.0f;
				center(1,3) = -24.0f;
				break;
			case 5:
			case 6:
				center(0,3) = -24.0f;
				center(1,3) = -24.0f;
			break;
		}
		bounded_matrix<float,4,4> inverse(Matrix4::Inverse(center));
		bounded_matrix<float,4,4> A(prod(inverse, rotatecc));
		bounded_matrix<float,4,4> mrotatedposition(prod(A, center));

		const PositionProperty& currentpieceposition = currentpiece->GetSceneNodeProperty<PositionProperty>("position");
		const Matrix4& mcurrentpieceposition = currentpieceposition.GetPosition();

		const PositionProperty& placedpiecesposition = placedpieces->GetSceneNodeProperty<PositionProperty>("position");
		const Matrix4& mplacedpiecesposition = placedpiecesposition.GetPosition();


		const SceneNode::SceneNodes& childnodes(currentpiece->GetChildNodes());
		SceneNode::SceneNodes::const_iterator itr=childnodes.begin();

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"Rotating piece:");

		for(;itr!=childnodes.end();itr++){
			try{
				const PositionProperty& positionproperty = (*itr)->GetSceneNodeProperty<PositionProperty>("position");
				const Matrix4& mposition = positionproperty.GetPosition();

				bounded_vector<float, 4> t;
				t(0) = mposition(0,3);
				t(1) = mposition(1,3);
				t(3) = 1.0f;

				bounded_vector<float,4> u(prod(mrotatedposition, t));

				int newx = RoundToInt(u(0));
				int newy = RoundToInt(u(1));

				{
					std::stringstream ss;
					ss << "x:"  << newx << " y:" << newy;
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
						ss.str());
				}
				{
					std::stringstream ss;
					ss << "current piece position " << mcurrentpieceposition(0,3) << ", " << mcurrentpieceposition(1,3);
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
						ss.str());
				}
				//calculate correct line and column for rotate piece
				int line = static_cast<int>(mcurrentpieceposition(1,3)+newy)/24;

				int column = static_cast<int>(mcurrentpieceposition(0,3) + newx - mplacedpiecesposition(0,3))/24;

				std::stringstream ss;
				ss << "col:" << column << " row:" << line;
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
					ss.str());

				if(line > 19 || line < 0){
					return false;
				}

				//Left / Right wall boundary?
				if(column > 9 || column < 0){
					return false;
				}

				//placed pieces collision?
				if(collisionmap[line][column]){
					return false;
				}
			}catch(SceneNodePropertyDoesNotExist e){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
					"Could not get position property from scenennode:"+(*itr)->GetName());
			}
		}
		return true;
	}
	void CopyIntoCurrentPiece(int piecenumber){
		currentpiece->DeleteChildNodes();
		pieces[piecenumber]->CopyChildNodesToTargetParent(*currentpiece);
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
