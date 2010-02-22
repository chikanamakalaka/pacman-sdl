/*
 * TetrisGamestateLoader
 *
 *  Created on: May 5, 2009
 *      Author: asantos
 */
#include <stdlib.h>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/mpl/list.hpp>

class TetrisGamestateLoader{
public:
	SignalBroker& signalbroker;
private:
	SceneGraphController* scenegraphcontroller;
	GamestateController* gamestatecontroller;
	boost::signals::connection scenegraphcontrollerconnection;
	boost::signals::connection gatestatecontrollerconnection;


	class EvNewGame:public boost::statechart::event<EvNewGame>{};
	class EvEndGame:public boost::statechart::event<EvEndGame>{};
	class EvEnteredName:public boost::statechart::event<EvEndGame>{};
	class EvEnterHighScores:public boost::statechart::event<EvEnterHighScores>{};
	class EvEnterCredits:public boost::statechart::event<EvEnterCredits>{};
	class EvEnterConfiguration:public boost::statechart::event<EvEnterConfiguration>{};
	class EvQuit:public boost::statechart::event<EvQuit>{};

	class MenuState;
	class TetrisStateMachine:public boost::statechart::state_machine<TetrisStateMachine, MenuState>{
	private:
		SignalBroker& signalbroker;
	public:

		TetrisStateMachine(SignalBroker& signalbroker):signalbroker(signalbroker){}
		SignalBroker& GetSignalBroker(){
			return signalbroker;
		}
	};

	class MenuState;
	class TetrisState;
	class EnterNameState;
	class HighScoresState;
	class CreditsState;
	class ConfigurationState;
	class TerminatedState;

	class MenuState:public boost::statechart::state<MenuState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::mpl::list<
		boost::statechart::transition< EvNewGame, TetrisState>,
		boost::statechart::transition< EvEnterHighScores, HighScoresState>,
		boost::statechart::transition< EvEnterCredits, CreditsState>,
		boost::statechart::transition< EvEnterConfiguration, ConfigurationState>,
		boost::statechart::transition< EvQuit, TerminatedState>
		> reactions;
		MenuState(my_context ctx):
			boost::statechart::state<MenuState, TetrisStateMachine>(ctx),
			Gamestate("MainMenu", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed MenuState");
		}
		~MenuState(){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Deconstructing MenuState");
		}
	};
	class EnterNameState:public boost::statechart::state<EnterNameState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvEnteredName, HighScoresState > reactions;
		EnterNameState(my_context ctx):
			boost::statechart::state<EnterNameState, TetrisStateMachine>(ctx),
			Gamestate("EnterName", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed EnterNameState");
		}
	};
	class HighScoresState:public boost::statechart::state<HighScoresState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		HighScoresState(my_context ctx):
			boost::statechart::state<HighScoresState, TetrisStateMachine>(ctx),
			Gamestate("HighScores", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed HighScoreState");

			//reload highscores when entering highscores state
			signalbroker.InvokeSignal<TetrisHighScoresGUIController::ReloadHighScoresHandler>("/highscores/reload");

		}
	};

	class TetrisState:public boost::statechart::state<TetrisState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::mpl::list<
		boost::statechart::transition< EvEndGame, EnterNameState >,
		boost::statechart::transition< EvQuit, MenuState >
		>reactions;
		TetrisState(my_context ctx):
			my_base(ctx),
			Gamestate("Tetris", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed TetrisState");

			//reload game when re-entering the game state
			signalbroker.InvokeSignal<TetrisLogic::LoadInitialStateHandler>("/logic/loadinitialstate");

			}
	};
	class CreditsState:public boost::statechart::state<CreditsState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		CreditsState(my_context ctx):
			my_base(ctx),
			Gamestate("Credits", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed CreditsState");
		}
	};
	class ConfigurationState:public boost::statechart::state<ConfigurationState, TetrisStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		ConfigurationState(my_context ctx):
			my_base(ctx),
			Gamestate("Configuration", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed ConfigurationState");
		}
	};
	class TerminatedState:public boost::statechart::state<TerminatedState, TetrisStateMachine>{
	public:
		TerminatedState(my_context ctx):
			my_base(ctx){
			//outermost_context().GetSignalBroker().InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Quit");
			std::exit(0);
		}
	};
	TetrisStateMachine tetrisstatemachine;

