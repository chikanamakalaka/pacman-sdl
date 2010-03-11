/*
 * PacmanGamestateLoader
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
#include "xmlguichan/tinyxml.h"
class PacmanGamestateLoader{
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
	class PacmanStateMachine:public boost::statechart::state_machine<PacmanStateMachine, MenuState>{
	private:
		SignalBroker& signalbroker;
	public:

		PacmanStateMachine(SignalBroker& signalbroker):signalbroker(signalbroker){}
		SignalBroker& GetSignalBroker(){
			return signalbroker;
		}
	};

	class MenuState;
	class PacmanState;
	class EnterNameState;
	class HighScoresState;
	class CreditsState;
	class ConfigurationState;
	class TerminatedState;

	class MenuState:public boost::statechart::state<MenuState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::mpl::list<
		boost::statechart::transition< EvNewGame, PacmanState>,
		boost::statechart::transition< EvEnterHighScores, HighScoresState>,
		boost::statechart::transition< EvEnterCredits, CreditsState>,
		boost::statechart::transition< EvEnterConfiguration, ConfigurationState>,
		boost::statechart::transition< EvQuit, TerminatedState>
		> reactions;
		MenuState(my_context ctx):
			boost::statechart::state<MenuState, PacmanStateMachine>(ctx),
			Gamestate("MainMenu", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed MenuState");
		}
		~MenuState(){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Deconstructing MenuState");
		}
	};
	class EnterNameState:public boost::statechart::state<EnterNameState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvEnteredName, HighScoresState > reactions;
		EnterNameState(my_context ctx):
			boost::statechart::state<EnterNameState, PacmanStateMachine>(ctx),
			Gamestate("EnterName", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed EnterNameState");
		}
	};
	class HighScoresState:public boost::statechart::state<HighScoresState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		HighScoresState(my_context ctx):
			boost::statechart::state<HighScoresState, PacmanStateMachine>(ctx),
			Gamestate("HighScores", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed HighScoreState");

			//reload highscores when entering highscores state
			signalbroker.InvokeSignal<PacmanHighScoresGUIController::ReloadHighScoresHandler>("/highscores/reload");

		}
	};

	class PacmanState:public boost::statechart::state<PacmanState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::mpl::list<
		boost::statechart::transition< EvEndGame, EnterNameState >,
		boost::statechart::transition< EvQuit, MenuState >
		>reactions;
		PacmanState(my_context ctx):
			my_base(ctx),
			Gamestate("Pacman", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed PacmanState");

			//reload game when re-entering the game state
			signalbroker.InvokeSignal<PacmanLogic::LoadInitialStateHandler>("/logic/loadinitialstate");

			}
	};
	class CreditsState:public boost::statechart::state<CreditsState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		CreditsState(my_context ctx):
			my_base(ctx),
			Gamestate("Credits", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed CreditsState");
		}
	};
	class ConfigurationState:public boost::statechart::state<ConfigurationState, PacmanStateMachine>,public Gamestate{
	public:
		typedef boost::statechart::transition< EvQuit, MenuState > reactions;
		ConfigurationState(my_context ctx):
			my_base(ctx),
			Gamestate("Configuration", outermost_context().GetSignalBroker()){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Constructed ConfigurationState");
		}
	};
	class TerminatedState:public boost::statechart::state<TerminatedState, PacmanStateMachine>{
	public:
		TerminatedState(my_context ctx):
			my_base(ctx){
			//outermost_context().GetSignalBroker().InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Quit");
			std::exit(0);
		}
	};
	PacmanStateMachine Pacmanstatemachine;

public:
	PacmanGamestateLoader(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		scenegraphcontroller(0),
		gamestatecontroller(0),
		Pacmanstatemachine(signalbroker){
		scenegraphcontrollerconnection = signalbroker.ConnectToSignal
			<SceneGraphController::GetSceneGraphControllerHandler>
			(
				"/scenegraphcontroller/get",
				boost::bind(&PacmanGamestateLoader::GetSceneGraphController, this, _1));
		gatestatecontrollerconnection = signalbroker.ConnectToSignal
			<GamestateController::GetGamestateControllerHandler>
			(
				"/gamestatecontroller/get",
				boost::bind(&PacmanGamestateLoader::GetGamestateController, this, _1));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/newgame",
				boost::bind(&PacmanGamestateLoader::NewGame, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/endgame",
				boost::bind(&PacmanGamestateLoader::EndGame, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/enteredname",
				boost::bind(&PacmanGamestateLoader::EnteredName, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/enterhighscores",
				boost::bind(&PacmanGamestateLoader::EnterHighScores, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/credits",
				boost::bind(&PacmanGamestateLoader::EnterCredits, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/configuration",
				boost::bind(&PacmanGamestateLoader::EnterConfiguration, this));

		signalbroker.ConnectToSignal
			<GamestateController::StateChangeHandler>
			(
				"/pacmangamestatecontroller/quit",
				boost::bind(&PacmanGamestateLoader::Quit, this));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initiating gamestates.");



	}
	void GetSceneGraphController(SceneGraphController& scenegraphcontroller){
		scenegraphcontrollerconnection.disconnect();
		this->scenegraphcontroller = &scenegraphcontroller;
		if(this->scenegraphcontroller && this->gamestatecontroller){
			LoadGamestates();
		}
		
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished PacmanGamestateLoader::GetSceneGraphController()");
	}
	void GetGamestateController(GamestateController& gamestatecontroller){
		gatestatecontrollerconnection.disconnect();
		this->gamestatecontroller = &gamestatecontroller;
		if(this->scenegraphcontroller && this->gamestatecontroller){
			LoadGamestates();
		}
		
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished PacmanGamestateLoader::GetGamestateController()");
	}
	void NewGame(){
		EvNewGame evnewgame;
		Pacmanstatemachine.process_event(evnewgame);
	}
	void EndGame(){
		EvEndGame evendgame;
		Pacmanstatemachine.process_event(evendgame);
	}
	void EnteredName(){
		EvEnteredName eventeredname;
		Pacmanstatemachine.process_event(eventeredname);
	}
	void EnterHighScores(){
		EvEnterHighScores eventerhighscores;
		Pacmanstatemachine.process_event(eventerhighscores);
	}
	void EnterCredits(){
		EvEnterCredits eventercredits;
		Pacmanstatemachine.process_event(eventercredits);
	}
	void EnterConfiguration(){
		EvEnterConfiguration eventerconfiguration;
		Pacmanstatemachine.process_event(eventerconfiguration);
	}
	void Quit(){
		EvQuit evquit;
		Pacmanstatemachine.process_event(evquit);
		if(Pacmanstatemachine.terminated()){
			exit(0);
		}

	}
	void LoadGamestates(){

		{
			/*SceneGraph& scenegraph = */scenegraphcontroller->CreateSceneGraph("MainMenu");
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph: MainMenu");
		}
		{

			//SceneGraph& scenegraph = //scenegraphcontroller->CreateSceneGraph("Pacman");
			LoadSceneGraphFromFile(FileSystem::MakeUsrLocalPath("/levels/level1.xml"), "Pacman");
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
		//block all namespaces events before starting, Pacmanstatemachine.initiate() will open up the entry state event namespace
		signalbroker.Block("MainMenu");
		signalbroker.Block("Pacman");
		signalbroker.Block("EnterName");
		signalbroker.Block("HighScores");
		signalbroker.Block("Credits");
		signalbroker.Block("Configuration");


		Pacmanstatemachine.initiate();
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finished initializing gamestates");
	}

