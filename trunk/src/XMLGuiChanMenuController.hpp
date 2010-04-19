/*
 * PacmanConfigurationMenuController.hpp
 *
 *  Created on: Jan 20, 2010
 *      Author: asantos
 */

#ifndef XMLGUICHANMENUCONTROLLER_HPP_
#define XMLGUICHANMENUCONTROLLER_HPP_

#include <boost/bimap.hpp>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class XMLGuiChanMenuController:public SignalSubscriber, gcn::Widget{
	void draw(gcn::Graphics*){}
public:
	class MenuItemMouseListener : public gcn::MouseListener
	{
	public:
		typedef void(MenuItemPressed)(const std::string&, gcn::Label* label);
		typedef void(MenuItemReleased)(const std::string&, gcn::Label* label);
		typedef void(MenuItemClicked)(const std::string&, gcn::Label* label);
		typedef void(MenuItemEntered)(const std::string&, gcn::Label* label);
		typedef void(MenuItemExited)(const std::string&, gcn::Label* label);
	private:
		const std::string menuname;
		const std::string name;
		gcn::Label* label;
		SignalBroker& signalbroker;
	public:
		MenuItemMouseListener(const std::string& menuname, const std::string& name, gcn::Label* label, SignalBroker& signalbroker):
			menuname(menuname),
			name(name),
			label(label),
			signalbroker(signalbroker){}
		void mousePressed(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse pressed " << menuname << " - "  << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemPressed>
				( "/"+menuname+"/item/pressed", name, label);
		}
		void mouseReleased(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse released " << menuname << " - "  << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemReleased>
				( "/"+menuname+"/item/released", name, label);
		}
		void mouseClicked(gcn::MouseEvent& mouseEvent) {
			std::stringstream ss;
			ss << "Mouse clicked " << menuname << " - "  << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemClicked>
				( "/"+menuname+"/item/clicked", name, label);
		}
		void mouseEntered(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse entered " << menuname << " - " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemEntered>
				( "/"+menuname+"/item/entered", name, label);
		}
		void mouseExited(gcn::MouseEvent& mouseEvent) {
			std::stringstream ss;
			ss << "Mouse exited " << menuname << " - "  << name << ":" << mouseEvent.getX() << " "<< mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemExited>
				( "/"+menuname+"/item/exited", name, label);
		}
	};

	class KeyboardBindingKeyListener : public gcn::KeyListener
	{
		public:
		typedef void(KeyPressed)(gcn::TextField* label, const std::string& name, int keyvalue);
		typedef void(KeyReleased)(gcn::TextField* label, const std::string& name, int keyvalue);
	private:
		const std::string menuname;
		const std::string name;
		gcn::TextField* textfield;
		SignalBroker& signalbroker;
	public:
		KeyboardBindingKeyListener(const std::string& menuname, const std::string& name, gcn::TextField* textfield, SignalBroker& signalbroker):
			menuname(menuname),
			name(name),
			textfield(textfield),
			signalbroker(signalbroker){}

	    void keyPressed(gcn::KeyEvent& keyEvent)
	    {
	    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				"Key pressed: " + keyEvent.getKey().getValue());

	        signalbroker.InvokeSignal
			<KeyPressed>
			( "/"+menuname+"/textfield/keypressed", textfield, name, keyEvent.getKey().getValue());
	    }

	    void keyReleased(gcn::KeyEvent& keyEvent)
	    {
	    	std::stringstream ss;
	    	ss << "Key released: " << keyEvent.getKey().getValue();
	    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

	        signalbroker.InvokeSignal
			<KeyReleased>
			( "/"+menuname+"/textfield/keyreleased", textfield, name, keyEvent.getKey().getValue());
	    }
	};

	SignalBroker& signalbroker;

	gcn::Gui* gui;
	XmlGui *xmlgui;

	gcn::OpenGLSDLImageLoader* imageLoader;
	gcn::OpenGLGraphics* graphics;
	gcn::SDLInput* input;

	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	std::string menuname;
	const std::string signalnamespace;
	bool menuinitialized;

public:
	XMLGuiChanMenuController(SignalBroker& signalbroker, const std::string& menuname, const std::string& signalnamespace):
		SignalSubscriber(signalbroker, signalnamespace),
		signalbroker(signalbroker),
		gui(new gcn::Gui()),
		xmlgui(new XmlGui()),
		imageLoader(new gcn::OpenGLSDLImageLoader()),
		graphics(new gcn::OpenGLGraphics(640,480)),
		input(new gcn::SDLInput()),

		menuname(menuname),
		signalnamespace(signalnamespace),
		menuinitialized(false)
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"XMLGuiChanMenuController::XMLGuiChanMenuController():this->signalnamespace==" + this->signalnamespace);


		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&XMLGuiChanMenuController::CreateMenuBase, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&XMLGuiChanMenuController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&XMLGuiChanMenuController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/"+menuname+"/item/pressed",
			boost::bind(&XMLGuiChanMenuController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/"+menuname+"/item/released",
			boost::bind(&XMLGuiChanMenuController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/"+menuname+"/item/clicked",
			boost::bind(&XMLGuiChanMenuController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/"+menuname+"/item/entered",
			boost::bind(&XMLGuiChanMenuController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/"+menuname+"/item/exited",
			boost::bind(&XMLGuiChanMenuController::MenuItemExited, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<KeyboardBindingKeyListener::KeyPressed>
		(	"/"+menuname+"/textfield/keypressed",
			boost::bind(&XMLGuiChanMenuController::KeyBindingKeyPressed, this, _1, _2, _3));

		SignalSubscriber::ConnectToSignal
		<KeyboardBindingKeyListener::KeyReleased>
		(	"/"+menuname+"/textfield/keyreleased",
			boost::bind(&XMLGuiChanMenuController::KeyBindingKeyReleased, this, _1, _2, _3));

	}
	virtual ~XMLGuiChanMenuController(){
		delete xmlgui;
		delete gui;

		delete font;
		delete hoverfont;

        delete input;
        delete graphics;
        delete imageLoader;

	}
protected:
	bool IsMenuInitialized()const{
		return menuinitialized;
	}
	void MenuInitialized(){
		menuinitialized = true;
	}
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph)=0;
	virtual void CreateMenuBase(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "CreateMenuBase["+menuname+"]:"+name);

		if(name == signalnamespace){
			gcn::Image::setImageLoader(imageLoader);
			font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
			hoverfont = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/hoverfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing MainMenu XML");

			gui->setGraphics(graphics);
			gui->setInput(input);

			gcn::Widget::setGlobalFont(font);

			CreateMenu(name, scenegraph);
		}
	}
	virtual void SDLEventHandler(SDL_Event event){
		if(menuinitialized){
			input->pushInput(event);
		}
	}
	virtual void Logic(long dt, long t){
		if(menuinitialized){
			gui->logic();
		}
	}
	virtual void MenuItemPressed(const std::string& name, gcn::Label* label)=0;
	virtual void MenuItemReleased(const std::string& name, gcn::Label* label)=0;
	virtual void MenuItemClicked(const std::string& name, gcn::Label* label)=0;
	virtual void MenuItemEntered(const std::string& name, gcn::Label* label)=0;
	virtual void MenuItemExited(const std::string& name, gcn::Label* label)=0;
	virtual void KeyBindingKeyPressed(gcn::TextField* textfield, const std::string& name, int keyvalue)=0;
	virtual void KeyBindingKeyReleased(gcn::TextField* textfield, const std::string& name, int keyvalue)=0;

};

#endif /* XMLGUICHANMENUCONTROLLER_HPP_ */