public:
	TetrisGamestateLoader(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		scenegraphcontroller(0),
		gamestatecontroller(0),
		tetrisstatemachine(signalbroker){
		scenegraphcontrollerconnection = signalbroker.ConnectToSignal
			<SceneGraphController::GetSceneGraphControllerHandler>
			(
				"/scenegraphcontroller/get",
				boost::bind(&TetrisGamestateLoader::GetSceneGraphController, this, _1));
		gatestatecontrollerconnection = signalbroker.ConnectToSignal
			<GamestateController::GetGamestateControllerHandler>
			(
				"/gamestatecontroller/get",
				boost::bind(&TetrisGamestateLoader::GetGamestateController, this, _1));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/newgame",
				boost::bind(&TetrisGamestateLoader::NewGame, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/endgame",
				boost::bind(&TetrisGamestateLoader::EndGame, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/enteredname",
				boost::bind(&TetrisGamestateLoader::EnteredName, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/enterhighscores",
				boost::bind(&TetrisGamestateLoader::EnterHighScores, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/credits",
				boost::bind(&TetrisGamestateLoader::EnterCredits, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/configuration",
				boost::bind(&TetrisGamestateLoader::EnterConfiguration, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/tetrisgamestatecontroller/quit",
				boost::bind(&TetrisGamestateLoader::Quit, this));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initiating gamestates.");



	}
	void GetSceneGraphController(SceneGraphController& scenegraphcontroller){
		scenegraphcontrollerconnection.disconnect();
		this->scenegraphcontroller = &scenegraphcontroller;
		if(this->scenegraphcontroller && this->gamestatecontroller){
			LoadGamestates();
		}
		
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished TetrisGamestateLoader::GetSceneGraphController()");
	}
	void GetGamestateController(GamestateController& gamestatecontroller){
		gatestatecontrollerconnection.disconnect();
		this->gamestatecontroller = &gamestatecontroller;
		if(this->scenegraphcontroller && this->gamestatecontroller){
			LoadGamestates();
		}
		
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished TetrisGamestateLoader::GetGamestateController()");
	}
	void NewGame(){
		EvNewGame evnewgame;
		tetrisstatemachine.process_event(evnewgame);
	}
	void EndGame(){
		EvEndGame evendgame;
		tetrisstatemachine.process_event(evendgame);
	}
	void EnteredName(){
		EvEnteredName eventeredname;
		tetrisstatemachine.process_event(eventeredname);
	}
	void EnterHighScores(){
		EvEnterHighScores eventerhighscores;
		tetrisstatemachine.process_event(eventerhighscores);
	}
	void EnterCredits(){
		EvEnterCredits eventercredits;
		tetrisstatemachine.process_event(eventercredits);
	}
	void EnterConfiguration(){
		EvEnterConfiguration eventerconfiguration;
		tetrisstatemachine.process_event(eventerconfiguration);
	}
	void Quit(){
		EvQuit evquit;
		tetrisstatemachine.process_event(evquit);
		if(tetrisstatemachine.terminated()){
			exit(0);
		}

	}
	void LoadGamestates(){

		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("MainMenu");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: MainMenu");
		}
		{

			SceneGraph& scenegraph = scenegraphcontroller->CreateSceneGraph("Tetris");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: Tetris");

			SceneNode& root = scenegraph.GetRoot();



			boost::shared_ptr<TriangleStrip> block(new TriangleStrip(false));
			//create block geometry
			block->AddVertex(Vertex(0,0,1, 0,0));
			block->AddVertex(Vertex(24,0,1, 1,0));
			block->AddVertex(Vertex(0,24,1, 0,1));
			block->AddVertex(Vertex(24,24,1, 1,1));

			//create hidden pieces from which to copy later in the game
			{
				// X
				// X
				// X
				// X
				SceneNode& piece0 = root.CreateChildNode("piece0");

				PositionProperty& positionproperty = piece0.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;

				{
					SceneNode& piece0a = piece0.CreateChildNode("piece0a");
					piece0a.AddSceneNodeProperty("geometry", boost::shared_ptr<GeometryProperty>(new GeometryProperty(block)));
					piece0a.AddSceneNodeProperty("texture", boost::shared_ptr<TextureProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/redblock.png"))));
					PositionProperty& positionproperty = piece0a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece0b = piece0.CreateChildNode("piece0b");
					piece0b.AddSceneNodeProperty("geometry", boost::shared_ptr<GeometryProperty>(new GeometryProperty(block)));
					piece0b.AddSceneNodeProperty("texture",  boost::shared_ptr<TextureProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/redblock.png"))));
					PositionProperty& positionproperty = piece0b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece0c = piece0.CreateChildNode("piece0c");
					piece0c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece0c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/redblock.png"))));
					PositionProperty& positionproperty = piece0c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 48.0f;
				}
				{
					SceneNode& piece0d = piece0.CreateChildNode("piece0d");
					piece0d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece0d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/redblock.png"))));
					PositionProperty& positionproperty = piece0d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 72.0f;
				}
			}

			{
				// X X
				// X X
				SceneNode& piece1 = root.CreateChildNode("piece1");

				PositionProperty& positionproperty = piece1.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;
				{
					SceneNode& piece1a = piece1.CreateChildNode("piece1a");
					piece1a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece1a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/blueblock.png"))));
					PositionProperty& positionproperty = piece1a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece1b = piece1.CreateChildNode("piece1b");
					piece1b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece1b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/blueblock.png"))));
					PositionProperty& positionproperty = piece1b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece1c = piece1.CreateChildNode("piece1c");
					piece1c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece1c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/blueblock.png"))));
					PositionProperty& positionproperty = piece1c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece1d = piece1.CreateChildNode("piece1d");
					piece1d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece1d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/blueblock.png"))));
					PositionProperty& positionproperty = piece1d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 24.0f;
				}
			}

			{
				// X
				// X X
				//   X
				SceneNode& piece2 = root.CreateChildNode("piece2");

				PositionProperty& positionproperty = piece2.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;

				{
					SceneNode& piece2a = piece2.CreateChildNode("piece2a");
					piece2a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece2a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/greenblock.png"))));
					PositionProperty& positionproperty = piece2a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece2b = piece2.CreateChildNode("piece2b");
					piece2b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece2b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/greenblock.png"))));
					PositionProperty& positionproperty = piece2b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece2c = piece2.CreateChildNode("piece1c");
					piece2c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece2c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/greenblock.png"))));
					PositionProperty& positionproperty = piece2c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece2d = piece2.CreateChildNode("piece1d");
					piece2d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece2d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/greenblock.png"))));
					PositionProperty& positionproperty = piece2d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 48.0f;
				}
			}

			{
				//   X
				// X X
				// X
				SceneNode& piece3 = root.CreateChildNode("piece3");

				PositionProperty& positionproperty = piece3.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;

				{
					SceneNode& piece3a = piece3.CreateChildNode("piece3a");
					piece3a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece3a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/yellowblock.png"))));
					PositionProperty& positionproperty = piece3a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece3b = piece3.CreateChildNode("piece3b");
					piece3b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece3b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/yellowblock.png"))));
					PositionProperty& positionproperty = piece3b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece3c = piece3.CreateChildNode("piece3c");
					piece3c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece3c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/yellowblock.png"))));
					PositionProperty& positionproperty = piece3c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece3d = piece3.CreateChildNode("piece3d");
					piece3d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece3d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/yellowblock.png"))));
					PositionProperty& positionproperty = piece3d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 48.0f;
				}
			}

			{
				// X
				// X X
				// X
				SceneNode& piece4 = root.CreateChildNode("piece4");

				PositionProperty& positionproperty = piece4.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;

				piece4.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/orangeblock.png"))));
				{
					SceneNode& piece4a = piece4.CreateChildNode("piece4a");
					piece4a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece4a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/orangeblock.png"))));
					PositionProperty& positionproperty = piece4a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece4b = piece4.CreateChildNode("piece4b");
					piece4b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece4b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/orangeblock.png"))));
					PositionProperty& positionproperty = piece4b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece4c = piece4.CreateChildNode("piece4c");
					piece4c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece4c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/orangeblock.png"))));
					PositionProperty& positionproperty = piece4c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece4d = piece4.CreateChildNode("piece4d");
					piece4d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece4d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/orangeblock.png"))));
					PositionProperty& positionproperty = piece4d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 48.0f;
				}
			}

			{
				// X X
				// X
				// X
				SceneNode& piece5 = root.CreateChildNode("piece5");

				PositionProperty& positionproperty = piece5.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;

				{
					SceneNode& piece5a = piece5.CreateChildNode("piece5a");
					piece5a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece5a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/lightblueblock.png"))));
					PositionProperty& positionproperty = piece5a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece5b = piece5.CreateChildNode("piece5b");
					piece5b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece5b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/lightblueblock.png"))));
					PositionProperty& positionproperty = piece5b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece5c = piece5.CreateChildNode("piece5c");
					piece5c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece5c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/lightblueblock.png"))));
					PositionProperty& positionproperty = piece5c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece5d = piece5.CreateChildNode("piece5d");
					piece5d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece5d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/lightblueblock.png"))));
					PositionProperty& positionproperty = piece5d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 48.0f;
				}
			}

			{
				// X X
				//   X
				//   X
				SceneNode& piece6 = root.CreateChildNode("piece6");

				PositionProperty& positionproperty = piece6.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 160.0f;
				{
					SceneNode& piece6a = piece6.CreateChildNode("piece6a");
					piece6a.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece6a.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/purpleblock.png"))));
					PositionProperty& positionproperty = piece6a.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 0.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece6b = piece6.CreateChildNode("piece6b");
					piece6b.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece6b.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/purpleblock.png"))));
					PositionProperty& positionproperty = piece6b.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 0.0f;
				}
				{
					SceneNode& piece6c = piece6.CreateChildNode("piece6c");
					piece6c.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece6c.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/purpleblock.png"))));
					PositionProperty& positionproperty = piece6c.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 24.0f;
				}
				{
					SceneNode& piece6d = piece6.CreateChildNode("piece6d");
					piece6d.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(block)));
					piece6d.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/purpleblock.png"))));
					PositionProperty& positionproperty = piece6d.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					m(0,3) = 24.0f;
					m(1,3) = 48.0f;
				}
			}


			//create wall(s)
			boost::shared_ptr<TriangleStrip> wall(new TriangleStrip(true));
			wall->AddVertex(Vertex(0,0,0));
			wall->AddVertex(Vertex(5,0,0));
			wall->AddVertex(Vertex(0,480,0));
			wall->AddVertex(Vertex(5,480,0));

			{
				SceneNode& rightwall = root.CreateChildNode("rightwall");


				PositionProperty& positionproperty = rightwall.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 195.0f;
				m(1,3) = 0.0f;
				rightwall.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(wall)));
				/*TextureProperty& textureproperty = */rightwall.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/white.png"))));
			}

			{
				SceneNode& leftwall = root.CreateChildNode("leftwall");


				PositionProperty& positionproperty = leftwall.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 440.0f;
				m(1,3) = 0.0f;
				leftwall.AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(wall)));
				/*TextureProperty& textureproperty = */leftwall.AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath("/images/white.png"))));
			}

			{
				SceneNode& nextpeice = root.CreateChildNode("nextpiece");
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenenode: nextpeice.");


				PositionProperty& positionproperty = nextpeice.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 492.0f;
				m(1,3) = 240.5f;

				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created positionproperty for nextpeice.");
			}

			{
				SceneNode& currentpiece = root.CreateChildNode("currentpiece");
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenenode: currentpiece.");


				PositionProperty& positionproperty = currentpiece.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 320.0f;
				m(1,3) = 0.0f;

				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created positionproperty for currentpiece.");
			}

			{
				SceneNode& placedpeices = root.CreateChildNode("placedpieces");
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenenode: placedpeices.");


				PositionProperty& positionproperty = placedpeices.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
				Matrix4& m = positionproperty.GetPosition();
				//[row, column]
				m(0,3) = 200.0f;
				m(1,3) = 0.0f;

				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created positionproperty for placedpeices.");

				//create all line nodes. These will hold a line of blocks for placed pieces
				for(int i=0; i<20; i++)
				{
					std::string name("line"+boost::lexical_cast<std::string>(i));
					SceneNode& line = placedpeices.CreateChildNode(name);
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenenode: "+name+".");


					PositionProperty& positionproperty = line.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
					Matrix4& m = positionproperty.GetPosition();
					//[row, column]
					//lines from the top down, line0 = top, line 19=bottom
					m(0,3) = 0.0f;
					m(1,3) = 24.0f*i;

					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created positionproperty for "+name+".");
				}
			}
		}
		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("EnterName");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: EnterName");
		}
		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("HighScores");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: HighScores");
		}
		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("Credits");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: Credits");
		}
		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("Configuration");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: Configuration");
		}
		//block all namespaces events before starting, tetrisstatemachine.initiate() will open up the entry state event namespace
		signalbroker.Block("MainMenu");
		signalbroker.Block("Tetris");
		signalbroker.Block("EnterName");
		signalbroker.Block("HighScores");
		signalbroker.Block("Credits");
		signalbroker.Block("Configuration");


		tetrisstatemachine.initiate();
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished initializing gamestates");
	}

private:

};
