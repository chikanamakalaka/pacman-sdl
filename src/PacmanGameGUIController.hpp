/*
 * PacmanGameGUIController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef PACMANGAMEGUICONTROLLER_HPP_
#define PACMANGAMEGUICONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class PacmanGameGUIController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;

	gcn::Label* score;
	gcn::Label* level;


public:
	PacmanGameGUIController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "PacmanMenu", "PacmanMenu"),
		signalbroker(signalbroker),
		menuname("PacmanMenu")
		{

		SignalSubscriber::ConnectToSignal
		<PacmanLogic::ScoreChangedHandler>
		( 	"/pacman/scorechanged",
			boost::bind(&PacmanGameGUIController::ScoreChanged, this, _1));


		SignalSubscriber::ConnectToSignal
		<PacmanLogic::LevelChangedHandler>
		( 	"/pacman/levelchanged",
			boost::bind(&PacmanGameGUIController::LevelChanged, this, _1));

	}
	virtual ~PacmanGameGUIController(){
		if(IsMenuInitialized()){
			delete font;

			delete input;
			delete graphics;
			delete imageLoader;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading PacmanGUI");

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		imageLoader = new gcn::OpenGLSDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);
		graphics = new gcn::OpenGLGraphics(640,480);
		input = new gcn::SDLInput();

		font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		gcn::Widget::setGlobalFont(font);

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing PacmanGUI XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/pacmangui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		score = dynamic_cast<gcn::Label*>(xmlgui->getWidget("score"));
		level = dynamic_cast<gcn::Label*>(xmlgui->getWidget("level"));

		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded PacmanGUI");

		MenuInitialized();
	}
	void ScoreChanged(int score){
		this->score->setCaption(boost::lexical_cast<std::string>(score));
	}
	void LevelChanged(int level){
		this->level->setCaption(boost::lexical_cast<std::string>(level));
	}

	void MenuItemPressed(const std::string& name, gcn::Label* label){

	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
	}
	void MenuItemEntered(const std::string& name, gcn::Label* label){
	}
	void MenuItemExited(const std::string& name, gcn::Label* label){
	}
	void KeyBindingKeyPressed(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}
	void KeyBindingKeyReleased(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}

};

#endif /* PACMANGAMEGUICONTROLLER_HPP_ */
