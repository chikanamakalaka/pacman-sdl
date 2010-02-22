/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISHIGHSCORESGUICONTROLLER_HPP_
#define TETRISHIGHSCORESGUICONTROLLER_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class TetrisHighScoresGUIController:public SignalSubscriber{
public:
	typedef void(ReloadHighScoresHandler)();
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
			( "/highscoresgui/item/pressed", name, label);
    }
    void mouseReleased(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse released " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
        	ss.str());

    	signalbroker.InvokeSignal
			<MenuItemReleased>
			( "/highscoresgui/item/released", name, label);
    }
    void mouseClicked(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse clicked " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

    	signalbroker.InvokeSignal
			<MenuItemClicked>
			( "/highscoresgui/item/clicked", name, label);
    }
    void mouseEntered(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse entered " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

    	signalbroker.InvokeSignal
			<MenuItemEntered>
			( "/highscoresgui/item/entered", name, label);
    }
    void mouseExited(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse exited " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

        signalbroker.InvokeSignal
			<MenuItemExited>
			( "/highscoresgui/item/exited", name, label);
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

	std::vector<gcn::Label*> highscores;

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

	bool menuinitialized;

public:
	TetrisHighScoresGUIController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "HighScores", "TetrisHighScoresGUIController"),
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
			boost::bind(&TetrisHighScoresGUIController::CreateHighScoresGUI, this, _1, _2));


		signalbroker.ConnectToSignal
		<TetrisHighScoresGUIController::ReloadHighScoresHandler>
		(	"/highscores/reload",
			boost::bind(&TetrisHighScoresGUIController::ReloadHighScores, this));

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&TetrisHighScoresGUIController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisHighScoresGUIController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/highscoresgui/item/pressed",
			boost::bind(&TetrisHighScoresGUIController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/highscoresgui/item/released",
			boost::bind(&TetrisHighScoresGUIController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/highscoresgui/item/clicked",
			boost::bind(&TetrisHighScoresGUIController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/highscoresgui/item/entered",
			boost::bind(&TetrisHighScoresGUIController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/highscoresgui/item/exited",
			boost::bind(&TetrisHighScoresGUIController::MenuItemExited, this, _1, _2));
	}
	virtual ~TetrisHighScoresGUIController(){
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
	virtual void CreateHighScoresGUI(const std::string& name, SceneGraph& scenegraph){
		if(name == "HighScores"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris HighScoresGUI");

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
					std::string("gcn::Exception:") + e.getMessage());
			}

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing HighScores XML");

			xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/highscoresgui.xml"));
			gui->setGraphics(graphics);
			gui->setInput(input);
			gui->setTop(xmlgui->getWidget("top"));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");
			highscores.resize(10);
			highscores[0] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore0"));
			highscores[1] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore1"));
			highscores[2] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore2"));
			highscores[3] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore3"));
			highscores[4] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore4"));
			highscores[5] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore5"));
			highscores[6] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore6"));
			highscores[7] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore7"));
			highscores[8] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore8"));
			highscores[9] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore9"));
			continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));


			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

			continuemouselistener = new MenuItemMouseListener("quit", *continuelabel, signalbroker);
			continuelabel->addMouseListener(continuemouselistener);

			std::string highscoresstr;
			std::stringstream ss;
			TetrisDB tetrisdb;
			std::list<TetrisDB::Score> highscores = tetrisdb.HighScores();
			std::list<TetrisDB::Score>::const_iterator itr = highscores.begin();

			if(itr!=highscores.end()){
				for(int i=0;itr!=highscores.end();itr++,i++){
					this->highscores[i]->setCaption((boost::format("%1% %|10t|%2%") % itr->GetName() % itr->GetScore()).str());
				}
			}


			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph.GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris HighScoresGUI");
			
			menuinitialized=true;

		}
	}
	void ReloadHighScores(){

		std::string highscoresstr;
		std::stringstream ss;
		TetrisDB tetrisdb;
		std::list<TetrisDB::Score> highscores = tetrisdb.HighScores();
		std::list<TetrisDB::Score>::const_iterator itr = highscores.begin();

		if(itr!=highscores.end()){
			for(int i=0;itr!=highscores.end();itr++,i++){
				this->highscores[i]->setCaption((boost::format("%1% %|10t|%2%") % itr->GetName() % itr->GetScore()).str());
			}
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

#endif /* TETRISHIGHSCORESGUICONTROLLER_HPP_ */
