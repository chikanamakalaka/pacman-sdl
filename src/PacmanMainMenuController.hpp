/*
 * PacmanMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef PACMANMAINMENUCONTROLLER_HPP_
#define PACMANMAINMENUCONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class PacmanMainMenuController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::Label* newgamelabel;
	MenuItemMouseListener* newgamemouselistener;

	gcn::Label* highscoreslabel;
	MenuItemMouseListener* highscoresmouselistener;

	gcn::Label* creditslabel;
	MenuItemMouseListener* creditsmouselistener;

	gcn::Label* configurationlabel;
	MenuItemMouseListener* configurationmouselistener;

	gcn::Label* quitlabel;
	MenuItemMouseListener* quitmouselistener;

	boost::shared_ptr<SceneGraph> scenegraph;
	bool animationretreat;
	boost::shared_ptr<SceneNode> pill;
	boost::shared_ptr<SceneNode> pacman;
	boost::shared_ptr<SceneNode> inky;
	boost::shared_ptr<SceneNode> blinky;
	boost::shared_ptr<SceneNode> pinky;
	boost::shared_ptr<SceneNode> clyde;

	gcn::Widget* top;



public:
	PacmanMainMenuController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "MainMenu", "MainMenu"),
		signalbroker(signalbroker),
		menuname("MainMenu"),
		animationretreat(true)
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"PacmanMainMenuController::PacmanMainMenuController():this->signalnamespace==" + this->signalnamespace);
	}
	virtual ~PacmanMainMenuController(){

		if(IsMenuInitialized()){
			delete top;
	        /*
			delete newgamemouselistener;
			delete highscoresmouselistener;
			delete creditsmouselistener;
			delete configurationmouselistener;
			delete quitmouselistener;*/
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Pacman MainMenu");

		this->scenegraph = scenegraph;

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);


		try{
			xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/mainmenugui.xml"));
			top = xmlgui->getWidget("top");
			gui->setTop(top);

		}catch(gcn::Exception e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				std::string("gcn::Exception:")+e.getMessage());
		}

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		newgamelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("newgamelabel"));
		highscoreslabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscoreslabel"));
		creditslabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("creditslabel"));
		configurationlabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("configurationlabel"));
		quitlabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("quitlabel"));

		newgamelabel->setFont(font);
		highscoreslabel->setFont(font);
		creditslabel->setFont(font);
		configurationlabel->setFont(font);
		quitlabel->setFont(font);

		newgamemouselistener = new MenuItemMouseListener(menuname, "newgame", newgamelabel, signalbroker);
		highscoresmouselistener = new MenuItemMouseListener(menuname, "enterhighscores", highscoreslabel, signalbroker);
		creditsmouselistener = new MenuItemMouseListener(menuname, "credits", creditslabel, signalbroker);
		configurationmouselistener = new MenuItemMouseListener(menuname, "configuration", configurationlabel, signalbroker);
		quitmouselistener = new MenuItemMouseListener(menuname, "quit", quitlabel, signalbroker);

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

		newgamelabel->addMouseListener(newgamemouselistener);
		highscoreslabel->addMouseListener(highscoresmouselistener);
		creditslabel->addMouseListener(creditsmouselistener);
		configurationlabel->addMouseListener(configurationmouselistener);
		quitlabel->addMouseListener(quitmouselistener);


		{
			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			boost::shared_ptr<SceneNodeProperty> renderable(new RenderableProperty(guichangui));
			scenegraph->GetRoot().AddSceneNodeProperty("renderable", renderable);
		}
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Pacman Main Menu");


		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Getting main menu scene nodes");


		//Menu animation texture changes
		//Process at appropriate interval
		signalbroker.InvokeSignal<TimerView::IntervalHandler>(
				"/timer/setinterval",
				"/pacmanmainmenu/processanimation", boost::bind(&PacmanMainMenuController::ProcessAnimation, this, _1, _2), 5000);


		MenuInitialized();

	}
	void ProcessAnimation(float t, float dt){
		try{
			if(!pill)
				pill =  scenegraph->GetRoot().GetChildNodePtrByName("pill");
			if(!pacman)
				pacman =  scenegraph->GetRoot().GetChildNodePtrByName("pacman");
			if(!blinky)
				blinky =  scenegraph->GetRoot().GetChildNodePtrByName("blinky");
			if(!pinky)
				pinky =  scenegraph->GetRoot().GetChildNodePtrByName("pinky");
			if(!inky)
				inky =  scenegraph->GetRoot().GetChildNodePtrByName("inky");
			if(!clyde)
				clyde =  scenegraph->GetRoot().GetChildNodePtrByName("clyde");
		}catch(std::exception e){
			std::cout<<e.what()<<std::endl;
		}

		animationretreat=!animationretreat;
		if(animationretreat){
			if(pill)
				HideNodeAndDescendants(pill);
			//pacman->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("blink");
			if(blinky)
				blinky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("blink");
			if(pinky)
				pinky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("blink");
			if(inky)
				inky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("blink");
			if(clyde)
				clyde->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("blink");

		}else{
			if(pill)
				ShowNodeAndDescendants(pill);
			//pacman->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("walkleft");
			if(blinky)
				blinky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("walkleft");
			if(pinky)
				pinky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("walkleft");
			if(inky)
				inky->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("walkleft");
			if(clyde)
				clyde->GetSceneNodeProperty<AnimationsProperty>("animations").SelectAnimation("walkleft");
		}
	}
	void MenuItemPressed(const std::string& name, gcn::Label* label){


	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/pacmangamestatecontroller/"+name);
	}
	void MenuItemEntered(const std::string& name, gcn::Label* label){
		std::cout<<"MainMenu MenuItemEntered"<<std::endl;
		label->setFont(hoverfont);
	}
	void MenuItemExited(const std::string& name, gcn::Label* label){
		label->setFont(font);
	}
	void KeyBindingKeyPressed(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}
	void KeyBindingKeyReleased(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}

	void HideNodeAndDescendants(boost::shared_ptr<SceneNode> scenenode){
		if(scenenode->HasSceneNodeProperty("renderable")){
			scenenode->GetSceneNodeProperty<RenderableProperty>("renderable").GetRenderable().SetVisibility(false);
		}
		SceneNode::SceneNodes::iterator itr = scenenode->GetChildNodes().begin();
		for(; itr!=scenenode->GetChildNodes().end();itr++){
			HideNodeAndDescendants(*itr);
		}
	}
	void ShowNodeAndDescendants(boost::shared_ptr<SceneNode> scenenode){
		if(scenenode->HasSceneNodeProperty("renderable")){
			scenenode->GetSceneNodeProperty<RenderableProperty>("renderable").GetRenderable().SetVisibility(true);
		}
		SceneNode::SceneNodes::iterator itr = scenenode->GetChildNodes().begin();
		for(; itr!=scenenode->GetChildNodes().end();itr++){
			ShowNodeAndDescendants(*itr);
		}
	}
};

#endif /* PACMANMAINMENUCONTROLLER_HPP_ */