private:
	boost::shared_ptr<SceneGraph> LoadSceneGraphFromFile(const std::string& path, const std::string& name=""){
		boost::shared_ptr<SceneGraph> scenegraph = scenegraphcontroller->CreateSceneGraph(name);
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Created scenegraph from file");

		SceneNode& root = scenegraph->GetRoot();


		TiXmlDocument document(path);
		document.LoadFile();
		const TiXmlElement* level = document.FirstChildElement("level");

		if(level){

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Found <level>");

			const std::string* id = level->Attribute("id");
			SceneNode& levelnode = root.CreateChildNode(id?*id:"");

			std::map<std::string, boost::shared_ptr<SceneNodeProperty> > textures;

			std::map<std::string, SceneNode::SceneNodePtr> sprites;


			//Load all sprites
			const TiXmlElement* spritesheets = level->FirstChildElement("spritesheets");
			if(spritesheets){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Found <spritesheets>");
				
				SceneNode& spritesheetsnode = root.CreateChildNode("spritesheets");
				const TiXmlElement* spritesheet = spritesheets->FirstChildElement("spritesheet");
				while(spritesheet){
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Found <spritesheet>");

					const std::string* id = spritesheet->Attribute("id");
					const std::string* filepath = spritesheet->Attribute("filepath");

					int defaultwidth = 0;
					int defaultheight = 0;
					float filewidth = 1.0f;
					float fileheight = 1.0f;

					if(filepath){

						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "It's using filepath");
						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", *filepath);

						SceneNode& spritesheetnode = spritesheetsnode.CreateChildNode(id?*id:"");
						spritesheet->Attribute("defaultwidth", &defaultwidth);
						spritesheet->Attribute("defaultheight", &defaultheight);
						spritesheet->Attribute("filewidth", &filewidth);
						spritesheet->Attribute("fileheight", &fileheight);

						std::stringstream ss;
						ss<<"creating spritesheet"<<std::endl
						<<"defaultwidth:"<<defaultwidth<<std::endl
						<<"defaultheight:"<<defaultheight<<std::endl;
						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());


						const TiXmlElement* sprite = spritesheet->FirstChildElement("sprite");
						while(sprite){
							const std::string* id = sprite->Attribute("id");
							if(id){
								SceneNode::SceneNodePtr spritenode = spritesheetnode.CreateChildNodePtr(*id);

								int width = defaultwidth;
								int height = defaultheight;
								float u0 = 0.0f;
								float u1 = 1.0f;
								float v0 = 0.0f;
								float v1 = 1.0f;

								sprite->Attribute("width", &width);
								sprite->Attribute("height", &height);
								if(width==0){
									width = defaultwidth;
								}
								if(height==0){
									height = defaultheight;
								}
								sprite->Attribute("u0", &u0);
								sprite->Attribute("u1", &u1);
								sprite->Attribute("v0", &v0);
								sprite->Attribute("v1", &v1);

								boost::shared_ptr<TriangleStrip> sprite(new TriangleStrip(true));
								//create sprite geometry
								sprite->AddVertex(Vertex(0,0,1, u0/filewidth,v0/fileheight));
								sprite->AddVertex(Vertex(width,0,1, u1/filewidth,v0/fileheight));
								sprite->AddVertex(Vertex(0,height,1, u0/filewidth,v1/fileheight));
								sprite->AddVertex(Vertex(width,height,1, u1/filewidth,v1/fileheight));

								std::stringstream ss;
								ss<<"creating sprite"<<*id<<std::endl
								<<"width:"<<width<<std::endl
								<<"height:"<<height<<std::endl
								<<"(u0,v0):"<<"("<<u0/filewidth<<","<<v0/fileheight<<")"<<std::endl
								<<"(u1,v1):"<<"("<<u1/filewidth<<","<<v1/fileheight<<")"<<std::endl;
								signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());


								spritenode->AddSceneNodeProperty("geometry", boost::shared_ptr<SceneNodeProperty>(new GeometryProperty(sprite)));
								spritenode->AddSceneNodeProperty("texture", boost::shared_ptr<SceneNodeProperty>(new TextureProperty(FileSystem::MakeUsrLocalPath(*filepath))));

								sprites[*id]=spritenode;
							}

							sprite = sprite->NextSiblingElement("sprite");
						}
					}
					spritesheet = spritesheet->NextSiblingElement("spritesheet");
				}
			}

			//Load layers
			const TiXmlElement* layers = level->FirstChildElement("layers");
			if(layers){
				const TiXmlElement* layer = layers->FirstChildElement("layer");
				while(layer){

					const std::string* id = layer->Attribute("id");
					SceneNode& layernode = levelnode.CreateChildNode(id?*id:"");

					const TiXmlElement* element = layer->FirstChildElement();
					while(element){

						if(element->Value()=="spritegrid"){
							const std::string* id = element->Attribute("id");
							float horizontalcellspacing = 16.0f;
							float verticalcellspacing = 16.0f;

							float x = 0.0f;
							float y = 0.0f;

							element->Attribute("horizontalcellspacing", &horizontalcellspacing);
							element->Attribute("verticalcellspacing", &verticalcellspacing);


							element->Attribute("x", &x);
							element->Attribute("y", &y);

							SceneNode& spritegridnode = layernode.CreateChildNode(id?*id:"");
							PositionProperty& position = spritegridnode.AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
							Matrix4& m = position.GetPosition();
							//position spritecell in the grid
							//[row, column]
							m(0,3) = x;
							m(1,3) = y;

							TiXmlElement* spritecell = element->FirstChildElement("spritecell");
							while(spritecell){
								const std::string* id = spritecell->Attribute("id");
								const std::string* spriteid = spritecell->Attribute("spriteid");
								float x = 0.0f;
								float y = 0.0f;
								spritecell->Attribute("x", &x);
								spritecell->Attribute("y", &y);

								if(spriteid){
									SceneNode::SceneNodePtr spritenode = sprites[*spriteid]->Clone();
									spritenode->SetName(id?*id:"");
									//SceneNode& spritecellnode = spritegridnode.CreateChildNode(id?*id:"");

									PositionProperty& position = spritenode->AddSceneNodeProperty("position", boost::shared_ptr<PositionProperty>(new PositionProperty()));
									Matrix4& m = position.GetPosition();
									//position spritecell in the grid
									//[row, column]
									m(0,3) = x * horizontalcellspacing;
									m(1,3) = y * verticalcellspacing;

									std::stringstream ss;
									ss<<"placing sprite: "<<*spriteid<<std::endl
									<<"x:"<<m(0,3)<<std::endl
									<<"y:"<<m(1,3)<<std::endl;
									signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", ss.str());


									spritegridnode.AddChildNode(spritenode);
								}
								spritecell = spritecell->NextSiblingElement("spritecell");
							}
						}
						element = element->NextSiblingElement();
					}
					layer = layer->NextSiblingElement("layer");
				}
			}

			//Load characters
			const TiXmlElement* characters = level->FirstChildElement("characters");
			if(characters){
				const TiXmlElement* character = characters->FirstChildElement();
				while(character){
					if(character->Value()=="playercharacter"){
						const TiXmlElement* animations = character->FirstChildElement("animations");
						if(animations)
						{

						}
					}else if(character->Value()=="nonplayercharacter"){
						const TiXmlElement* animations = character->FirstChildElement("animations");
						if(animations)
						{

						}
					}

					character = character->NextSiblingElement();
				}
			}
			root.DeleteChildNodeByName("spritesheets");
		}
		return scenegraph;
	}
};
