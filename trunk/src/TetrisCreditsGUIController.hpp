/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISCREDITSGUICONTROLLER_HPP_
#define TETRISCREDITSGUICONTROLLER_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class TetrisCreditsGUIController:public SignalSubscriber{
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
			( "/creditsgui/item/pressed", name, label);
    }
    void mouseReleased(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse released " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
        	ss.str());

    	signalbroker.InvokeSignal
			<MenuItemReleased>
			( "/creditsgui/item/released", name, label);
    }
    void mouseClicked(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse clicked " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			ss.str());

    	signalbroker.InvokeSignal
			<MenuItemClicked>
			( "/creditsgui/item/clicked", name, label);
    }
    void mouseEntered(gcn::MouseEvent& mouseEvent){
    	std::stringstream ss;
    	ss << "Mouse entered " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

    	signalbroker.InvokeSignal
			<MenuItemEntered>
			( "/creditsgui/item/entered", name, label);
    }
    void mouseExited(gcn::MouseEvent& mouseEvent) {
    	std::stringstream ss;
    	ss << "Mouse exited " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
    		ss.str());

        signalbroker.InvokeSignal
			<MenuItemExited>
			( "/creditsgui/item/exited", name, label);
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

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

	bool menuinitialized;

public:
	TetrisCreditsGUIController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Credits", "TetrisCreditsGUIController"),
		signalbroker(signalbroker),
		xmlgui(new XmlGui()),
		gui(new gcn::Gui()),
		menuinitialized(false)
		{

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisCreditsGUIController::TetrisCreditsGUIController():this->signalnamespace==" + this->signalnamespace);

		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&TetrisCreditsGUIController::CreateCreditsGUI, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&TetrisCreditsGUIController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisCreditsGUIController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/creditsgui/item/pressed",
			boost::bind(&TetrisCreditsGUIController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/creditsgui/item/released",
			boost::bind(&TetrisCreditsGUIController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/creditsgui/item/clicked",
			boost::bind(&TetrisCreditsGUIController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/creditsgui/item/entered",
			boost::bind(&TetrisCreditsGUIController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/creditsgui/item/exited",
			boost::bind(&TetrisCreditsGUIController::MenuItemExited, this, _1, _2));
	}
	virtual ~TetrisCreditsGUIController(){
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
	virtual void CreateCreditsGUI(const std::string& name, SceneGraph& scenegraph){
		if(name == "Credits"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris CreditsGUI");

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
					"gcn::Exception:" + e.getMessage());
			}

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing Credits XML");

			xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/creditsgui.xml"));
			gui->setGraphics(graphics);
			gui->setInput(input);
			gui->setTop(xmlgui->getWidget("top"));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

			continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));


			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

			continuemouselistener = new MenuItemMouseListener("quit", *continuelabel, signalbroker);
			continuelabel->addMouseListener(continuemouselistener);


			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph.GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris HighScoresGUI");
			
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

#endif /* TETRISHIGHSCORESGUICONTROLLER_HPP_ */
