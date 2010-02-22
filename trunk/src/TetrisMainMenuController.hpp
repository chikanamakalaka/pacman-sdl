/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISMAINMENUCONTROLLER_HPP_
#define TETRISMAINMENUCONTROLLER_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class TetrisMainMenuController:public SignalSubscriber{
private:
	class MenuItemMouseListener : public gcn::MouseListener
	{
	public:
		typedef void(MenuItemPressed)(const std::string&, gcn::Label& label);
		typedef void(MenuItemReleased)(const std::string&, gcn::Label& label);
		typedef void(MenuItemClicked)(const std::string&, gcn::Label& label);
		typedef void(MenuItemEntered)(const std::string&, gcn::Label& label);
		typedef void(MenuItemExited)(const std::string&, gcn::Label& label);
	private:
		const std::string name;
		gcn::Label& label;
		SignalBroker& signalbroker;
	public:
		MenuItemMouseListener(const std::string& name, gcn::Label& label, SignalBroker& signalbroker):
			name(name),
			label(label),
			signalbroker(signalbroker){}
    void mousePressed(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse pressed " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

        signalbroker.InvokeSignal
			<MenuItemPressed>
			( "/mainmenu/item/pressed", name, label);
    }
    void mouseReleased(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse released " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

        signalbroker.InvokeSignal
			<MenuItemReleased>
			( "/mainmenu/item/released", name, label);
    }
    void mouseClicked(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse clicked " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

        signalbroker.InvokeSignal
			<MenuItemClicked>
			( "/mainmenu/item/clicked", name, label);
    }
    void mouseEntered(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse entered " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
        	ss.str());

        signalbroker.InvokeSignal
			<MenuItemEntered>
			( "/mainmenu/item/entered", name, label);
    }
    void mouseExited(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse exited " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

        signalbroker.InvokeSignal
			<MenuItemExited>
			( "/mainmenu/item/exited", name, label);
    }
};
	SignalBroker& signalbroker;

	gcn::SDLInput* input;
	gcn::OpenGLGraphics* graphics;
	gcn::OpenGLSDLImageLoader* imageLoader;

	XmlGui *xmlgui;
	gcn::Gui* gui;
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

	bool menuinitialized;

public:
	TetrisMainMenuController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "MainMenu", "TetrisMainMenuController"),
		signalbroker(signalbroker),
		xmlgui(new XmlGui()),
		gui(new gcn::Gui()),
		menuinitialized(false)
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisMainMenuController::TetrisMainMenuController():this->signalnamespace==" + this->signalnamespace);

		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&TetrisMainMenuController::CreateMainMenu, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&TetrisMainMenuController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisMainMenuController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/mainmenu/item/pressed",
			boost::bind(&TetrisMainMenuController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/mainmenu/item/released",
			boost::bind(&TetrisMainMenuController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/mainmenu/item/clicked",
			boost::bind(&TetrisMainMenuController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/mainmenu/item/entered",
			boost::bind(&TetrisMainMenuController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/mainmenu/item/exited",
			boost::bind(&TetrisMainMenuController::MenuItemExited, this, _1, _2));
	}
	virtual ~TetrisMainMenuController(){
		delete xmlgui;
		delete gui;

		if(menuinitialized){
			delete font;
			delete hoverfont;

			delete input;
			delete graphics;
			delete imageLoader;

			delete newgamemouselistener;
			delete highscoresmouselistener;
			delete creditsmouselistener;
			delete configurationmouselistener;
			delete quitmouselistener;
		}
	}
protected:
	virtual void CreateMainMenu(const std::string& name, SceneGraph& scenegraph){
		if(name == "MainMenu"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris MainMenu");

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

			newgamemouselistener = new MenuItemMouseListener("newgame", *newgamelabel, signalbroker);
			highscoresmouselistener = new MenuItemMouseListener("enterhighscores", *highscoreslabel, signalbroker);
			creditsmouselistener = new MenuItemMouseListener("credits", *creditslabel, signalbroker);
			configurationmouselistener = new MenuItemMouseListener("configuration", *configurationlabel, signalbroker);
			quitmouselistener = new MenuItemMouseListener("quit", *quitlabel, signalbroker);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

			newgamelabel->addMouseListener(newgamemouselistener);
			highscoreslabel->addMouseListener(highscoresmouselistener);
			creditslabel->addMouseListener(creditsmouselistener);
			configurationlabel->addMouseListener(configurationmouselistener);
			quitlabel->addMouseListener(quitmouselistener);


			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph.GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris Main Menu");
			
			menuinitialized=true;

		}
	}
	void SDLEventHandler(SDL_Event event){
		if(menuinitialized){
			input->pushInput(event);
		}
	}
	void Logic(long dt, long t){
		if(menuinitialized){
			gui->logic();
		}
	}
	void MenuItemPressed(const std::string& name, gcn::Label& label){


	}
	void MenuItemReleased(const std::string& name, gcn::Label& label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label& label){
		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/tetrisgamestatecontroller/"+name);
	}
	void MenuItemEntered(const std::string& name, gcn::Label& label){
		label.setFont(hoverfont);
	}
	void MenuItemExited(const std::string& name, gcn::Label& label){
		label.setFont(font);
	}

};

#endif /* TETRISMAINMENUCONTROLLER_HPP_ */
