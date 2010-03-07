/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISENTERNAMEGUICONTROLLER_HPP_
#define TETRISENTERNAMEGUICONTROLLER_HPP_

#include <boost/format.hpp>

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"

#include "TetrisDB.hpp"

class TetrisEnterNameGUIController:public SignalSubscriber{
public:
	typedef void(RefreshSharedScoreHandler)(int);
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
			( "/enternamegui/item/pressed", name, label);
    }
    void mouseReleased(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse released " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
        	ss.str());

        signalbroker.InvokeSignal
			<MenuItemReleased>
			( "/enternamegui/item/released", name, label);
    }
    void mouseClicked(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse clicked " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

    	signalbroker.InvokeSignal
			<MenuItemClicked>
			( "/enternamegui/item/clicked", name, label);
    }
    void mouseEntered(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse entered " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

        signalbroker.InvokeSignal
			<MenuItemEntered>
			( "/enternamegui/item/entered", name, label);
    }
    void mouseExited(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse exited " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

    	signalbroker.InvokeSignal
			<MenuItemExited>
			( "/enternamegui/item/exited", name, label);
    }
};
	SignalBroker& signalbroker;
	int highscore;

	gcn::SDLInput* input;
	gcn::OpenGLGraphics* graphics;
	gcn::OpenGLSDLImageLoader* imageLoader;

	XmlGui *xmlgui;
	gcn::Gui* gui;
	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	gcn::Label* score;
	gcn::TextField* name;

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

	bool menuinitialized;

public:
	TetrisEnterNameGUIController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "EnterName", "TetrisEnterNameGUIController"),
		signalbroker(signalbroker),
		xmlgui(new XmlGui()),
		gui(new gcn::Gui()),
		menuinitialized(false)
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisEnterNameGUIController::TetrisEnterNameGUIController():this->signalnamespace==" + this->signalnamespace);

		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&TetrisEnterNameGUIController::CreateEnterNameGUI, this, _1, _2));


		signalbroker.ConnectToSignal
		<TetrisEnterNameGUIController::RefreshSharedScoreHandler>
		(	"/tetrisenternamegui/refreshsharedscore",
			boost::bind(&TetrisEnterNameGUIController::RefreshSharedScore, this, _1));
			
			

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&TetrisEnterNameGUIController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisEnterNameGUIController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/enternamegui/item/pressed",
			boost::bind(&TetrisEnterNameGUIController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/enternamegui/item/released",
			boost::bind(&TetrisEnterNameGUIController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/enternamegui/item/clicked",
			boost::bind(&TetrisEnterNameGUIController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/enternamegui/item/entered",
			boost::bind(&TetrisEnterNameGUIController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/enternamegui/item/exited",
			boost::bind(&TetrisEnterNameGUIController::MenuItemExited, this, _1, _2));
	}
	virtual ~TetrisEnterNameGUIController(){
		delete xmlgui;
		delete gui;

		if(menuinitialized){
			delete font;
			delete hoverfont;

			delete input;
			delete graphics;
			delete imageLoader;

			delete continuemouselistener;
		}
	}
protected:
	virtual void CreateEnterNameGUI(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		if(name == "EnterName"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris EnterNameGUI");

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

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing HighScores XML");

			xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/enternamegui.xml"));
			gui->setGraphics(graphics);
			gui->setInput(input);
			gui->setTop(xmlgui->getWidget("top"));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

			score = dynamic_cast<gcn::Label*>(xmlgui->getWidget("score"));
			this->name = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("name"));
			continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));



			continuemouselistener = new MenuItemMouseListener("enteredname", *continuelabel, signalbroker);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

			continuelabel->addMouseListener(continuemouselistener);


			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris EnterNameGUI");
			
			menuinitialized=true;

		}
	}
	void RefreshSharedScore(int score){
		highscore = score;
		this->score->setCaption(boost::lexical_cast<std::string>(score));
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
		if(name == "enteredname"){
			TetrisDB tetrisdb;
			tetrisdb.AddScore(highscore, this->name->getText());

		}
		
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

#endif /* TETRISENTERNAMEGUICONTROLLER_HPP_ */
