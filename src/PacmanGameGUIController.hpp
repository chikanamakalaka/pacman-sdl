/*
 * TetrisGameGUIController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISGAMEGUICONTROLLER_HPP_
#define TETRISGAMEGUICONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class TetrisGameGUIController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;

	gcn::Label* score;
	gcn::Label* level;


public:
	TetrisGameGUIController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "PacmanMenu", "PacmanMenu"),
		signalbroker(signalbroker),
		menuname("PacmanMenu")
		{

		SignalSubscriber::ConnectToSignal
		<TetrisLogic::ScoreChangedHandler>
		( 	"/tetris/scorechanged",
			boost::bind(&TetrisGameGUIController::ScoreChanged, this, _1));


		SignalSubscriber::ConnectToSignal
		<TetrisLogic::LevelChangedHandler>
		( 	"/tetris/levelchanged",
			boost::bind(&TetrisGameGUIController::LevelChanged, this, _1));

	}
	virtual ~TetrisGameGUIController(){
		if(IsMenuInitialized()){
			delete font;

			delete input;
			delete graphics;
			delete imageLoader;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading TetrisGUI");

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		imageLoader = new gcn::OpenGLSDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);
		graphics = new gcn::OpenGLGraphics(640,480);
		input = new gcn::SDLInput();

		font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		gcn::Widget::setGlobalFont(font);

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing TetrisGUI XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/tetrisgui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		score = dynamic_cast<gcn::Label*>(xmlgui->getWidget("score"));
		level = dynamic_cast<gcn::Label*>(xmlgui->getWidget("level"));

		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded TetrisGUI");

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

#endif /* TETRISGAMEGUICONTROLLER_HPP_ */
