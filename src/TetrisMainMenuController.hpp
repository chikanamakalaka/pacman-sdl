/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISMAINMENUCONTROLLER_HPP_
#define TETRISMAINMENUCONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class TetrisMainMenuController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

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


public:
	TetrisMainMenuController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "MainMenu", "MainMenu"),
		signalbroker(signalbroker),
		menuname("MainMenu")
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisMainMenuController::TetrisMainMenuController():this->signalnamespace==" + this->signalnamespace);
	}
	virtual ~TetrisMainMenuController(){

		if(IsMenuInitialized()){
			delete font;
			delete hoverfont;

			delete newgamemouselistener;
			delete highscoresmouselistener;
			delete creditsmouselistener;
			delete configurationmouselistener;
			delete quitmouselistener;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Pacman MainMenu");

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		try{
			imageLoader = new gcn::OpenGLSDLImageLoader();
			gcn::Image::setImageLoader(imageLoader);
			graphics = new gcn::OpenGLGraphics(640,480);
			input = new gcn::SDLInput();

			font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
			hoverfont = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/hoverfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
			gcn::Widget::setGlobalFont(font);
		}catch(gcn::Exception e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				std::string("gcn::Exception:")+e.getMessage());
		}

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing MainMenu XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/mainmenugui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		newgamelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("newgamelabel"));
		highscoreslabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscoreslabel"));
		creditslabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("creditslabel"));
		configurationlabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("configurationlabel"));
		quitlabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("quitlabel"));

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


		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris Main Menu");

		MenuInitialized();

	}
	void MenuItemPressed(const std::string& name, gcn::Label* label){


	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/tetrisgamestatecontroller/"+name);
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

};

#endif /* TETRISMAINMENUCONTROLLER_HPP_ */
